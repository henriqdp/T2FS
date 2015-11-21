#include "../include/t2fs.h"

#include <stdio.h>
#include <stdlib.h>

void read_superblock(){
  if(DEBUG_ON){
    printf("Attepmting to read superblock.\n\n");
  }
  char read_buffer[256];
  read_sector(0, read_buffer);
}


int identify2 (char *name, int size){
  if(!superblock_read){
    read_superblock();
  }
  printf("Identificacao dos autores do trabalho:\n");
  printf("1) Henrique de Paula Lopes - matricula 295689\n");
  return 0;
}

FILE2 create2 (char *filename){
  FILE2 file = 0;
  return file;
}

int delete2 (char *filename){
  return 0;
}

FILE2 open2 (char *filename){
  FILE2 retorno = 0;
  return retorno;
}

int close2 (FILE2 handle){
  return 0;
}

int read2 (FILE2 handle, char *buffer, int size){
  return 0;
}

int write2 (FILE2 handle, char *buffer, int size){
  return 0;
}

int seek2 (FILE2 handle, unsigned int offset){
  return 0;
}

int mkdir2 (char *pathname){
  return 0;
}

int rmdir2 (char *pathname){
  return 0;
}

DIR2 opendir2 (char *pathname){
  DIR2 dir = 0;
  return dir;
}

int readdir2 (DIR2 handle, DIRENT2 *dentry){
  return 0;
}

int closedir2 (DIR2 handle){
  return 0;
}

int chdir2 (char *pathname){
  return 0;
}

int getcwd2 (char *pathname, int size){
  return 0;
}
