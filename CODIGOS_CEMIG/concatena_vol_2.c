#include "funcoes_auxiliares.h"
#include <stdio.h>

#define MAX_VOL 47
#define MAX_SWP 10
#define MAX_RAY 360
int main(int argc, char *argv[])
   {
   
   int nelev = 0, i = 0, j = 0;
   double elevang = 0.0;
   
   Radar *radar = NULL;
   Radar *radar2 = NULL;
   Radar *radar_out = NULL;
   char data_hora[16];
   char lista_nome[256];
   char arquivo[32];
   char arquivo_out[32];
   char *linha = NULL;
   size_t len;
   FILE *f_lista=NULL;
   char nome_out[64];
   
   
   radar_out = RSL_new_radar(MAX_VOL);
   for (i = 0; i < MAX_VOL; i++)
      {
      radar_out->v[i] = RSL_new_volume(MAX_SWP);
      for (j = 0; j < MAX_SWP; j++)
         {
         radar_out->v[i]->sweep[j] = RSL_new_sweep(MAX_RAY);
         }
      }


   memset(lista_nome, 0, sizeof(lista_nome));
   memset(arquivo, 0, sizeof(arquivo));
   
   f_lista = fopen(argv[1], "r");
   if (NULL == f_lista)
      {
      printf("Erro %s\n", argv[1]);
      return 1;
      }

   /*abre o primeiro arquivo*/
   memset(lista_nome, 0, sizeof(lista_nome));
   memset(arquivo, 0, sizeof(arquivo));
   getline(&linha, &len, f_lista);
   strncpy(arquivo, linha, 23);
   printf("Arquivo 1: %s\n", arquivo);
   strncpy(arquivo_out, linha, 23);
   radar = NULL;
   radar = RSL_anyformat_to_radar(arquivo, NULL);


   /*abre o arquivo de saida*/
   memset(nome_out, 0, sizeof(nome_out));
   sprintf(nome_out, "%s_OUT.UF.gz", arquivo_out);
   radar_out->h = radar->h;
   for (i = 0; i < 4; i++)
      {
      for (j = 0; j < MAX_VOL; j++)
         {
         if (NULL != radar->v[j])
            {
            radar_out->v[j]->h = radar->v[j]->h;
            radar_out->v[j]->sweep[i] = RSL_copy_sweep(radar->v[j]->sweep[i]);
            radar_out->v[j]->h.nsweeps = 10;
            }
         }      
      }
    
   

   /*abre o segundo arquivo*/
   memset(arquivo, 0, sizeof(arquivo));
   getline(&linha, &len, f_lista);
   strncpy(arquivo, linha, 23);
   printf("Arquivo 2: %s\n", arquivo);
   radar2 = NULL;
   radar2 = RSL_anyformat_to_radar(arquivo, NULL);


   for (i = 0; i < 6; i++)
      {
      for (j = 0; j < MAX_VOL; j++)
         {
         if (NULL != radar->v[j])
            {
            radar_out->v[j]->sweep[i+4] = RSL_copy_sweep(radar2->v[j]->sweep[i]);
            }
         }      
      }

   
   fclose(f_lista);
   
   RSL_radar_to_uf_gzip(radar_out, nome_out);
   return 0;
   }
