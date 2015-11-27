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
	$(CC) -c -I$(INC_DIR) $(SRC_DIR)/* -Wall
	mv *.o $(BIN_DIR)
	ar crs $(LIB_DIR)/libt2fs.a $(BIN_DIR)/*.o $(LIB_DIR)/apidisk.o 
	gcc -o sisop -I$(INC_DIR) $(TST_DIR)/*.c -L$(LIB_DIR) -lt2fs -Wall
	mv sisop $(BIN_DIR)

clean:
	rm -rf $(LIB_DIR)/*.a $(LIB_DIR)/t2fs.o $(LIB_DIR)/aux.o $(BIN_DIR)/* $(SRC_DIR)/*~ $(INC_DIR)/*~ *~ *.o
