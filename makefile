#
# Makefile ESQUELETO
#
# DEVE ter uma regra "all" para geração da biblioteca
# regra "clean" para remover todos os objetos gerados.
#
# NECESSARIO adaptar este esqueleto de makefile para suas necessidades.
#
#

CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src
TST_DIR=./teste


all:
	$(CC) -c $(SRC_DIR)* -Wall
	ar crs libt2fs.a *.o $(LIB_DIR)/apidisk.o
	mv libt2fs.a $(LIB_DIR)
	gcc -o sisop $(TST_DIR)/teste.c -L$(LIB_DIR) -lt2fs -Wall
	mv sisop *.o $(BIN_DIR)

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.* $(SRC_DIR)/*~ $(INC_DIR)/*~ *~ *.o
