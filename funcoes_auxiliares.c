#include "funcoes_auxiliares.h"
/*
@##############################################################################
@
@   NOME         : 
@
@   FUNCAO       : 
@
@   PARAMETROS   : (E) 
@                  (S) 
@
@   RETORNO      : 
@
@   OBSERVACOES  : 
@
@##############################################################################
*/

char le_parametros_entrada(char *arquivo, int *parametros,
                           int *alturas, int *var, char *lista)
   {
   FILE *fPar = NULL;
   char buffer[256];
   int i = 0;
   int j = 0;
   int k = 0;
   int l = 0;
   
   memset(buffer, 0, sizeof(buffer));
   memset(parametros, 0, sizeof(parametros));
   
   fPar = fopen(arquivo, "r");

   if (fPar == NULL)
      {
      printf("Arquivo de parametros invalido\n");
      return 1;
      }
   
   while(fgets(buffer, 256, fPar) > (char *) NULL)
      {
      if ((buffer[0] != '#') && (buffer[0] != '\n') && (buffer[0] != ' '))
         {
         if (i < 6)
            {
            /*as 6 primeiras linhas sao as informacoes*/
            parametros[i++] = atoi(buffer);
            }
         else
            {
            /*a partir da sexta linha comeca a lista das variaveis a
              serem extraidas, depois a qtd de alturas, depois os arquivos*/
            if (j < parametros[4])
               {   
               alturas[j++] = atoi(buffer);
               }
            else
               {
               if (k < parametros[5])
                  {   
                  var[k++] = atoi(buffer);
                  }
               else
                  {
                  strcpy(&lista[l++], buffer);
                  }
               }
            }
         }
      memset(buffer, 0, sizeof(buffer));
      }
   fclose(fPar);

   if ((j != parametros[4]) || (k != parametros[5]) || (i != 6))
      {   
      printf("Arquivo de parametros invalido\n");
      return 1;
      }
   
   

   return 0;
   }


void imprime_lista_var(void)
   {
   printf(" 0 = DZ_INDEX = reflectivity.\n");
   printf(" 1 = VR_INDEX = velocity.\n");
   printf(" 2 = SW_INDEX = spectrum_width.\n");
   printf(" 3 = CZ_INDEX = corrected reflectivity.\n");
   printf(" 4 = ZT_INDEX = total reflectivity.\n");
   printf(" 5 = DR_INDEX = differential refl.\n");
   printf(" 6 = LR_INDEX = another differential refl.\n");
   printf(" 7 = ZD_INDEX = another refl form.\n");
   printf(" 8 = DM_INDEX = recieved power.\n");
   printf(" 9 = RH_INDEX = Rho coefficient.\n");
   printf("10 = PH_INDEX = Phi (MCTEX parameter).\n");
   printf("11 = XZ_INDEX = X-band reflectivity.\n");
   printf("12 = CR_INDEX = Corrected DR.\n");
   printf("13 = MZ_INDEX = DZ mask for 1C-51 HDF.\n");
   printf("14 = MR_INDEX = DR mask for 1C-51 HDF.\n");
   printf("15 = ZE_INDEX = Edited reflectivity.\n");
   printf("16 = VE_INDEX = Edited velocity.\n");
   printf("17 = KD_INDEX = KDP (unknown)  for MCTEX data.\n");
   printf("18 = TI_INDEX = TIME (unknown)  for MCTEX data.\n");
   printf("19 = DX_INDEX\n");
   printf("20 = CH_INDEX\n");
   printf("21 = AH_INDEX\n");
   printf("22 = CV_INDEX\n");
   printf("23 = AV_INDEX\n");
   printf("24 = SQ_INDEX = Signal Quality Index (Sigmet)\n");
   }

/*
@##############################################################################
@
@   NOME         : imprime_lista_variaveis
@
@   FUNCAO       : Apresenta a lista de variaveis disponiveis na struct radar
@
@   PARAMETROS   : (E) ponteiro para struct radar
@                  (S) nenhum
@
@   RETORNO      : nenhum
@
@   OBSERVACOES  : 
@
@##############################################################################
*/

void imprime_lista_variaveis(Radar *radar)
   {
   int i;

   printf("Variaveis no arquivo:\n");

   for (i = 0; i < MAX_RADAR_VOLUMES; i++)
      {
      if (NULL != radar->v[i])
         {
         printf("%02i - %s\n", i, RSL_ftype[i]);
         }
      }
   }


/*
@##############################################################################
@
@   NOME         : aloca_matriz_16
@
@   FUNCAO       : Aloca espaco na memoria para uma matriz de saida de 2 bytes
@                  por campo
@
@   PARAMETROS   : (E) tamanho da matriz (nx, ny)
@                  (S) 
@
@   RETORNO      : ponteiro
@
@   OBSERVACOES  : 
@
@##############################################################################
*/

short int *aloca_matriz_16(int nx, int ny)
   {
   short int *ptr = NULL;
   
#ifdef SAFE_MEM_MATRIX
   if (nx*ny > MAX_MATRIX_SIZE)
      {
      return NULL;
      }
#endif
   
   ptr = (short int *) calloc(nx*ny, sizeof(short int));
   
   if (NULL == ptr)
      {
      return NULL;
      }
   else
      {
      memset(ptr, 0, nx*ny*sizeof(short int));
      return ptr;
      }
   }

/*
@##############################################################################
@
@   NOME         : aloca_matriz_8
@
@   FUNCAO       : Aloca espaco na memoria para uma matriz de saida de 1 bytes
@                  por campo
@
@   PARAMETROS   : (E) tamanho da matriz (nx, ny)
@                  (S) 
@
@   RETORNO      : ponteiro
@
@   OBSERVACOES  : 
@
@##############################################################################
*/

char *aloca_matriz_8(int nx, int ny)
   {
   char *ptr = NULL;
   
#ifdef SAFE_MEM_MATRIX
   if (nx*ny > MAX_MATRIX_SIZE)
      {
      return NULL;
      }
#endif
   
   ptr = (char *) calloc(nx*ny, sizeof(char));
   
   if (NULL == ptr)
      {
      return NULL;
      }
   else
      {
      memset(ptr, 0, nx*ny*sizeof(char));
      return ptr;
      }
   }



/*
@##############################################################################
@
@   NOME         : copia_cabecalho
@
@   FUNCAO       : Copia para o cabecalho do arquivo de saida
@                  as informacoes dos parametros de entrada
@
@   PARAMETROS   : (E) ponteiro para a struct de parametros de entrada,
@                      var, level, arquivo e codigo do produto
@                  (S) nenhum
@
@   RETORNO      : nenhum
@
@   OBSERVACOES  : 
@
@##############################################################################
*/

void preenche_cabecalho(struct header_saida *header,
                        struct params_list *params, Radar *radar,
                        int arq, int var, int lev)
   {

   memset(header, 0, sizeof(struct header_saida));
   memcpy(&header->info_radar, (void *) &radar->h, sizeof(Radar_header));
   memcpy(&header->nome_original,
          &params->file_list[arq], MAX_FILENAME);
   header->nx = params->nx;
   header->ny = params->ny;
   header->nz = params->nz;
   header->dx = params->dx;
   header->dy = params->dy;
   header->dz = params->dz;
   header->radar_x = params->nx/2;
   header->radar_y = params->ny/2;
   header->radar_z = 0;
   /*no caso de VIL preenche os niveis de integracao das variaveis*/
   if (params->produto == PROD_VIL)
      {
      header->level = params->levels[0];
      header->level2 = params->levels[1];
      }
   else
      {
      header->level = params->levels[lev];
      }
   
   header->var = params->vars[var];
   header->produto = params->produto;
   header->produto_orig = PROD_RAW;
   header->base = params->base;
   }


/*
@##############################################################################
@
@   NOME         : CUSTOM_F e CUSTOM_INVF
@
@   FUNCAO       : Funcoes de transformacao de float para unsigned int (Range)
@
@   PARAMETROS   : (E) 
@                  (S) 
@
@   RETORNO      : 
@
@   OBSERVACOES  : 
@
@##############################################################################
*/
float CUSTOM_F(Range x)
   {
   return ((float) x) / 1000.0;
   }

Range CUSTOM_INVF(float x)
   {
   return (Range)(x * 1000);
   }

/*
@##############################################################################
@
@   NOME         : calcula_vil
@
@   FUNCAO       : calcula o conteudo de agua liquida integrada entre 2 camadas
@
@   PARAMETROS   : (E) refletividades em 2 niveis
@                  (S) vil
@
@   RETORNO      : 
@
@   OBSERVACOES  : 
@
@##############################################################################
*/
float calcula_vil(float z1, float z2, float dz)
   {
   float a = 0.00000344;
   float b = 0.57142857;
   
   if (z1 > 65) z1 = 65;
   if (z2 > 65) z2 = 65;
   
   z1 = pow(10,(z1/10));
   z2 = pow(10,(z2/10));
   
   return (dz * a * pow(((z1+z2)/2), b));
   
   }
/*
@##############################################################################
@
@   NOME         : calcula_vii
@
@   FUNCAO       : calcula o conteudo de gelo integrada entre 2 camadas
@
@   PARAMETROS   : (E) refletividades em 2 niveis
@                  (S) vil
@
@   RETORNO      : 
@
@   OBSERVACOES  : 
@
@##############################################################################
*/
float calcula_vii(float z1, float z2, float dz)
   {
   float a = pow(1000.0*917.0*4000000.0*M_PI, 3.0/7.0);
   float b = 0.57142857;
   
   if (z1 > 65) z1 = 65;
   if (z2 > 65) z2 = 65;
   
   z1 = pow(10,(z1/10));
   z2 = pow(10,(z2/10));
   
   return (dz * a * pow(((z1+z2)/2), b));
   
   }
/*
@##############################################################################
@
@   NOME         : acha_altura_cubo
@
@   FUNCAO       : encontra a maior altura de um bin no volume
@
@   PARAMETROS   : (E) volume
@                  (S) altura maxima (em metros)
@
@   RETORNO      : 
@
@   OBSERVACOES  : 
@
@##############################################################################
*/

float acha_altura_cubo(Volume *v)
   {
   int nsweep;
   float elev;
   float slant;
   float hmax = 0, h = 0, gr = 0;
   
   for(nsweep = 0; nsweep < v->h.nsweeps; nsweep++)
      {
      elev = v->sweep[nsweep]->ray[v->sweep[nsweep]->h.nrays-1]->h.elev;
      slant = v->sweep[nsweep]->ray[v->sweep[nsweep]->h.nrays-1]->h.range_bin1 +
         (v->sweep[nsweep]->ray[v->sweep[nsweep]->h.nrays-1]->h.gate_size *
          v->sweep[nsweep]->ray[v->sweep[nsweep]->h.nrays-1]->h.nbins);
      RSL_get_groundr_and_h(slant / 1000, elev, &gr, &h);
      if (h > hmax)
         hmax = h;
      }
   
   return 1000*hmax;
   }

/*
@##############################################################################
@
@   NOME         : soma_coluna
@
@   FUNCAO       : verfica se ha algum valor diferente de BADVAL na coluna
@
@   PARAMETROS   : (E) cubo, indice, tam_z
@                  (S) 
@
@   RETORNO      : OK, ou ERR
@
@   OBSERVACOES  : 
@
@##############################################################################
*/
int verifica_coluna(Cube *cubo, int i, int j, int nz)
   {
   int k;
   short int total_badval = 0;


   /*Valor de BADVAL em short int e' zero*/
   for (k = 0; k < nz; k++)
      {
      total_badval += cubo->carpi[k]->data[i][j];
      }

   if (total_badval == 0)
      return RSL_ERR;
   else
      return RSL_OK;
   
   }

/*
@##############################################################################
@
@   NOME         : monta_data
@
@   FUNCAO       : Monta uma string YYYYMMDD_hhmm com base no Radar header
@
@   PARAMETROS   : (E) *radar, tamanho da string
@                  (S) string de data
@
@   RETORNO      : void
@
@   OBSERVACOES  : 
@
@##############################################################################
*/
void monta_data(Radar_header *radar, char *data, int tamanho)
   {
   memset(data, 0, tamanho);
   sprintf(data, "%04d%02d%02d_%02d%02d", radar->year, radar->month,
           radar->day, radar->hour, radar->minute);
   }


/*
@##############################################################################
@
@   NOME         : corrige_param_radar
@
@   FUNCAO       : corrige parametros que nao foram lidos corretamente
@
@   PARAMETROS   : (E) *radar, cabecalho de entrada
@                  (S) *radar
@
@   RETORNO      : void
@
@   OBSERVACOES  : 
@
@##############################################################################
*/
void corrige_param_radar(Volume *v, struct params_list *params)
   {
   int j = 0, t_sweep = 0;
   
   if (params->beam_width <= 0)
      {
      return;
      }
   
   for (j = 0; j < v->h.nsweeps; j++)
      {
      if ((NULL != v->sweep[j]) && (++t_sweep <= v->h.nsweeps))
         {
         v->sweep[j]->h.beam_width = params->beam_width;
         v->sweep[j]->h.vert_half_bw = params->beam_width / 2;
         v->sweep[j]->h.horz_half_bw = params->beam_width / 2;
         }
      }
   }



/*
@##############################################################################
@
@   NOME         : calcula_chuva_dual_pol
@
@   FUNCAO       : calcula chuva para radares dual pol
@
@   PARAMETROS   : (E) dbz, zdr, kdp, a e b da relacao z-r
@                  (S) *radar
@
@   RETORNO      : chuva em mm/h (float)
@
@   OBSERVACOES  : 
@
@##############################################################################
*/

float calcula_chuva_dual_pol(float dbz, float zdr,
                             float kdp, float a, float b, char banda)
   {
   float rkdp = 0;
   float rain = 0;

   switch(banda)
      {
      /*banda-S*/
      case 0:
         if (kdp > 0)
            {
            rkdp = 44 * pow(kdp, 0.822);
            }
         else
            {
            rkdp = 0;
            }
         rain = (pow((pow(10, dbz/10) / a), (1/b)));
         if (rain <= 6)
            {
            return rain / (0.4 + 5*(pow(fabsf(zdr -1), 1.3)));
            }
         if ((rain > 6) && (rain <=50))
            {
            return rkdp / (0.4 + 3.5*(pow(fabsf(zdr -1), 1.7)));
            }
         if (rain > 50)
            {
            return rkdp;
            }
         /*caso nao encontrou nada...*/
         return -99;
         break;
         /*banda-C*/
      case 1:
         if ((dbz > 38) && (kdp > 0.15))
            {
            return 33.5 * pow(kdp, 0.83);
            }
         else
            {
            return (pow((pow(10, dbz/10) / a), (1/b)));
            }
         break;
         /*banda-X*/
      case 2:
         if ((dbz > 35) && (kdp > 0.3))
            {
            return 19.63 * pow(kdp, 0.823);
            }
         else
            {
            return (pow((pow(10, dbz/10) / a), (1/b)));
            }
         break;
      }
   /*caso a banda tenha sido passado errada, retorna 0*/
   return 0;
   }

/*
@##############################################################################
@
@   NOME         : calcula_chuva_single_pol
@
@   FUNCAO       : calcula chuva para radares single pol 
@
@   PARAMETROS   : (E) dbz, a e b da relacao z-r
@                  (S) *radar
@
@   RETORNO      : chuva em mm/h (float)
@
@   OBSERVACOES  : 
@
@##############################################################################
*/

float calcula_chuva_single_pol(float dbz, float a, float b)
   {
   return (pow((pow(10, dbz/10) / a), (1/b)));
   }

/*
@##############################################################################
@
@   NOME         : filtra_raw_data
@
@   FUNCAO       : filtra volume baseado em valores suspeitos de refletividade
@
@   PARAMETROS   : (E) *radar
@                  (S) 
@
@   RETORNO      : 
@
@   OBSERVACOES  : 
@
@##############################################################################
*/

void filtra_raw_data(Radar *radar)
   {
   Volume *vol_controle = NULL;
   int nweep = 0, nrays=0, nbins = 0;
   int i = 0, j = 0, k = 0;
   int var = -1;
   float val = 0, val1 = 0, val2 = 0, totaldBZ;
   
   /*verifica valores muito altos de refletividade*/

   /*testa primeiro a refletividade corrigida*/
   if (NULL != radar->v[CZ_INDEX])
      {
      var = CZ_INDEX;
      }
   else
      {
      /*se nao encontrou ref corrigida (arq SIGMET por exemplo), usa DZ*/
      if (NULL == radar->v[DZ_INDEX])
         {
         /*se nao encontrou nenhuma, sai*/
         return;
         }
      else
         {
         var = DZ_INDEX;
         }
      }

   /*cria um volume de controle com 1 e 0 - dados bons e dados ruins*/
   vol_controle = RSL_copy_volume(radar->v[var]);
   for(i = 0; i < vol_controle->h.nsweeps; i++)
      {
      for(j = 0; j < vol_controle->sweep[i]->h.nrays; j++)
         {
         for(k = 0; k < vol_controle->sweep[i]->ray[j]->h.nbins; k++)
            {
            vol_controle->sweep[i]->ray[j]->range[k] = 1;
            }
         }
      }
   
   /*verifica os bins com possiveis erros

   1 - bins isolados com dBZ > 35
   2 - bins acima de 70 dBZ
   3 - dBZ acumulado na radial maior que 45dBZ x numbins
   */


   for(i = 0; i < radar->v[var]->h.nsweeps; i++)
      {
      totaldBZ = 0;
      /*faz um tratamento para o ray 0*/
      for(k = 0; k < radar->v[var]->sweep[i]->ray[0]->h.nbins; k++)
         {
         val = radar->v[var]->h.f(radar->v[var]->sweep[i]->ray[0]->range[k]);
         if (val == BADVAL || val == APFLAG ||
             val == RFVAL || val == NOECHO) 
            {
            continue;
            }

         totaldBZ += val;
         
         if (val > 70)
            {
            vol_controle->sweep[i]->ray[0]->range[k] = 0;
            }
         
         if (val >= 35)
            {
            val1 = radar->v[var]->h.f(radar->v[var]->sweep[i]->ray[radar->v[var]->sweep[i]->h.nrays-1]->range[k]);
            val2 = radar->v[var]->h.f(radar->v[var]->sweep[i]->ray[1]->range[k]);
            if (val1 == BADVAL || val1 == APFLAG ||
                val1 == RFVAL || val1 == NOECHO ||
                val2 == BADVAL || val2 == APFLAG ||
                val2 == RFVAL || val2 == NOECHO ||
                ((val1 < 15) && (val2 < 15)))
               {
               /*bin isolado*/
               vol_controle->sweep[i]->ray[0]->range[k] = 0;
               }
            }
         }

      if (totaldBZ > (45.0 * ((float) radar->v[var]->sweep[i]->ray[0]->h.nbins)))
         {
         for(k = 0; k < radar->v[var]->sweep[i]->ray[0]->h.nbins; k++)
            {
            vol_controle->sweep[i]->ray[0]->range[k] = 0;
            }
         }

      

      /*verifica pontos isolados na radial*/
      for(k = 1; k < radar->v[var]->sweep[i]->ray[0]->h.nbins-1; k++)
         {
         val = radar->v[var]->h.f(radar->v[var]->sweep[i]->ray[0]->range[k]);
         if (val == BADVAL || val == APFLAG ||
             val == RFVAL || val == NOECHO) 
            {
            continue;
            }         
         if (val > 70)
            {
            vol_controle->sweep[i]->ray[0]->range[k] = 0;
            }
         if (val >= 35)
            {
            val1 = radar->v[var]->h.f(radar->v[var]->sweep[i]->ray[0]->range[k-1]);
            val2 = radar->v[var]->h.f(radar->v[var]->sweep[i]->ray[0]->range[k+1]);
            if (val1 == BADVAL || val1 == APFLAG ||
                val1 == RFVAL || val1 == NOECHO ||
                val2 == BADVAL || val2 == APFLAG ||
                val2 == RFVAL || val2 == NOECHO ||
                ((val1 < 15) && (val2 < 15)))
               {
               /*bin isolado*/
               vol_controle->sweep[i]->ray[0]->range[k] = 0;
               }               
            }            
         }

      

      /*varre os outros rays*/
      for(j = 1; j < radar->v[var]->sweep[i]->h.nrays - 1; j++)
         {
         totaldBZ = 0;
         for(k = 0; k < radar->v[var]->sweep[i]->ray[j]->h.nbins; k++)
            {
            val = radar->v[var]->h.f(radar->v[var]->sweep[i]->ray[j]->range[k]);
            if (val == BADVAL || val == APFLAG ||
                val == RFVAL || val == NOECHO) 
               {
               continue;
               }

            totaldBZ += val;
            if (val >= 35)
               {
               val1 = radar->v[var]->h.f(radar->v[var]->sweep[i]->ray[j-1]->range[k]);
               val2 = radar->v[var]->h.f(radar->v[var]->sweep[i]->ray[j+1]->range[k]);
               if (val1 == BADVAL || val1 == APFLAG ||
                   val1 == RFVAL || val1 == NOECHO ||
                   val2 == BADVAL || val2 == APFLAG ||
                   val2 == RFVAL || val2 == NOECHO ||
                ((val1 < 15) && (val2 < 15)))
                  {
                  /*bin isolado*/
                  vol_controle->sweep[i]->ray[j]->range[k] = 0;
                  }               
               }
            }

         if (totaldBZ > (45.0 * ((float) radar->v[var]->sweep[i]->ray[j]->h.nbins)))
            {
             for(k = 0; k < radar->v[var]->sweep[i]->ray[j]->h.nbins; k++)
                {
                vol_controle->sweep[i]->ray[j]->range[k] = 0;
                }
            }
         
         
         /*verifica pontos isolados na radial*/
         for(k = 1; k < radar->v[var]->sweep[i]->ray[j]->h.nbins-1; k++)
            {
            val = radar->v[var]->h.f(radar->v[var]->sweep[i]->ray[j]->range[k]);
            if (val == BADVAL || val == APFLAG ||
                val == RFVAL || val == NOECHO) 
               {
               continue;
               }
            if (val >= 35)
               {
               val1 = radar->v[var]->h.f(radar->v[var]->sweep[i]->ray[j]->range[k-1]);
               val2 = radar->v[var]->h.f(radar->v[var]->sweep[i]->ray[j]->range[k+1]);
               if (val1 == BADVAL || val1 == APFLAG ||
                   val1 == RFVAL || val1 == NOECHO ||
                   val2 == BADVAL || val2 == APFLAG ||
                   val2 == RFVAL || val2 == NOECHO ||
                   ((val1 < 15) && (val2 < 15)))
                  {
                  /*bin isolado*/
                  vol_controle->sweep[i]->ray[j]->range[k] = 0;
                  }               
               }            
            }
         }
      }


   /*temos entao um volume com 0 onde ha dados suspeitos e 1 em dados ok
   preencheremos os dados suspeitos como BAD_VAL

   */
   for(i = 0; i < vol_controle->h.nsweeps; i++)
      {
      for(j = 0; j < vol_controle->sweep[i]->h.nrays; j++)
         {
         for(k = 0; k < vol_controle->sweep[i]->ray[j]->h.nbins; k++)
            {
            if (0 == vol_controle->sweep[i]->ray[j]->range[k])
               {
               radar->v[var]->sweep[i]->ray[j]->range[k] =
                  radar->v[var]->h.invf(BADVAL);
               }
            }
         }
      }
   }

