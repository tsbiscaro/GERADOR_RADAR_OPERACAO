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

int faz_cappi(struct params_list *lista_parametros, Radar *radar)
   {
   Volume *volume = NULL;
   Carpi *cappi = NULL;
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

   
   float bin_temp;
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
   
      
/*      
      
fp = fopen("mascara.bin", "r");
fread((void *) in, 100*100*sizeof(short int), 1, fp);
fclose(fp);

lat_radar = fabs(radar->h.latd) + (float) radar->h.latm/60 + (float) radar->h.lats/3600;
lon_radar = fabs(radar->h.lond) + (float) radar->h.lonm/60 + (float) radar->h.lons/3600;
      
if (radar->h.latd < 0)
{
lat_radar = -lat_radar;
}
      
if (radar->h.lond < 0)
{
lon_radar = -lon_radar;
}
mask = interpola_mascara(in, 100, 100, 0.05, 0.05, -47, -21, lista_parametros, radar);

fp = fopen("mascara_proc.bin", "w");
fwrite((void *) mask, 400*400*sizeof(char), 1, fp);
fclose(fp);
free(mask);
      
return 0;
*/      
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
//         RSL_load_refl_color_table();
//         RSL_sweep_to_gif(volume->sweep[0], "testegif", 1000, 1000, 1000); 
         

      /*corrige o beam_width caso necessario*/
      corrige_param_radar(volume, lista_parametros);
      (void) filtra_volume(volume, lista_parametros);
         
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
            
         cappi = RSL_volume_to_carpi(volume, h, range,
                                     dx, dy, nx, ny,
                                     radar_x, radar_y, 0, 0);
            
         if (NULL == cappi)
            {
            /*Se deu erro no cappi pula pro proxima*/
            printf("Cappi invalido\n");
            continue;
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
                 RSL_ftype[lista_parametros->vars[var]],
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
                  bin_temp = cappi->f(cappi->data[i][j]);
                  if (bin_temp > 150)
                     bin_temp = -99;
                  (void) fwrite((void *) &bin_temp, sizeof(float), 1, fp);
                  }
               }
            fclose(fp);
            }
         RSL_free_carpi(cappi);
         }
      }
   
   return RSL_OK;
   
   }
