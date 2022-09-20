#include "funcoes_auxiliares.h"

int main(int argc, char *argv[])
   {
   int nelev = 0, i = 0;
   double elevang = 0.0;
   int var = 0, idx_var = 0;
   
   Radar *radar = NULL;
   Volume *v = NULL;
   Ray *ray = NULL;
   
   radar = RSL_anyformat_to_radar(argv[1], NULL);

   
   for (i = 0; i < MAX_VARS - 1; i++)
      {
      if (NULL != radar->v[i])
         {
         nelev = radar->v[i]->h.nsweeps;
         var++;
         }
      }

   printf("%02d %02d\n", nelev, var);
   return 0;
   }
