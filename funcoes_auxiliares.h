#define DATAHORA
#define USE_RSL_VARS
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <rsl.h>
#include <zlib.h>
#include <assert.h>

#define MAX_FILES 256
#define MAX_FILENAME 256
#define MAX_LEVELS 256
#define MAX_MATRIX_SIZE 5000*5000
#define SAFE_MEM_MATRIX
#define RSL_TRUE 1
#define RSL_FALSE 0
#define RSL_OK 0
#define RSL_ERR -1

/*
 Rastreio: Nao alterar esse valor sem verificar
 os indices dos vetores
*/
#define UNDEF_CLUSTER 0
#define MAX_CLUSTER 10000

/*Codigos dos produtos*/
#define PROD_RAW     0
#define PROD_PPI     1
#define PROD_RHI     2
#define PROD_CAPPI   3
#define PROD_MAXDIS  4
#define PROD_BASE    5
#define PROD_TOP     6
#define PROD_HMAX    7
#define PROD_VIL     8
#define PROD_VENTO   9
#define PROD_ALERTA  10 
#define PROD_CHUVA1  11 
#define PROD_CHUVA2  12
#define PROD_ALLPPI  13
#define PROD_BACIA   14 
#define PROD_CLUSTER 15 
#define PROD_TRACK   16 
#define PROD_RTI     17

#ifndef M_PI
 #define M_PI 3.14159265358979323846
#endif

#ifdef MAX_RADAR_VOLUMES
 #define MAX_VARS MAX_RADAR_VOLUMES
#else
 #define MAX_VARS 42
#endif
/*
static char *texto_var[] = {"DZ", "VR", "SW", "CZ", "ZT", "DR", 
                            "LR", "ZD", "DM", "RH", "PH", "XZ", 
                            "CD", "MZ", "MD", "ZE", "VE", "KD", 
                            "TI", "DX", "CH", "AH", "CV", "AV",
                            "SQ", "VS", "VL", "VG", "VT", "NP",
                            "HC", "VC", "V2", "S2", "V3", "S3",
                            "CR", "CC", "PR", "SD", "ZZ", "RD"};
*/


typedef struct estat
   {
   unsigned short int total_pix;
   unsigned short int padding;
   unsigned short int x_centro;
   unsigned short int y_centro;
   float valor_medio;
   float desvio;
   } Estat;

struct params_list
   {
   /*Qtd de pontos na matriz*/
   unsigned short int nx;
   unsigned short int ny;
   unsigned short int nz;
   /*Espacamento da matriz em x (METROS)*/
   unsigned short int dx;
   unsigned short int dy;
   unsigned short int dz;
   /*posicao do radar na grade de saida*/
   unsigned short int radar_x;
   unsigned short int radar_y;
   unsigned short int radar_z;
   /*Lista de nomes dos arquivos - sempre preencher com zeros*/
   char file_list[MAX_FILES][MAX_FILENAME];
   /*Sufixo do arquivo de saida*/
   char sufixo[MAX_FILENAME/2];
   /*Quantidade de niveis para os produtos
   (azimute para RHI, elev para CAPPI, etc*/
   unsigned short int nlevels;
   /*Niveis para os produtos (azim para RHI, elev para CAPPI, etc*/   
   unsigned short int levels[MAX_LEVELS];
   /*Base da grade (nivel base do cubo) em metros*/
   unsigned short int base;
   /*Quantidade de variaveis (volumes) para os produtos*/
   unsigned short int nvars;
   /*Variaveis (volumes) para os produtos*/
   unsigned short int vars[MAX_VARS];
   /*Produto gerado*/
   unsigned char produto;
   /*Flag de compressao. RSL_FALSE para nao comprimir, RSL_TRUE para comprimir*/
   char comprimir;
   /*parametros auxiliares. Usados para relacao Z/R,
     threshold de cluster, etc*/
   float a;
   float b;
   float c;
   float d;
   /*largura do feixe*/
   float beam_width;
   };

struct header_saida
   {
   /*Informacoes sobre o radar*/
   Radar_header info_radar;
   /*para alinhar a struct em multiplo de 8
     ponteiros tem 8 bytes em maquinas de 64 bits*/
   char padding1;
   /*Produto gerado*/
   unsigned char produto;
   /*Produto original - No caso de produtos derivados de outros produtos*/
   unsigned char produto_orig;
   /*Variavel gerada no produto (DZ, etc)*/
   unsigned char var;
   /*alinhamento*/
   /*Nome do arquivo original*/
   char nome_original[MAX_FILENAME];
   /*Qtd de pontos na matriz*/
   unsigned short int nx;
   unsigned short int ny;
   unsigned short int nz;
   /*Espacamento da matriz em x (METROS)*/
   unsigned short int dx;
   unsigned short int dy;
   unsigned short int dz;
   /*posicao do radar na grade de saida*/
   unsigned short int radar_x;
   unsigned short int radar_y;
   unsigned short int radar_z;
   /*Base da grade (nivel base do cubo) em metros*/
   unsigned short int base;
   /*Nivel do cappi em metros ou
     numero da elevacao do PPI (1-15 geralmente) ou
     angulo azimutal do RHI em graus (0-359)*/
   unsigned short int level;
   /*level2 so usado em VIL/ALERTA - eh o nivel da camada
     mais alta usada na integracao do conteudo de agua liquida*/
   unsigned short int level2;
   /*
   0 (zero) se o arquivo nao foi comprimido. Se o valor for maior que 0,
   usar esse valor como parametro size_in na funcao de descompressao.
   Sempre gravar size_out aqui apos usar a funcao de compressao
   */
   unsigned int tamanho_compressao;
   /*parametros auxiliares. Usados para relacao Z/R,
     threshold de cluster, etc*/
   float a;
   float b;
   float c;
   float d;
   };

int faz_cappi(struct params_list *lista_parametros, Radar *radar);
int faz_echobase(struct params_list *lista_parametros);
int faz_echotop(struct params_list *lista_parametros, Radar *radar);
int faz_hmax(struct params_list *lista_parametros);
int faz_maxdisplay(struct params_list *lista_parametros);
int faz_ppi(struct params_list *lista_parametros, Radar *radar);
int faz_rhi(struct params_list *lista_parametros);
int faz_vil(struct params_list *lista_parametros, Radar *radar);
int faz_rastreio(struct params_list *lista_parametros);
int faz_cluster(struct params_list *lista_parametros);
int faz_alerta(struct params_list *lista_parametros);
int faz_chuva(struct params_list *lista_parametros);
int faz_acumulado_chuva(struct params_list *lista_parametros);
int faz_ppi_todos(struct params_list *lista_parametros);
int faz_rti(struct params_list *lista_parametros);


char le_parametros_entrada(char *arquivo, int *parametros,
                           int *alturas, int *var, char *lista);
void imprime_lista_var(void);
void imprime_lista_variaveis(Radar *radar);
short int *aloca_matriz_16(int nx, int ny);
char *aloca_matriz_8(int nx, int ny);
void preenche_cabecalho(struct header_saida *header,
                        struct params_list *params, Radar *radar,
                        int arq, int var, int lev);
float CUSTOM_F(Range x);
Range CUSTOM_INVF(float x);
float calcula_vil(float z1, float z2, float dz);
Cube *faz_cubo(Volume *volume, struct params_list *lista_parametros,
               float alt_radar);
float acha_altura_cubo(Volume *v);

int uncompress_buffer(char *source, char *dest,
                      unsigned int size_in, unsigned int *size_out);

int compress_buffer(char *source, char *dest,
                    unsigned int size_in, unsigned int *size_out);

short int *fatia_cubo(Cube *cubo, int x1, int y1, int x2, int y2,
                      int *tam_x, int *tam_z);


int verifica_coluna(Cube *cubo, int i, int j, int nz);

void numera_cluster(short int *in, short int *out,
                    int i, int j, int nx, int ny,
                    int num_cluster, int limite, int var);

int limpa_resultado (short int *dados, short int *in, int tamanho,
                     int nx, int ny, short int *max_cluster,
                     Estat *stats, int var);

void monta_data(Radar_header *radar, char *data, int tamanho);


char *interpola_mascara(short int *entrada,
                        int nx_in, int ny_in,
                        float dx_in, float dy_in,
                        float superior_esquerdo_in_x,
                        float superior_esquerdo_in_y,
                        struct params_list *lista_parametros,
                        float lon_radar, float lat_radar);

void corrige_param_radar(Volume *v, struct params_list *params);
int filtra_volume(Volume *volume, struct params_list *lista_parametros);

/*
Funcoes que necessitam de C++ - Nao utilizadas nesse projeto
int RSL_radar_to_netcdf(Radar *radar, char *outfile);
Radar *RSL_gamic_to_radar(char *filename);
*/
