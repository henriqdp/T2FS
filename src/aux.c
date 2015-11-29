#include "apidisk.h"
#include "t2fs.h"
#include "aux.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


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

WORD read_word(sector_t* sector){
  WORD result = 0, buffer = 0;
  result = sector->buffer[sector->current_index++];
  buffer = sector->buffer[sector->current_index++];
  result += buffer << 8;
  if(sector->current_index >= SECTOR_SIZE)
    free(sector);
  return result;
}

BYTE read_byte(sector_t* sector){
  char result = sector->buffer[sector->current_index++];
  if(sector->current_index >= SECTOR_SIZE)
    free(sector);
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
  if(sector->current_index >= SECTOR_SIZE)
    free(sector);
  return result;
}


int read_superblock(){
  if(DEBUG_ON){
    printf("Tentando ler superblock.\n");
  }
  sector_t *setor_superbloco = initialize_sector(0);
  _superbloco = (superbloco_t*) malloc(sizeof(superbloco_t));

  if(_superbloco == NULL){
  	exit(EXIT_FAILURE);
  }

  //LEITURA DA IDENTIFICACAO DO DISCO
  unsigned int i;
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
    
  //copia da FAT para memoria
  FAT = (fat_t *) malloc(sizeof(fat_t));
  FAT->size_in_sectors = _superbloco->sFATSectorStart - _superbloco->pFATSectorStart;
  if(DEBUG_ON)
    printf("Tamanho da FAT em setores: %d\n", FAT->size_in_sectors);
  FAT->data = (WORD *) calloc(SECTOR_SIZE/2, sizeof(WORD));
  for(i = 0; i < FAT->size_in_sectors; i++){
     read_sector(_superbloco->pFATSectorStart + i, (char *)FAT->data + i*(SECTOR_SIZE/2));   
   }

   //especificacao do diretorio raiz na memoria
  current_directory = (dir_t *) malloc(sizeof(dir_t));
  current_directory->fullpath = (char *) calloc(1024, sizeof(char));
  memset(current_directory->fullpath, '\0', 1024);
  strcpy(current_directory->fullpath, "/");	
  if(DEBUG_ON){
    printf("Diretorio corrente: ");
    printf("%s\n", current_directory->fullpath);
  }
  current_directory->current_cluster = _superbloco->RootSectorStart;
  current_directory->first_cluster   = _superbloco->RootSectorStart;
  current_directory->current_entry   = 0;
  current_directory->cluster_index   = 0;

  working_directory = (dir_t *) malloc(sizeof(dir_t));
  working_directory->fullpath = (char *) calloc(1024, sizeof(char));

  if(DEBUG_ON){
  	printf("Current cluster: %hu\n", current_directory->current_cluster);
  	printf("Current_entry:   %hu\n", current_directory->current_entry);
  	printf("Cluster index:   %hu\n", current_directory->cluster_index);
  	printf("Is final cluster? ");
  	if(current_directory->is_final_cluster)
  		printf("True.\n");
  	else
  		printf("False.\n");
  }

  free(setor_superbloco);
  return 0;
}

int read_next_entry(dir_t *directory, DIRENT2 *dentry, WORD *cluster){
	sector_t *sector;
	int current_entry_within_sector, real_sector;
	if(DEBUG_ON)
		printf("Primeiro cluster: %d\n", directory->first_cluster);
	if(directory->first_cluster == _superbloco->RootSectorStart){
		real_sector = directory->current_cluster + directory->current_entry / (SECTOR_SIZE/32);

		if(real_sector >= _superbloco->DataSectorStart){
			if(DEBUG_ON)
				printf("leitura do diretorio raiz chegou ao fim deste.\n");
			directory->current_entry = 0;
			return -1;
		}

		current_entry_within_sector = directory->current_entry % (SECTOR_SIZE/32);
		directory->current_entry++;
	}
	else{
		int cluster_base_sector = _superbloco->DataSectorStart + (directory->current_cluster - 2)*_superbloco->SectorPerCluster;
		int current_entry_within_cluster = directory->current_entry % ((SECTOR_SIZE/32)*_superbloco->SectorPerCluster);
		current_entry_within_sector = directory->current_entry % (SECTOR_SIZE/32);
		real_sector = cluster_base_sector + (current_entry_within_cluster / (SECTOR_SIZE/32));

		if(DEBUG_ON){
			printf("Procurando entrada em diretorio que nao eh o raiz!!\n");
			printf("Entrada corrente: %d\n", directory->current_entry);
			printf("Cluster corrente:%d\n", directory->current_cluster);
			printf("First cluster: %d\n", directory->first_cluster);
			printf("Final cluster? %d\n", directory->is_final_cluster);
		  	printf("Setor base: %d\n", cluster_base_sector);
		  	printf("Setor real: %d\n", real_sector);
		  	printf("Dentro do cluster: %d\n", directory->current_entry % 8);
		}
		directory->current_entry++;
		if(directory->current_entry % ((SECTOR_SIZE/32)*_superbloco->SectorPerCluster) == 0){
			if(DEBUG_ON)
				printf("TEM QUE TROCAR DE CLSTERRRR\n\n\n");
			if(directory->is_final_cluster == true){
				rewind_dir(directory);
				return -1;
			}
			else{
				directory->current_cluster = FAT->data[directory->current_cluster-2];
				if(FAT->data[directory->current_cluster-2] == FINAL_CLUSTER)
					directory->is_final_cluster = true;
				return read_next_entry(directory, dentry, cluster);
			}

		}		
	}

	sector = initialize_sector(real_sector);
	
	int i;
	for (i = 0; i < current_entry_within_sector; i++){
		read_dword(sector);
		read_dword(sector);
		read_dword(sector);
		read_dword(sector);
		
		read_dword(sector);
		read_dword(sector);
		read_dword(sector);
		read_dword(sector);
	}
	dentry->fileType = (int) read_byte(sector);
	if(dentry->fileType != 0){
		for(i = 0; i < MAX_FILE_NAME_SIZE; i++){
	 		dentry->name[i] = read_byte(sector);
	 	}
	 	dentry->fileSize = read_dword(sector);
	 	short int firstCluster = read_word(sector);
	 	if(DEBUG_ON){
	 		switch(dentry->fileType){
	 			case 1:
	 				printf("Arquivo encontrado: \n");
	 				break;
	 			case 2:
	 				printf("Subdiretorio encontrado:\n");
	 				break;
	 		}
	 		printf("\tNome: %s, tamanho: %lu\n", dentry->name, dentry->fileSize);
	 		printf("\tFirst cluster: %hu\n\n", firstCluster);
	 		
	 	}
	 	*cluster = firstCluster;
	 	free(sector);
	 	return 0;
	}else{
		free(sector);
		return read_next_entry(directory, dentry, cluster);
	}
	return 0;
}

void mirror_paths(int way){
	if(way == CURR_TO_WORK){
		char *aux = working_directory->fullpath;
		memcpy(working_directory, current_directory, sizeof(dir_t));
		working_directory->fullpath = aux;
		strcpy(working_directory->fullpath, current_directory->fullpath);
	}
	else{
		char *aux = current_directory->fullpath;
		memcpy(current_directory, working_directory, sizeof(dir_t));
		current_directory->fullpath = aux;
		strcpy(current_directory->fullpath, working_directory->fullpath);
	}
}

void rewind_dir(dir_t *dir){
	dir->current_cluster = dir->first_cluster;
	dir->current_entry   = 0;
}



void set_working_to_root(){
	working_directory->first_cluster    = _superbloco->RootSectorStart;
	working_directory->current_cluster = _superbloco->RootSectorStart;
	strcpy(working_directory->fullpath, "/");
	working_directory->current_entry = 0;
}

int change_dir(char *path, Bool subsequent){

	char *next_dir;
	if(subsequent)
		next_dir = strtok(NULL, "/");
	else
		next_dir = strtok(path, "/");
	if(next_dir == NULL){
		mirror_paths(WORK_TO_CURR);
		return 0;
	}

	if(DEBUG_ON){
			printf("Procurando por diretorio: ");	
			puts(next_dir);
	}

	if(working_directory->first_cluster == _superbloco->RootSectorStart)
		if(strcmp(next_dir, ".") == 0 || strcmp(next_dir, "..") == 0)
			return change_dir(path, true);

	DIRENT2 *entry = (DIRENT2 *) malloc(sizeof(DIRENT2));
	Bool found = false;
	WORD clusterNo;
	rewind_dir(working_directory);

	while(!found && read_next_entry(working_directory, entry, &clusterNo) != -1){
		if(strcmp(next_dir, entry->name) == 0)
			found = true;
	}

	if(found){
		if(DEBUG_ON)
			printf("Diretorio encontrado.\n");
		if(entry->fileSize != 0){
			if(DEBUG_ON)
				printf("Erro! Arquivo especificado nao e' um diretorio.\n");
			return -1;
		}
		if(clusterNo == 0){
			set_working_to_root();
			return change_dir(path, true);
		}
		else{
			working_directory->first_cluster = clusterNo;
			working_directory->current_cluster = clusterNo;
			working_directory->current_entry = 0;
			
			if(FAT->data[clusterNo-2] == FINAL_CLUSTER)
				working_directory->is_final_cluster = true;
			else
				working_directory->is_final_cluster = false;

			if(strcmp(next_dir, ".") != 0){
				if(strcmp(next_dir, "..") == 0){
					char *aux = working_directory->fullpath + strlen(working_directory->fullpath) - 1;
					if(*aux == '/')
						*(aux--) = '\0';
					while(*aux != '/'){
						*aux = '\0';
						aux -= 1;
					}
				}
				else{
					if (working_directory->fullpath[strlen(working_directory->fullpath) - 1] != '/')
						strcat(working_directory->fullpath, "/");
					strcat(working_directory->fullpath, next_dir);
				}
			}
			free(entry);
			return change_dir(path, true);
		}
	}
	else{
		free(entry);
		if(DEBUG_ON)
			printf("Diretorio especificado nao encontrado.\n");
		return -1;
	}
	return 0;
}

int get_first_invalid_entry(){
	return 0;
}

Bool split_path(char *path){
	char *aux = path + strlen(path) - 1;
	if(*aux == '/')
		*aux = '\0';
	while(*aux != '/' && aux != path)
		aux--;
	if(aux != path){
		*aux = '\0';
		return true;
	}
	else{
		return false;
	}
}

Bool is_valid(char *name){
	Bool valid = true;
	char *aux = name;
	while(valid && *aux != '\0'){
		if((*aux < '0' && *aux > '9') && (*aux < 'a' && *aux > 'z') && (*aux < 'A' && *aux > 'Z'))
			valid = false;
		else
			aux++;
	}
	return valid;

}

