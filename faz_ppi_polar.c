/*
@##############################################################################
@
@   NOME         : faz_ppi.c
@
@   FUNCAO       : Cria um ppi retangular com base em um arquivo de radar
@
@   PARAMETROS   : (E) Nome do arquivo de entrada, angulo do ppi, variavel
@                  (S) matriz de ppi
@
@   RETORNO      : incluir o diretorio da lib no LD_LIBRARY_PATH
@
@   OBSERVACOES  : gcc faz_cappi.c -I/home/tbiscaro/rsl-v1.40/include \
@                        -L/home/tbiscaro/rsl-v1.40/lib -lm -O0 -lrsl
@
@##############################################################################
*/
#include "funcoes_auxiliares.h"

int faz_ppi_polar(struct params_list *lista_parametros, Radar *radar)
   {
   Volume *volume = NULL;
   Sweep *sweep = NULL;
   Ray *ray = NULL;
   Cappi *ppi = NULL;
   Carpi *ppi_ret = NULL;
   
   unsigned char t_sweep;
   int nx = 0, ny = 0, nz = 0;
   int radar_x = 0, radar_y = 0;
   int maxrays, maxbins, i, j;
   float start_bin, size_bin;
   float bin_temp;
   int pos_x, pos_y;
   float dx = 0, dy = 0;
   char arq_out[MAX_FILENAME];
   int lev = 0, arq = 0, var = 0;
   short int header_size = 0;
   struct header_saida cabecalho;
   FILE *fp = NULL;   
   float elev, azim, range, height;
   char data_hora[16];

   char data[8];
   char hora[4];

   float tmpW = 0.0;
   
   
   memset(&cabecalho, 0, sizeof(struct header_saida));
   header_size = sizeof(struct header_saida);

   nx = lista_parametros->nx;
   ny = lista_parametros->ny;
   radar_x = lista_parametros->radar_x;
   radar_y = lista_parametros->radar_y;
   dx = ((float) lista_parametros->dx) / 1000;
   dy = ((float) lista_parametros->dy) / 1000;
   lista_parametros->produto = PROD_PPI;
   

      monta_data(&radar->h, data_hora, sizeof(data_hora));
      
      for (var = 0; var < lista_parametros->nvars; var++)
         {
         if (radar->v[lista_parametros->vars[var]] == NULL)
            {
            printf("Variavel %s invalida\n",
                   RSL_ftype[lista_parametros->vars[var]]);
            /*Se nao encontrou a variavel pula pra proxima*/
            continue;
            }
         
         volume = radar->v[lista_parametros->vars[var]];
         
         /*corrige o beam_width caso necessario*/
         corrige_param_radar(volume, lista_parametros);
         
         t_sweep = 0;
         for (lev = 0; lev < lista_parametros->nlevels; lev++)
            {
            if (((lista_parametros->levels[lev] - 1) > volume->h.nsweeps)
                || ((lista_parametros->levels[lev] - 1) < 0))
               {
               printf("Sweep %d invalido\n", lista_parametros->levels[lev]);
               continue;               
               }
            sweep = volume->sweep[lista_parametros->levels[lev] - 1];
            if (NULL == sweep)
               {
               /*Se deu erro no sweep pula pro proxima*/
               printf("Sweep %d invalido\n", lista_parametros->levels[lev]);
               continue;
               }

            /*
              sweep nao esta retornando NULL no caso de sweep invalido
              entao e' melhor conferir se ja abriu o total de sweep no
              volume
            */
            if (++t_sweep > volume->h.nsweeps)
               {
               printf("Maximo de sweeps (%d) atingido\n", volume->h.nsweeps);
               /*Ja leu todos os sweeps, sai do for*/
               break;
               }            
            
            memset(arq_out, 0, MAX_FILENAME*sizeof(char));            
            
#ifdef DATAHORA            
            sprintf(arq_out, "%s_%s_%02d_%s.dat",
                    lista_parametros->sufixo,
                    RSL_ftype[lista_parametros->vars[var]],
                    lista_parametros->levels[lev], data_hora);
/*
            sprintf(arq_out, "%s_%s_%02d_%s_%s.dat",
                    lista_parametros->sufixo,
                    RSL_ftype[lista_parametros->vars[var]],
                    lista_parametros->levels[lev], data, hora);
*/
#else


            sprintf(arq_out, "%s_%s_%02d.dat",
                    lista_parametros->sufixo,
                    RSL_ftype[lista_parametros->vars[var]],
                    lista_parametros->levels[lev]);
#endif            
            /*
            Grava o arquivo, comprime caso flag de compressao tenha sido marcado no cabecalho
            No caso de ocorrer erro na compressao, grava sem comprimir
            */

            if ((fp = fopen(arq_out, "w")) == NULL)
               {
               printf("Erro na abertura do arquivo %s de saida\n", arq_out);
               }
            else
               {
               //RSL_sort_rays_in_sweep(sweep);
               (void) fwrite((void *) &sweep->h.elev, sizeof(float), 1, fp);
               tmpW = (float) sweep->h.nrays;               
               (void) fwrite((void *) &tmpW, sizeof(float), 1, fp);
               (void) fwrite((void *) &sweep->h.beam_width,
                             sizeof(float), 1, fp);
               tmpW = (float) sweep->ray[0]->h.gate_size;
               (void) fwrite((void *) &tmpW, sizeof(float), 1, fp);
               for (i = 0; i < sweep->h.nrays - 1; i++)
                  {
                  (void) fwrite((void *) &sweep->ray[i]->h.azimuth,
                                sizeof(float), 1, fp);
                  tmpW = (float) sweep->ray[i]->h.nbins;
                  (void) fwrite((void *) &tmpW, sizeof(float), 1, fp);
                  for (j = 0; j < sweep->ray[i]->h.nbins - 1; j++)
                     {
                     bin_temp = sweep->h.f(sweep->ray[i]->range[j]);
                     if (bin_temp > 150)
                        bin_temp = -99;
                     (void) fwrite((void *) &bin_temp, sizeof(float), 1, fp);
                     }
                  }               
               fclose(fp);
               }
            }
         }
      
   return RSL_OK;
   
   }
