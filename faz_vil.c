/*
@##############################################################################
@
@   NOME         : faz_vil.c
@
@   FUNCAO       : Conteudo de agua liquida na camada
@
@   PARAMETROS   : (E) Nome do arquivo de entrada
@                  (S) matriz de alturas
@
@   RETORNO      : incluir o diretorio da lib no LD_LIBRARY_PATH
@
@   OBSERVACOES  : gcc faz_echo.c -I/home/tbiscaro/rsl-v1.40/include \
@                        -L/home/tbiscaro/rsl-v1.40/lib -lm -O0 -lrsl
@
@##############################################################################
*/

#include "funcoes_auxiliares.h"

int faz_vil(struct params_list *lista_parametros,  Radar *radar)
   {
  
   Volume *volume = NULL;
   Cube *cubo = NULL;
   
   int nx = 0, ny = 0, nz = 0;
   int radar_x = 0, radar_y = 0, radar_z = 0;
   float dx = 0, dy = 0, dz = 0;
   char arq_out[MAX_FILENAME];
   int i = 0, j = 0, k = 0, lev = 0, arq = 0, var = 0;
   short int header_size = 0;
   float range = 1000;
   float bin_temp0 = 0, bin_temp1 = 0, vil_temp = 0;
   int camada0, camada1, ind_camada0, ind_camada1;
   struct header_saida cabecalho;
   FILE *fp = NULL;
   static short int *saida;
   char data_hora[16];
   float bin_temp;
   
   memset(&cabecalho, 0, sizeof(struct header_saida));
   header_size = sizeof(struct header_saida);
   
   nx = lista_parametros->nx;
   ny = lista_parametros->ny;
   nz = lista_parametros->nz;
   dx = ((float) lista_parametros->dx) / 1000;
   dy = ((float) lista_parametros->dy) / 1000;
   dz = ((float) lista_parametros->dz) / 1000;
   radar_x = lista_parametros->radar_x;
   radar_y = lista_parametros->radar_y;
   radar_z = lista_parametros->radar_z;
   lista_parametros->produto = PROD_VIL;
   
   saida = aloca_matriz_16(nx, ny);

   if (NULL == saida)
      {
      printf("Memoria insuficiente\n");
      return RSL_ERR;
      }


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
/*
         cubo = RSL_volume_to_cube(volume, dx, dy, dz, nx, ny, nz,
                                   range, radar_x, radar_y, radar_z);
*/
         
         cubo = faz_cubo(volume, lista_parametros, radar->h.height);

         if (NULL == cubo)
            {
            printf("Cubo da variavel %s invalida\n",
                   RSL_ftype[lista_parametros->vars[var]]);
            /*Se nao encontrou a variavel pula pra proxima*/
            continue;
            }

         nz = lista_parametros->nz;         
         /*
         valores de camada em metros
         */
         camada0 = lista_parametros->levels[0];
         camada1 = lista_parametros->levels[1];

         /*verifica se as camadas escolhidas estao no limite do cubo*/
         if (camada0 < lista_parametros->base)
            {
            camada0 = lista_parametros->base;
            }
         
         if (camada1 > (nz*dz*1000 + lista_parametros->base))
            {
            camada1 = nz*dz*1000 + lista_parametros->base;
            }

         /*normaliza as camadas no intervalo do cubo*/
         ind_camada0 = (int) ((camada0-lista_parametros->base) / (dz*1000));
         ind_camada1 = (int) ((camada1-lista_parametros->base) / (dz*1000));
         
         camada0 = ind_camada0*dz*1000 + lista_parametros->base;
         camada1 = ind_camada1*dz*1000 + lista_parametros->base;

         lista_parametros->levels[0] = camada0;
         lista_parametros->levels[1] = camada1;
         
         preenche_cabecalho(&cabecalho, lista_parametros,
                            radar, arq, var, lev);

         for (i=0; i < nx; i++)
            {
            for (j=0; j < ny; j++)
               {
               /*
               if (verifica_coluna(cubo, i, j, nz) == RSL_ERR)
                  {
                  saida[i + nx*j] = volume->h.invf(BADVAL);
                  continue;
                  }
               */
               vil_temp = 0;
               for (k = ind_camada0; k < (ind_camada1 - 1); k++)
                  {
                  bin_temp0 = cubo->carpi[k]->f(cubo->carpi[k]->data[i][j]);
                  bin_temp1 = cubo->carpi[k+1]->f(cubo->carpi[k+1]->data[i][j]);
                  
                  if (bin_temp0 != BADVAL && bin_temp0 != APFLAG &&
                      bin_temp0 != RFVAL && bin_temp0 != NOECHO &&
                      bin_temp1 != BADVAL && bin_temp1 != APFLAG &&
                      bin_temp1 != RFVAL && bin_temp1 != NOECHO)
                     {
                     vil_temp += calcula_vil(bin_temp0, bin_temp1, dz*1000);
                     }
                  }
               if (vil_temp > 0)
                  saida[i + nx*j] = volume->h.invf(vil_temp);
               else
                  saida[i + nx*j] = volume->h.invf(NOECHO);                     
               }
            }
         
         memset(arq_out, 0, MAX_FILENAME*sizeof(char));

#ifdef DATAHORA         
         sprintf(arq_out, "%s_%s_%05d-%05d_%s.dat",
                 lista_parametros->sufixo,
                 RSL_ftype[lista_parametros->vars[var]],
                 camada0, camada1, data_hora);
#else
         sprintf(arq_out, "%s_%s_%05d-%05d.dat",
                 lista_parametros->sufixo,
                 RSL_ftype[lista_parametros->vars[var]],
                 camada0, camada1);
         
#endif

         if ((fp = fopen(arq_out, "w")) == NULL)
            {
            printf("Erro na abertura do arquivo %s de saida\n", arq_out);
            }
         else
            {
               for (i = 0; i < nx; i++)
                  {
                  for (j = 0; j < ny; j++)
                  {
                  bin_temp = volume->h.f(saida[i + nx*j]);
                  if (bin_temp > 1500)
                     bin_temp = -99;
                  (void) fwrite((void *) &bin_temp, sizeof(float), 1, fp);
                  }
               }
            fclose(fp);
            }
         
         
         RSL_free_cube(cubo);
         }
   
   free((void *) saida);
   
   return RSL_OK;
   
   }
