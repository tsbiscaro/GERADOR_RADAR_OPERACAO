/*
@##############################################################################
@
@   NOME         : faz_cappi.c
@
@   FUNCAO       : Cria um cappi retangular com base em um arquivo de radar
@
@   PARAMETROS   : (E) Nome do arquivo de entrada, altura do CAPPI, variavel
@                  (S) Matriz do CAPPI
@
@   RETORNO      : incluir o diretorio da lib no LD_LIBRARY_PATH
@
@   OBSERVACOES  : gcc faz_cappi.c -I/home/tbiscaro/rsl-v1.40/include \
@                        -L/home/tbiscaro/rsl-v1.40/lib -lm -O0 -lrsl
@
@##############################################################################
*/
#include "funcoes_auxiliares.h"

int faz_chuva(struct params_list *lista_parametros, Radar *radar)
   {
   Volume *volumeZDR = NULL, *volumeDBZ = NULL,
      *volumeKDP = NULL, *volumeCZ = NULL, *volumeDZ = NULL;
   Carpi *cappiZDR = NULL, *cappiDBZ = NULL, *cappiKDP = NULL;
   float range = 1000;
   float h = 0;
   int i, j;
   int nx = 0, ny = 0, nz = 0;
   int radar_x = 0, radar_y = 0, radar_z = 0;
   float dx = 0, dy = 0, dz = 0;
   char arq_out[MAX_FILENAME];
   int lev = 0, arq = 0, var = 0;
   short int header_size = 0;
   struct header_saida cabecalho;
   FILE *fp = NULL;
   char data_hora[16];
   char data[8];
   char hora[4];
   float a = 200, b = 1.6;
   char zdr_ok = 1, kdp_ok = 1;
   char banda = 0;
   
   
   float bin_temp, dbz, zdr, kdp;
   /*
   char *mask = NULL;
   char in[100*100];
   */
      
   memset(&cabecalho, 0, sizeof(struct header_saida));
   header_size = sizeof(struct header_saida);
   nx = lista_parametros->nx;
   ny = lista_parametros->ny;
   radar_x = lista_parametros->radar_x;
   radar_y = lista_parametros->radar_y;
   
   dx = ((float) lista_parametros->dx) / 1000;
   dy = ((float) lista_parametros->dy) / 1000;
   
      

   banda = lista_parametros->banda;
   a = lista_parametros->a;
   b = lista_parametros->b;
   
   monta_data(&radar->h, data_hora, sizeof(data_hora));
      
   for (var = 0; var < MAX_VARS - 1; var++)
      {
      switch(var)
         {
         case DZ_INDEX:            
            volumeDZ = radar->v[var];
            /*corrige o beam_width caso necessario*/
            corrige_param_radar(volumeDZ, lista_parametros);
            (void) filtra_volume(volumeDZ, lista_parametros);
            break;
         case CZ_INDEX:            
            volumeCZ = radar->v[var];
            /*corrige o beam_width caso necessario*/
            corrige_param_radar(volumeCZ, lista_parametros);
            (void) filtra_volume(volumeCZ, lista_parametros);
            break;
         case DR_INDEX:
            volumeZDR = radar->v[var];
            if (NULL == volumeZDR)
               {
               zdr_ok = 0;
               break;
               }
            corrige_param_radar(volumeZDR, lista_parametros);
            (void) filtra_volume(volumeZDR, lista_parametros);
            break;
         case KD_INDEX:
            volumeKDP = radar->v[var];
            if (NULL == volumeKDP)
               {
               kdp_ok = 0;
               break;
               }
            corrige_param_radar(volumeKDP, lista_parametros);
            (void) filtra_volume(volumeKDP, lista_parametros);
            break;            
         }

      }

   if (CZ_INDEX == lista_parametros->vars[0])
      {
      volumeDBZ = volumeCZ;
      }
   else
      {
      volumeDBZ = volumeDZ;
      }
   
   for (lev = 0; lev < lista_parametros->nlevels; lev++)
      {
      /*
      Verfica se o nivel pedido para o cappi esta
      acima do radar
      */
      if (radar->h.height > lista_parametros->levels[lev])
         {
         printf("Altura de cappi [%d (m)] invalida\n",
                lista_parametros->levels[lev]);
         continue;
         }
      
      /*
      Passa para o RSL o valor acima do radar, ja que o RSL
      calcula o cappi considerando o radar com altura zero
      */
      
      h = ((float) (lista_parametros->levels[lev] -
                    radar->h.height)) / 1000;
      
      cappiDBZ = RSL_volume_to_carpi(volumeDBZ, h, range,
                                     dx, dy, nx, ny,
                                     radar_x, radar_y, 0, 0);
      
      if (NULL == cappiDBZ)
         {
         /*Se deu erro no cappi pula pro proxima*/
         printf("Cappi DBZ invalido\n");
         continue;
         }

      /*verifica se tem zdr e kdp no volume*/
      if ((1 == zdr_ok) && (1 == kdp_ok))
         {
         cappiZDR = RSL_volume_to_carpi(volumeZDR, h, range,
                                        dx, dy, nx, ny,
                                        radar_x, radar_y, 0, 0);
         
         if (NULL == cappiZDR)
            {
            /*Se deu erro no cappi pula pro proxima*/
            printf("Cappi ZDR invalido\n");
            continue;
            }
      
         cappiKDP = RSL_volume_to_carpi(volumeKDP, h, range,
                                        dx, dy, nx, ny,
                                        radar_x, radar_y, 0, 0);
         
         if (NULL == cappiKDP)
            {
            /*Se deu erro no cappi pula pro proxima*/
            printf("Cappi KDP invalido\n");
            continue;
            }
         }
      
   
      preenche_cabecalho(&cabecalho, lista_parametros,
                         radar, arq, var, lev);
      
      memset(arq_out, 0, MAX_FILENAME*sizeof(char));
      
      memset(data, 0, 8*sizeof(char));
      memset(hora, 0, 4*sizeof(char));
      memcpy(data, &lista_parametros->file_list[arq][8], 8);
      memcpy(hora, &lista_parametros->file_list[arq][17], 4);
      
      
      
      
#ifdef DATAHORA            
      sprintf(arq_out, "%s_%s_%05d_%s.dat",
              lista_parametros->sufixo,
              RSL_ftype[lista_parametros->vars[0]],
              lista_parametros->levels[lev], data_hora);
/*
sprintf(arq_out, "%s_%s_%05d_%s_%s.dat",
lista_parametros->sufixo,
RSL_ftype[lista_parametros->vars[var]],
lista_parametros->levels[lev], data, hora);
*/
#else
      sprintf(arq_out, "%s_%s_%05d.dat",
              lista_parametros->sufixo,
              RSL_ftype[lista_parametros->vars[0]],
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
               dbz = cappiDBZ->f(cappiDBZ->data[i][j]);

               if (dbz > 150)
                  {
                  dbz = -9999;
                  (void) fwrite((void *) &dbz, sizeof(float), 1, fp);
                  }
               else
                  {
                  if ((0 == zdr_ok) || (0 == kdp_ok))
                     {
                     bin_temp = calcula_chuva_single_pol(dbz, a, b);
                     }
                  else
                     {
                     zdr = cappiZDR->f(cappiZDR->data[i][j]);
                     kdp = cappiKDP->f(cappiKDP->data[i][j]);
                     
                     if (zdr > 150)
                        zdr = 0;
                     if (kdp > 150)
                        kdp = 0;
                     
                     /*calcula Dual Pol - Banda vem do arquivo de parametro
                     0 = S, 1 = C, 2 = X
                     */
                     bin_temp = calcula_chuva_dual_pol(dbz, zdr,
                                                       kdp, a, b, banda);
                     }
                  
                  (void) fwrite((void *) &bin_temp, sizeof(float), 1, fp);
                  }
               }
            }
         fclose(fp);
         }
      RSL_free_carpi(cappiDBZ);
      RSL_free_carpi(cappiZDR);
      RSL_free_carpi(cappiKDP);
      }

   return RSL_OK;
   
   }
