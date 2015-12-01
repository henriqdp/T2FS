#include "apidisk.h"
#include "t2fs.h"
#include "aux.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int write_byte(sector_t *sector, BYTE byte){
	if(sector->current_index >= SECTOR_SIZE)
		return -1;
	sector->buffer[sector->current_index++] = byte;
	return 0;
}

int write_word(sector_t *sector, WORD word){
	if(sector->current_index >= SECTOR_SIZE)
		return -1;
	char aux = word % 256;
	sector->buffer[sector->current_index++] = aux;
	aux = word >> 8;
	sector->buffer[sector->current_index++] = aux;
	return 0;
}

int write_dword(sector_t *sector, DWORD dword){
	if(sector->current_index >= SECTOR_SIZE)
		return -1;
	char aux = dword % 256;
	sector->buffer[sector->current_index++] = aux;
	aux = (dword >> 8) % 256;
	sector->buffer[sector->current_index++] = aux;
	aux = (dword >> 16) % 256;
	sector->buffer[sector->current_index++] = aux;
	aux = dword >> 24;
	sector->buffer[sector->current_index++] = aux;
	return 0;
}

int update_sector(sector_t *sector){
	if (sector->sector_index == 0)
		return -1;
	else{
		return write_sector(sector->sector_index, sector->buffer);
	}
}

int seek_sector(sector_t *sector, int position){
	if(sector == NULL || position >= (SECTOR_SIZE/32))
		return -1;
	else{
		sector->current_index = position * 32;
		return 0;
	}

}

int update_index(sector_t *sector, int new_sector_index){
	sector->sector_index = new_sector_index;
	return 0;
}


sector_t *initialize_sector(int sector_number){
  if(sector_number == -1){
  	sector_t *data = (sector_t *) malloc(sizeof(sector_t));
  	data->current_index = 0;
  	data->sector_index = 0;
  	if(data)
  		return data;
  	else{
  		if(DEBUG_ON)
  			printf("Erro! Impossivel alocar memoria para o novo setor.\n");
  		return NULL;
  	}
  }
  else{
	  sector_t *data = (sector_t *) malloc(sizeof(sector_t));
	  data->current_index = 0;
	  data->sector_index = sector_number;
	  int result = read_sector(sector_number, data->buffer);
	  if(result == 0)
	    return data;
	  else{
	    if(DEBUG_ON)
	      printf("Erro na leitura do setor especificado! (Setor #%d)\n", sector_number);
	    return NULL;
	  }
	}
}

WORD read_word(sector_t* sector){
  WORD result = 0, buffer = 0;
  result = sector->buffer[sector->current_index++];
  buffer = sector->buffer[sector->current_index++];
  result += buffer << 8;
  return result;
}

BYTE read_byte(sector_t* sector){
  char result = sector->buffer[sector->current_index++];
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

  current_directory->current_cluster = _superbloco->RootSectorStart;
  current_directory->first_cluster   = _superbloco->RootSectorStart;
  current_directory->current_entry   = 0;
  current_directory->cluster_index   = 0;

  working_directory = (dir_t *) malloc(sizeof(dir_t));
  working_directory->fullpath = (char *) calloc(1024, sizeof(char));

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

		directory->current_entry++;
		if(directory->current_entry % ((SECTOR_SIZE/32)*_superbloco->SectorPerCluster) == 0){
			if(DEBUG_ON)
				printf("FIM DO CLUSTER\n");
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
	 				printf("\n\nArquivo encontrado: \n");
	 				break;
	 			case 2:
	 				printf("\n\nSubdiretorio encontrado:\n");
	 				break;
	 		}
	 		printf("\tNome: %s, tamanho: %lu\n", dentry->name, dentry->fileSize);
	 		printf("\tFirst cluster: %hu\n", firstCluster);
	 		printf("\tIndice no subdiretorio: %d\n", directory->current_entry - 1);
	 		
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
	if(dir->first_cluster != _superbloco->RootSectorStart && FAT->data[dir->current_cluster-2] == FINAL_CLUSTER){
		dir->is_final_cluster = true;
	}
	else{
		dir->is_final_cluster = false;
	}
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
	{
		if(path[0] == '/')
    		set_working_to_root();
  		else
    		mirror_paths(CURR_TO_WORK);
		next_dir = strtok(path, "/");
	}
	if(next_dir == NULL){
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
	if(strlen(name) > MAX_FILE_NAME_SIZE)
		return false;
	Bool valid = true;
	char *aux = name;
	while(valid && *aux != '\0'){
		if((*aux < '0' || *aux > '9') && (*aux < 'a' || *aux > 'z') && (*aux < 'A' || *aux > 'Z'))
			valid = false;
		else
			aux++;
	}
	return valid;

}

int get_handler(int type){
	Bool found = false;
	int result = -1;
	int i = 0;
	switch(type){
		case DIR_HANDLER:
			while(!found && i < 20){
				if(directories[i].active == false){
					result = i;
					directories[i].active = true;
					found = true;
				}
				i++;
			}
			break;
		case FILE_HANDLER:
			while(!found && i < 20){
				if(files[i].active == false){
					result = i;
					files[i].active = true;
					found = true;
				}
				i++;
			}
			break;
	}
	return result;
}


/*

		CONSERTAR ESSA FUNCAO (funcionando apenas pro diretorio raiz)

*/
int get_first_invalid_entry(){
	rewind_dir(working_directory);
	sector_t *sector;
	short i;
	short type;
	Bool found = false;
	if(working_directory->first_cluster == _superbloco->RootSectorStart){
		int entries_in_root = (_superbloco->DataSectorStart - _superbloco->RootSectorStart) * (SECTOR_SIZE/32);
		while(!found && working_directory->current_entry < entries_in_root){
			sector = initialize_sector(_superbloco->RootSectorStart + (working_directory->current_entry / (SECTOR_SIZE/32)));
			for(i = 0; i < working_directory->current_entry % (SECTOR_SIZE/32); i++){
						read_dword(sector);
						read_dword(sector);
						read_dword(sector);
						read_dword(sector);
		
						read_dword(sector);
						read_dword(sector);
						read_dword(sector);
						read_dword(sector);
			}
			type = read_byte(sector);
			if(type == 0)
				found = true;
			else
				working_directory->current_entry++;

		}
		if(found){
			return 0;
		}
		else{
			return -1;
		}
	}
	else{
		rewind_dir(working_directory);
		int clusters_first_sector;
		int sector_within_cluster;
		while(!found && !(working_directory->current_entry / ((SECTOR_SIZE/32)*_superbloco->SectorPerCluster) >= 1 
			&& working_directory->is_final_cluster == true)){
			clusters_first_sector = _superbloco->DataSectorStart + (working_directory->current_cluster-2)*_superbloco->SectorPerCluster;
			sector_within_cluster = (working_directory->current_entry % (_superbloco->SectorPerCluster * (SECTOR_SIZE/32))) / (SECTOR_SIZE/32);
			
			sector = initialize_sector(clusters_first_sector + sector_within_cluster);

			for(i = 0; i < working_directory->current_entry % (SECTOR_SIZE/32); i++){
						read_dword(sector);
						read_dword(sector);
						read_dword(sector);
						read_dword(sector);
		
						read_dword(sector);
						read_dword(sector);
						read_dword(sector);
						read_dword(sector);
			}
			type = read_byte(sector);
			if(type == 0)
				found = true;
			else
				working_directory->current_entry++;

		}
		if(found)
			return 0;
		else
			/*
				TODO: EXPANDIR PARA ADICIONAR CLUSTER AO DIRETORIO
			*/
			return -1;
	}
}

int update_FAT(){
	int i, j;
	for(i = 0; i < FAT->size_in_sectors; i++){
		sector_t *sector      = initialize_sector(_superbloco->pFATSectorStart + i);
		sector_t *sector_copy = initialize_sector(_superbloco->sFATSectorStart + i);
		for(j = 0; j < (SECTOR_SIZE/2); j++){
			write_word(sector,       FAT->data[(SECTOR_SIZE/2)*i + j]);
			write_word(sector_copy,  FAT->data[(SECTOR_SIZE/2)*i + j]);
		
		}
		update_sector(sector);
		update_sector(sector_copy);
		free(sector);
	}
	return 0;
}

int get_free_fat_index(){
	int index = 0;
	Bool found = false;
	while(!found && index < (FAT->size_in_sectors)*(SECTOR_SIZE/2)){
		if(DEBUG_ON)
			printf("Analisando endereco %d da fat. Valor: %d\n", index, FAT->data[index]);
		if(FAT->data[index] == 0){
			if(DEBUG_ON) printf("FOUND IT!\n");
			found = true;
		}
		else
			index++;
	}
	return index + 2;
}

int update_entry(record_t new_record){
	sector_t *sector;
	if(strcmp(working_directory->fullpath, "/") == 0){
		sector = initialize_sector(_superbloco->RootSectorStart + (working_directory->current_entry / (SECTOR_SIZE/32)));
	}
	else{
		int cluster_base_sector = _superbloco->DataSectorStart + (working_directory->current_cluster-2)*_superbloco->SectorPerCluster;
		int real_sector = cluster_base_sector + (working_directory->current_entry / (SECTOR_SIZE/32));
		sector = initialize_sector(real_sector);
	}

	int result = 0;
	result = seek_sector(sector, working_directory->current_entry % (SECTOR_SIZE/32));
	if(result < 0)
		return -1;
	write_byte(sector, new_record.TypeVal);
	int i;
	for(i = 0; i < MAX_FILE_NAME_SIZE; i++){
		write_byte(sector, new_record.name[i]);
	}
	write_dword(sector, 0);
	write_word(sector, new_record.firstCluster);

	update_sector(sector);
	free(sector);

	return 0;
}


int mkdir_relative(char *folder_name){
	if(!is_valid(folder_name)){
		if(DEBUG_ON)
			printf("Nome de diretorio invalido: %s.\n", folder_name);
		return -1;
	}


	int entry_index = get_first_invalid_entry();
	if(entry_index < 0)
		return -1;

	int fat_index = get_free_fat_index();
	if(fat_index < 0)
		return -1;
	
	record_t new_entry; 
	new_entry.TypeVal = 2;
	memset(&(new_entry.name), '\0', MAX_FILE_NAME_SIZE);
	strcpy(&(new_entry.name[0]), folder_name);
	new_entry.firstCluster = fat_index;

	if(DEBUG_ON){
		printf("Entrada da FAT livre para a gravacao do novo diretorio: %d\n", new_entry.firstCluster);
		printf("Indice da entrada livre dentro do diretorio: %d\n", working_directory->current_entry);
		printf("Nome do novo diretorio: %s\n", new_entry.name);
		printf("A ser gravado na entrada %d do setor %d\n", working_directory->current_entry, working_directory->current_cluster);
	}

	int result = update_entry(new_entry);
	if(result < 0)
		return -1;

	// sector_t *sector = initialize_sector(-1);
	// char dir_name[MAX_FILE_NAME_SIZE];
	// int i;
	// for(i = 0; i < MAX_FILE_NAME_SIZE; i++){
	// 	dir_name[i] = '\0';
	// }
	

	// dir_name[0] = '.';
	// write_byte(sector, 2);
	// for(i = 0; i < MAX_FILE_NAME_SIZE; i++){
	// 	write_byte(sector, dir_name[i]);
	// }
	// write_dword(sector, 0);
	// write_word(sector, fat_index);

	// dir_name[1] = '.';
	// write_byte(sector, 2);
	
	// for(i = 0; i < MAX_FILE_NAME_SIZE; i++){
	// 	write_byte(sector, dir_name[i]);
	// }
	// write_dword(sector, 0);
	// if(working_directory->first_cluster == _superbloco->RootSectorStart)
	// 	write_word(sector, 0);
	// else
	// 	write_word(sector, working_directory->first_cluster);

	// FAT->data[fat_index - 2] = FINAL_CLUSTER;
	// update_FAT();

	// update_index(sector, fat_index * _superbloco->SectorPerCluster);
	// update_sector(sector);

	return 0;
}