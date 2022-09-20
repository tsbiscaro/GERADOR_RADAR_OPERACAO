#include "funcoes_auxiliares.h"

int main(int argc, char *argv[])
   {
   int nelev = 0, i = 0;
   double elevang = 0.0;
   
   Radar *radar = NULL;

   radar = RSL_anyformat_to_radar(argv[1], NULL);

   nelev = radar->v[1]->h.nsweeps;
   printf("%02d\n", nelev);

   return 0;
   }
