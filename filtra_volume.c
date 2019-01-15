/*
@##############################################################################
@
@   NOME         : filtra_volume.c
@
@   FUNCAO       : Aplica o filtro de remocao de ruidos no volume
@
@   PARAMETROS   : (E) Ponteiro para o volume
@                  (S) 
@
@   RETORNO      : incluir o diretorio da lib no LD_LIBRARY_PATH
@
@   OBSERVACOES  : gcc faz_cappi.c -I/home/tbiscaro/rsl-v1.40/include \
@                        -L/home/tbiscaro/rsl-v1.40/lib -lm -O0 -lrsl
@
@##############################################################################
*/
#include "funcoes_auxiliares.h"


int filtra_volume(Volume *v, struct params_list *lista_parametros)
   {
   int i = 0, j = 0, t_sweep = 0;
   float slant_r, elev, ground_r, h;
   float limit, azim;
   Ray *ray0, *ray1, *ray2;
   float elev0, elev1, elev2, value0, value1, value2;
   int bin, bin1, bin2;
   int maxrays, rayidx;
   int cont_bin = 0;
   return 0;
   
   if (v->h.nsweeps < 3)
      {
      printf("Necessario no minimo 3 niveis para a filtragem\n");
      return -1;
      /*necessario ter 3 niveis no minimo*/
      }
   
   /*compara as 3 primeiras elevacoes*/

   maxrays = v->sweep[0]->h.nrays;
   if (v->sweep[1]->h.nrays < maxrays) maxrays = v->sweep[1]->h.nrays;
   if (v->sweep[2]->h.nrays < maxrays) maxrays = v->sweep[2]->h.nrays;
   
   limit =  v->sweep[0]->h.beam_width;
   
   for (rayidx = 0; rayidx < maxrays; rayidx++)
      {
      ray0 =  v->sweep[0]->ray[rayidx];

      if (NULL == ray0) continue;
      
      elev0 = ray0->h.elev;
      azim = ray0->h.azimuth;
      
      ray1 = RSL_get_closest_ray_from_sweep(v->sweep[1], azim, limit);
      if (NULL == ray1) continue;
      elev1 = ray1->h.elev;
      
      ray2 = RSL_get_closest_ray_from_sweep(v->sweep[2], azim, limit);      
      if (NULL == ray2) continue;
      elev2 = ray2->h.elev;
      
      for (bin = 0; bin < v->sweep[0]->ray[rayidx]->h.nbins; bin++)
         {
//         printf("%03d %03d %03d\n", rayidx, bin, v->sweep[1]->ray[359]->h.nbins);
         value0 = v->h.f(ray0->range[bin]);
         slant_r = ((ray0->h.gate_size * bin) + ray0->h.range_bin1) / 1000;
         
         /*acha a distancia do ponto em relacao ao solo*/
         RSL_get_groundr_and_h(slant_r, elev0, &ground_r, &h);
         
         /*acha nos sweeps acima o ponto correspondente*/
         RSL_get_slantr_and_h(ground_r, elev1, &slant_r, &h);
         value1 = RSL_get_value_from_ray(ray1, slant_r);
         bin1 = (slant_r*1000 - ray1->h.range_bin1) /  ray1->h.gate_size;

         RSL_get_slantr_and_h(ground_r, elev2, &slant_r, &h);
         value2 = RSL_get_value_from_ray(ray2, slant_r);
         bin2 = (slant_r*1000 - ray2->h.range_bin1) /  ray2->h.gate_size;

         cont_bin = 0;
         if (value0 != BADVAL && value0 != APFLAG &&
             value0 != RFVAL && value0 != NOECHO)
            {
            cont_bin++;
            }
         if (value1 != BADVAL && value1 != APFLAG &&
             value1 != RFVAL && value1 != NOECHO)
            {
            cont_bin++;
            }
         if (value2 != BADVAL && value2 != APFLAG &&
             value2 != RFVAL && value2 != NOECHO)
            {
            cont_bin++;
            }
         
         if (cont_bin < 3)
            {
            ray0->range[bin] = v->h.invf(NOECHO);
            ray1->range[bin1] = v->h.invf(NOECHO);
            ray1->range[bin2] = v->h.invf(NOECHO);
            }
         }
      
      }
   return 0;
   }
