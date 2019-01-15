CC=gcc
LIBS=-lm -lrsl -lz
CFLAGS=
OBJ=funcoes_auxiliares.o faz_cubo.o faz_cappi.o faz_echotop.o faz_ppi.o faz_vil.o main_geral.o filtra_volume.o

#gcc faz_echobase.c faz_maxdisplay.c faz_cappi.c faz_echotop.c faz_ppi.c faz_hmax.c faz_rhi.c main_geral.c -o cappi.exe -I/home/tbiscaro/rsl-v1.40/include  -L/home/tbiscaro/rsl-v1.40/lib -lm  -lrsl -g -O0 -Wall
############################# Makefile ##########################
all: cappi
cappi: $(OBJ)
	$(CC) -o cappi.exe $(OBJ) $(LIBS) $(CFLAGS) -I$(IDIR) -L$(LDIR) -g
%.o: %.c
	$(CC) -o $@ -c $< $(LIBS) $(CFLAGS) -I$(IDIR) -L$(LDIR) -g
clean:
	rm -rf *.o cappi.exe
