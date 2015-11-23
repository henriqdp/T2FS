#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include <stdio.h>
#include <stdlib.h>

sector_t *initialize_sector(int sector_number){
  sector_t *data = (sector_t *) malloc(sizeof(sector_t));
  data->current_index = 0;
  int result = read_sector(sector_number, data->buffer);
  if(result == 0)
    return data;
  else{
    if(DEBUG_ON)
      printf("Erro na leitura do setor especificado! (Setor #%d)\n", sector_number);
    return NULL;
  }
}


BYTE read_byte(sector_t* sector){
  char result = sector->buffer[sector->current_index++];
  return result;
}

WORD read_word(sector_t* sector){
  WORD result = 0, buffer = 0;
  result = sector->buffer[sector->current_index++];
  buffer = sector->buffer[sector->current_index++];
  result += buffer << 8;
  return result;
}

DWORD read_dword(sector_t* sector){
  DWORD result = 0, buffer = 0;
  result = sector->buffer[sector->current_index++];
  buffer = sector->buffer[sector->current_index++];
  result += buffer << 8;
  buffer = sector->buffer[sector->current_index++];
  result += buffer << 16;
  buffer = sector->buffer[sector->current_index++];
  result += buffer << 24;
  return result;
}

void read_superblock(){
  if(DEBUG_ON){
    printf("Tentando ler superblock.\n");
  }
  sector_t *setor_superbloco = initialize_sector(0);
  _superbloco = (superbloco_t*) malloc(sizeof(superbloco_t));

  //LEITURA DA IDENTIFICACAO DO DISCO
  short int i;
  for(i = 0; i < 4; i++){
    _superbloco->Id[i] = read_byte(setor_superbloco);
  }

  //LEITURA DA VERSAO
  _superbloco->Version = read_word(setor_superbloco);

  //LEITURA DO TAMANHO DO SUPERBLOCO
  _superbloco->SuperBlockSize = read_word(setor_superbloco);

  //LEITURA DO TAMANHO DO DISCO
  _superbloco->DiskSize = read_dword(setor_superbloco);

  //LEITURA DO NUMERO DE SETORES
  _superbloco->NofSectors = read_dword(setor_superbloco);

  //LEITURA DO NUMERO DE SETORES POR CLUSTER
  _superbloco->SectorPerCluster = read_dword(setor_superbloco);

  //LEITURA DO PRIMEIRO SETOR DA FAT PRINCIPAL
  _superbloco->pFATSectorStart = read_dword(setor_superbloco);

  //LEITURA DO PRIMEIRO SETOR DA FAT SECUNDARIA
  _superbloco->sFATSectorStart = read_dword(setor_superbloco);

  //LEITURA DO PRIMEIRO SETOR LOGICO DO DIRETORIO RAIZ
  _superbloco->RootSectorStart = read_dword(setor_superbloco);

  //LEITURA DO PRIMEIRO SETOR LOGICO DA AREA DE BLOCOS DE DADOS
  _superbloco->DataSectorStart = read_dword(setor_superbloco);

  //LEITURA DA QUANTIDADE DE ENTRADAS NO DIRETORIO RAIZ
  _superbloco->NofDirEntries = read_dword(setor_superbloco);

  if(DEBUG_ON){
    printf("Descricao do superbloco:\n\n");
    printf("Identificacao: %c%c%c%c\n", _superbloco->Id[0], _superbloco->Id[1], _superbloco->Id[2], _superbloco->Id[3]);
    printf("Versao: %x\n", _superbloco->Version);
    printf("Tamanho do superbloco: %hd\n", _superbloco->SuperBlockSize);
    printf("Tamanho do disco: %d\n", _superbloco->DiskSize);
    printf("Numero de setores: %d\n", _superbloco->NofSectors);
    printf("Numero de setores por cluster: %d\n", _superbloco->SectorPerCluster);
    printf("Primeiro setor da FAT principal: %d\n", _superbloco->pFATSectorStart);
    printf("Primeiro setor da FAT secundaria: %d\n", _superbloco->sFATSectorStart);
    printf("Primeiro setor logico do diretorio raiz: %d\n", _superbloco->RootSectorStart);
    printf("Primeiro setor logico da area de blocos de dados: %d\n", _superbloco->DataSectorStart);
    printf("Quantidade de entradas no diretorio raiz: %d\n", _superbloco->NofDirEntries);
  }
  superblock_read = true;
}


int identify2 (char *name, int size){
  if(!superblock_read){
    read_superblock();
  }
  printf("Identificacao dos autores do trabalho:\n");
  printf("1) Henrique de Paula Lopes - matricula 205689\n");
  return 0;
}

FILE2 create2 (char *filename){
  if(!superblock_read){
    read_superblock();
  }
  FILE2 file = 0;
  return file;
}

int delete2 (char *filename){
  if(!superblock_read){
    read_superblock();
  }
  return 0;
}

FILE2 open2 (char *filename){
  if(!superblock_read){
    read_superblock();
  }
  FILE2 retorno = 0;
  return retorno;
}

int close2 (FILE2 handle){
  if(!superblock_read){
    read_superblock();
  }
  return 0;
}

int read2 (FILE2 handle, char *buffer, int size){
  if(!superblock_read){
    read_superblock();
  }
  return 0;
}

int write2 (FILE2 handle, char *buffer, int size){
  if(!superblock_read){
    read_superblock();
  }
  return 0;
}

int seek2 (FILE2 handle, unsigned int offset){
  if(!superblock_read){
    read_superblock();
  }
  return 0;
}

int mkdir2 (char *pathname){
  if(!superblock_read){
    read_superblock();
  }
  return 0;
}

int rmdir2 (char *pathname){
  if(!superblock_read){
    read_superblock();
  }
  return 0;
}

DIR2 opendir2 (char *pathname){
  if(!superblock_read){
    read_superblock();
  }
  DIR2 dir = 0;

  return dir;
}

int readdir2 (DIR2 handle, DIRENT2 *dentry){
  if(!superblock_read){
    read_superblock();
  }
  return 0;
}

int closedir2 (DIR2 handle){
  if(!superblock_read){
    read_superblock();
  }
  return 0;
}

int chdir2 (char *pathname){
  if(!superblock_read){
    read_superblock();
  }
  return 0;
}

int getcwd2 (char *pathname, int size){
  if(!superblock_read){
    read_superblock();
  }
  return 0;
}
