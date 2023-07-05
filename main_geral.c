
#include "funcoes_auxiliares.h"

int main(int argc, char *argv[])
   {
   Radar *radar = NULL;
   int ii;
   Cube *cubo;
   FILE *fp;
   struct params_list lista_parametros;
   int VAR[4] = {DZ_INDEX, CZ_INDEX, VR_INDEX, SW_INDEX};
   int var_idx, i;
   int var = 0, dx = 0, dy = 0, nx = 0, ny = 0, banda=0;
   int dxppi, nxppi, dyppi, nyppi;
   
   
   if (argc != 3)
      {
      printf("Uso: %s <arquivo> <arquivo de parametros>\n", argv[0]);
      return -1;
      }

   fp = fopen(argv[2], "r");
   if (NULL == fp)
      {
      printf("Erro na abertura do arquivo %s\n", argv[2]);
      return -1;
      }
   
   fscanf(fp, "%d", &var);
   fscanf(fp, "%d", &nx);
   fscanf(fp, "%d", &ny);
   fscanf(fp, "%d", &dx);
   fscanf(fp, "%d", &dy);
   fscanf(fp, "%d", &nxppi);
   fscanf(fp, "%d", &nyppi);
   fscanf(fp, "%d", &dxppi);
   fscanf(fp, "%d", &dyppi);
   fscanf(fp, "%d", &banda);
   fclose(fp);

   
   memset(&lista_parametros, 0, sizeof(struct params_list));

   strcpy(lista_parametros.file_list[0], argv[1]);

   radar = RSL_anyformat_to_radar(lista_parametros.file_list[0], NULL);
   if (NULL == radar)
      {
      printf("Erro na abertura do arquivo %s\n",
             lista_parametros.file_list[0]);
      return -1;
      }

   /*aplica filtro de radiais espurias*/
   filtra_raw_data(radar);

   lista_parametros.a = 200.0;
   lista_parametros.b = 1.6;
   
   lista_parametros.nx = nx;
   lista_parametros.ny = ny;
   lista_parametros.nz = 18;
   lista_parametros.dx = dx;
   lista_parametros.dy = dy;
   lista_parametros.dz = 1000;
   lista_parametros.radar_x = lista_parametros.nx/2;
   lista_parametros.radar_y = lista_parametros.ny/2;
   lista_parametros.radar_z = 0;
   lista_parametros.banda = banda;
   lista_parametros.nvars = 1;
   lista_parametros.nlevels = 15;
   if (0 == var)
      {
      lista_parametros.vars[0] = CZ_INDEX;
      }
   else
      {
      lista_parametros.vars[0] = DZ_INDEX;
      }


   lista_parametros.nlevels = 1;
   lista_parametros.levels[0] = 2;
   lista_parametros.produto = PROD_PPI;
   strcpy(lista_parametros.sufixo, "ppi");
   (void) faz_ppi(&lista_parametros, radar);
   return 0;
    
   
   lista_parametros.nlevels = 1;
   lista_parametros.levels[0] = 3000;
   strcpy(lista_parametros.sufixo, "rain_dp");
   lista_parametros.produto = PROD_CHUVA1;
   (void) faz_chuva(&lista_parametros, radar);
   return 0;
   

   lista_parametros.nlevels = 1;
   lista_parametros.levels[0] = 1;
   lista_parametros.produto = PROD_PPI;
   strcpy(lista_parametros.sufixo, "ppi");
   (void) faz_ppi(&lista_parametros, radar);

   lista_parametros.levels[0] = 2000;
   strcpy(lista_parametros.sufixo, "cappi");
   lista_parametros.produto = PROD_CAPPI;
   (void) faz_cappi(&lista_parametros, radar);

   return 0;
   

   
   for (i = 0; i < lista_parametros.nlevels; i++)
      lista_parametros.levels[i] = (i + 2)*1000;
   
   strcpy(lista_parametros.sufixo, "cappi");
   lista_parametros.produto = PROD_CAPPI;
   (void) faz_cappi(&lista_parametros, radar);
   
   lista_parametros.nlevels = 3;
   lista_parametros.levels[0] = 20;
   lista_parametros.levels[1] = 35;
   lista_parametros.levels[2] = 45;
   strcpy(lista_parametros.sufixo, "echotop");
   lista_parametros.produto = PROD_TOP;
   (void) faz_echotop(&lista_parametros, radar);
   
   /*faz VIL entre 1000 e 16000 m*/
   lista_parametros.nlevels = 2;
   lista_parametros.levels[0] = 2000;
   lista_parametros.levels[1] = 16000;
   strcpy(lista_parametros.sufixo, "vil");
   (void) faz_vil(&lista_parametros, radar);
   
   lista_parametros.nx = nxppi;
   lista_parametros.ny = nyppi;
   lista_parametros.nz = 1;
   lista_parametros.dx = dxppi;
   lista_parametros.dy = dyppi;
   lista_parametros.dz = 1000;
   lista_parametros.radar_x = lista_parametros.nx / 2;
   lista_parametros.radar_y = lista_parametros.ny / 2;
   lista_parametros.radar_z = 0;
   lista_parametros.nvars = 2;
   if (0 == var)
      {
      lista_parametros.vars[0] = CZ_INDEX;
      }
   else
      {
      lista_parametros.vars[0] = DZ_INDEX;
      }
   lista_parametros.vars[1] = VR_INDEX;

   lista_parametros.nlevels = 15;
   for (i = 0; i < lista_parametros.nlevels; i++)
      lista_parametros.levels[i] = i + 1;
   
   lista_parametros.produto = PROD_PPI;
   strcpy(lista_parametros.sufixo, "ppi");
   (void) faz_ppi(&lista_parametros, radar);

   lista_parametros.produto = PROD_PPI;
   strcpy(lista_parametros.sufixo, "ppi");
   (void) faz_ppi_polar(&lista_parametros, radar);


   RSL_free_radar(radar);

   
   return 0;
   }
