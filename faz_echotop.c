/*
@##############################################################################
@
@   NOME         : faz_echotop.c
@
@   FUNCAO       : EchoTop e EchoBase
@
@   PARAMETROS   : (E) 
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

int faz_echotop(struct params_list *lista_parametros, Radar *radar)
   {
   Volume *volume = NULL;
   Cube *cubo = NULL;
   float valor = 0, val1 = 0, val2 = 0;
   
   int nx = 0, ny = 0, nz = 0;
   int radar_x = 0, radar_y = 0, radar_z = 0;
   float dx = 0, dy = 0, dz = 0, altura = 0;
   float bin_temp = 0.0;
   
   char arq_out[MAX_FILENAME];
   int i = 0, j = 0, k = 0, lev = 0, arq = 0, var = 0;
   short int header_size = 0;
   float range = 1000;
   struct header_saida cabecalho;
   FILE *fp = NULL;
   static short int *saida;
   char data_hora[16];
   
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
   lista_parametros->produto = PROD_TOP;

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
         nz = lista_parametros->nz;

#if 0         
         memset(arq_out, 0, MAX_FILENAME*sizeof(char));
         strcpy(arq_out, "teste_cubo.dat");
         
         if ((fp = fopen(arq_out, "w")) == NULL)
            {
            printf("Erro na abertura do arquivo %s de saida\n", arq_out);
            }
         else
            {
            for (k = 0; k < nz; k++)
               {
               for (j = 0; j < ny; j++)
                  {
                  for (i = 0; i < nx; i++)
                     {
                     bin_temp = volume->h.f(cubo->carpi[k]->data[i][j]);
                     if (bin_temp > 150)
                        bin_temp = -99;
//                     if (bin_temp != 0x20000) bin_temp = 50; else bin_temp = -50;
                     (void) fwrite((void *) &bin_temp, sizeof(float), 1, fp);
                     }
                  }
               }
            fclose(fp);
            }

         return 0;
         
#endif         
         if (NULL == cubo)
            {
            printf("Cubo da variavel %s invalida\n",
                   RSL_ftype[lista_parametros->vars[var]]);
            /*Se nao encontrou a variavel pula pra proxima*/
            continue;
            }

         for (lev = 0; lev < lista_parametros->nlevels; lev++)
            {
            valor = (float) lista_parametros->levels[lev];
            
            preenche_cabecalho(&cabecalho, lista_parametros,
                               radar, arq, var, lev);
            
            /*EchoTop*/
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
                  saida[i + nx*j] = volume->h.invf(NOECHO);
                  for (k=(nz-1); k > 0; k--)
                     {
                     val2 = volume->h.f(cubo->carpi[k]->data[i][j]);
                     
                     if (val2 == BADVAL || val2 == APFLAG ||
                         val2 == RFVAL || val2 == NOECHO)
                        {
                        /*Se o valor eh invalido pula pro proximo k*/
                        continue;
                        }
//                     printf("%f\n", val2);
                     if (val2 >= valor)
                        {
                        /*calcula a altura para esse valor de variavel
                        
                        x = (y - yini) / a + xini
                        onde a eh o coef da reta (y2-y1)/(x2-x1)
                        altura eh x e o valor da var eh y
                        
                        */
                        altura = dz*k;
                        saida[i + nx*j] = volume->h.invf(altura + ((float) lista_parametros->base/1000));
                        /*Encontrou o valor, sai do for k*/
                        break;
                        }
                     }
                  }
               }
            
            memset(arq_out, 0, MAX_FILENAME*sizeof(char));
#ifdef DATAHORA            
            sprintf(arq_out, "%s_%s_%05d_%s.dat",
                    lista_parametros->sufixo,
                    RSL_ftype[lista_parametros->vars[var]],
                    lista_parametros->levels[lev], data_hora);
#else
            sprintf(arq_out, "%s_%s_%05d.dat",
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
               for (i = 0; i < nx; i++)
                  {
                  for (j = 0; j < ny; j++)
                     {
                     bin_temp = volume->h.f(saida[i + nx*j]);
                     if (bin_temp > 150)
                        bin_temp = -99;
                     (void) fwrite((void *) &bin_temp, sizeof(float), 1, fp);
                     }
                  }
               fclose(fp);
               }
            }
         RSL_free_cube(cubo);
         }
   
   free((void *) saida);
   
   return RSL_OK;
   
   }
