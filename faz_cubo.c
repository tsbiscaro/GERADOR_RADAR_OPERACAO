/*
@##############################################################################
@
@   NOME         : faz_cubo.c
@
@   FUNCAO       : gera um cubo
@
@   PARAMETROS   : (E) Volume, cabecalho de entrada,
@                      altura do radar (em metros), teto (em metros)
@                  (S) nenhum
@
@   RETORNO      : ponteiro para o cubo
@
@   OBSERVACOES  : gcc faz_echo.c -I/home/tbiscaro/rsl-v1.40/include \
@                        -L/home/tbiscaro/rsl-v1.40/lib -lm -O0 -lrsl
@
@##############################################################################
*/

#include "funcoes_auxiliares.h"

Cube *faz_cubo(Volume *volume, struct params_list *lista_parametros, float alt_radar)
   {
   Cube *cubo = NULL;
   
   float min_altura, max_altura, h;
   int nx = 0, ny = 0, nz = 0;
   int radar_x = 0, radar_y = 0, radar_z = 0;
   float dx = 0, dy = 0, dz = 0;
   int k = 0;
   float lat = 0, lon = 0;
   int hmax = 0;
   float teto = 0;
   
   /*
   calcula a qtd de niveis verticais com base na altura do radar,
   na altura maxima, e no espacamento vertical. O primeiro nivel sera
   sempre um multiplo de "dz" igual ou acima da altura do radar.

   Exemplo = altura do radar = 470 m e dz = 500 m ==> min_altura = 500
             altura do radar = 900 m e dz = 500 m ==> min_altura = 1000
             altura maxima  = 8200 m e dz = 500 m ==> max_altura = 8000   
   */

   /*corrige o beam_width caso necessario*/
   corrige_param_radar(volume, lista_parametros);

   nx = lista_parametros->nx;
   ny = lista_parametros->ny;
   dx = ((float) lista_parametros->dx) / 1000;
   dy = ((float) lista_parametros->dy) / 1000;
   dz = ((float) lista_parametros->dz) / 1000;
   radar_x = lista_parametros->radar_x;
   radar_y = lista_parametros->radar_y;
   radar_z = lista_parametros->radar_z;
   
   
   min_altura = (((int)((alt_radar - 1) / (dz*1000)) + 1) * (dz*1000));
   max_altura = 20000;
   
   /*
   if (teto > 0)
      {
      max_altura = (((int)(teto / (dz*1000))) * dz*1000);
      }
   else
      {
      max_altura = (((int)(acha_altura_cubo(volume) / (dz*1000))) * dz*1000);
      }
   */
   
   /*atualiza o nz e a altura da base no cabecalho*/
   nz = 1 + (int) ((max_altura - min_altura) / (dz*1000));
   lista_parametros->nz = nz;
   lista_parametros->base = min_altura;

   cubo = (Cube *) RSL_new_cube(nz);
   if (NULL == cubo)
      {
      return NULL;
      }

   cubo->nx = nx;
   cubo->ny = ny;
   cubo->nz = nz;
   cubo->dx = dx;
   cubo->dy = dy;
   cubo->dz = dz;
   
   /*monta o cubo*/
   for (k = 0; k < nz; k++)
      {
      h = (min_altura + (k*dz*1000)) - alt_radar;
      cubo->carpi[k] = (Carpi *) RSL_volume_to_carpi(volume, h/1000, 1000,
                                                     dx, dy, nx, ny,
                                                     radar_x, radar_y,
                                                     lat, lon);
      }   
   return cubo;
   }
