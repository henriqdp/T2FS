#include "apidisk.h"
#include "t2fs.h"
#include "aux.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int write_byte(sector_t *sector, BYTE byte){
	if(sector->current_index > SECTOR_SIZE - 1)
		return -1;
	sector->buffer[sector->current_index++] = byte;
	return 0;
}

int write_word(sector_t *sector, WORD word){
	if(sector->current_index > SECTOR_SIZE - 2)
		return -1;
	char aux = word % 256;
	sector->buffer[sector->current_index++] = aux;
	aux = word >> 8;
	sector->buffer[sector->current_index++] = aux;
	return 0;
}

int write_dword(sector_t *sector, DWORD dword){
	if(sector->current_index > SECTOR_SIZE - 4)
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
	  	exit(EXIT_FAILURE);
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
  DWORD result = 0x00, buffer = 0;
  result = (unsigned short) sector->buffer[sector->current_index++] % 65280;
  buffer = (unsigned short) sector->buffer[sector->current_index++] % 65280;
  result += buffer << 8;
  
  buffer = (unsigned short) sector->buffer[sector->current_index++] % 65280;
  result += buffer << 16;
  buffer = (unsigned short) sector->buffer[sector->current_index++] % 65280;
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
  
  FAT->data = (WORD *) calloc(SECTOR_SIZE/2 * FAT->size_in_sectors, sizeof(WORD));
  for(i = 0; i < FAT->size_in_sectors; i++){
     read_sector(_superbloco->pFATSectorStart + i, (char *)FAT->data + i*(SECTOR_SIZE/2));   
   }

  
   //especificacao do diretorio raiz na memoria
  current_directory = (dir_t *) malloc(sizeof(dir_t));
  current_directory->fullpath = (char *) calloc(1024, sizeof(char));
  memset(current_directory->fullpath, '\0', 1024);
  strcpy(current_directory->fullpath, "/");	

  current_directory->current_cluster = 0;
  current_directory->first_cluster   = 0;
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
		printf("Primeiro cluster do diretorio: %d\n", directory->first_cluster);
	if(directory->first_cluster == 0){
		real_sector = _superbloco->RootSectorStart + directory->current_entry / (SECTOR_SIZE/32);

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
				return ERR_END_OF_DIRECTORY;
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
	
	seek_sector(sector, current_entry_within_sector);
	dentry->fileType = (int) read_byte(sector);
	int i;
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
	 		printf("\tNome: %s, tamanho: %u\n", dentry->name, dentry->fileSize);
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
	if(dir->first_cluster != 0 && FAT->data[dir->current_cluster-2] == FINAL_CLUSTER){
		dir->is_final_cluster = true;
	}
	else{
		dir->is_final_cluster = false;
	}
}



void set_working_to_root(){
	working_directory->first_cluster    = 0;
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

	if(working_directory->first_cluster == 0)
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
		if(entry->fileType != 2){
			if(DEBUG_ON)
				printf("Erro! Arquivo especificado nao e' um diretorio.\n");
			return ERR_NOT_A_DIRECTORY;
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

Bool entry_exists(char *name){
	rewind_dir(working_directory);
	
	Bool exists = false;

	WORD cluster_number;
	DIRENT2 *entry = (DIRENT2 *) malloc(sizeof(DIRENT2));
	while(!exists && read_next_entry(working_directory, entry, &cluster_number) != -1)
	{
		if(strcmp(name, entry->name) == 0)
			exists = true;
	}
	return exists;

}

int get_handle(int type){
	Bool found = false;
	int result = -1;
	int i = 0;
	switch(type){
		case DIR_HANDLE:
			while(!found && i < 20){
				if(directories[i].active == false){
					result = i;
					directories[i].active = true;
					found = true;
				}
				i++;
			}
			break;
		case FILE_HANDLE:
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
	if(result != -1)
		return result;
	else
		return ERR_HANDLE_NOT_AVAILABLE;
}


int get_first_invalid_entry(){
	rewind_dir(working_directory);
	sector_t *sector;
	short type;
	Bool found = false;
	if(working_directory->first_cluster == 0){
		int entries_in_root = (_superbloco->DataSectorStart - _superbloco->RootSectorStart) * (SECTOR_SIZE/32);
		while(!found && working_directory->current_entry < entries_in_root){
			sector = initialize_sector(_superbloco->RootSectorStart + (working_directory->current_entry / (SECTOR_SIZE/32)));
			seek_sector(sector, working_directory->current_entry % (SECTOR_SIZE/32));
			type = read_byte(sector);
			if(type == 0)
				found = true;
			else
				working_directory->current_entry++;
			free(sector);

		}
		
		if(found){
			return 0;
		}
		else{
			return ERR_ROOT_DIR_FULL;
		}
	}
	else{
		rewind_dir(working_directory);
		int clusters_first_sector;
		int sector_within_cluster;

		while(!found){
			if(working_directory->current_entry != 0 
				&& working_directory->current_entry % ((SECTOR_SIZE/32)*_superbloco->SectorPerCluster) == 0){
				if(FAT->data[working_directory->current_cluster-2] != FINAL_CLUSTER)
					working_directory->current_cluster = FAT->data[working_directory->current_cluster-2];
				else{
					int new_fat = get_free_fat_index();
					if(new_fat != ERR_FAT_FULL){
						FAT->data[working_directory->current_cluster-2] = new_fat;
						working_directory->current_cluster = FAT->data[working_directory->current_cluster-2];
						FAT->data[working_directory->current_cluster-2] = FINAL_CLUSTER;
						update_FAT();
					}
					else
						return ERR_FAT_FULL;
				}
			}

			clusters_first_sector = _superbloco->DataSectorStart + (working_directory->current_cluster-2)*_superbloco->SectorPerCluster;
			sector_within_cluster = (working_directory->current_entry % (_superbloco->SectorPerCluster * (SECTOR_SIZE/32))) / (SECTOR_SIZE/32);
			
			sector = initialize_sector(clusters_first_sector + sector_within_cluster);
			seek_sector(sector, working_directory->current_entry % (SECTOR_SIZE/32));
			type = read_byte(sector);
			free(sector);
			if(type == 0)
				found = true;
			else
				working_directory->current_entry++;
		}

		return 0;
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
		free(sector_copy);
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
			found = true;
		}
		else
			index++;
	}
	if(found)
		return index + 2;
	else
		return ERR_FAT_FULL;
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

	seek_sector(sector, working_directory->current_entry % (SECTOR_SIZE/32));
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


int create_relative(char *name, int type){
	if(!is_valid(name)){
		if(DEBUG_ON)
			printf("Nome de diretorio invalido: %s.\n", name);
		return ERR_INVALID_ENTRY_NAME;
	}

	if(entry_exists(name))
		return ERR_ENTRY_ALREADY_EXISTS;

	int entry_index = get_first_invalid_entry();
	if(entry_index < 0)
		return entry_index;

	int fat_index = get_free_fat_index();
	if(fat_index < 0)
		return fat_index;

	FILE2 handle;
	if(type == CREATE_FILE){
		handle = get_handle(FILE_HANDLE);
		if(handle < 0)
			return handle;
	}
	
	record_t new_entry; 
	if(type == CREATE_FILE)
		new_entry.TypeVal = 1;
	else
		new_entry.TypeVal = 2;
	memset(&(new_entry.name), '\0', MAX_FILE_NAME_SIZE);
	strcpy(&(new_entry.name[0]), name);
	new_entry.firstCluster = fat_index;

	if(DEBUG_ON){
		printf("Entrada da FAT livre para a gravacao do novo diretorio: %d\n", new_entry.firstCluster);
		printf("Indice da entrada livre dentro do diretorio: %d\n", working_directory->current_entry);
		printf("Nome do novo diretorio: %s\n", new_entry.name);
		printf("A ser gravado na entrada %d do setor %d\n", working_directory->current_entry, working_directory->current_cluster);
	}

	update_entry(new_entry);
	
	FAT->data[new_entry.firstCluster-2] = FINAL_CLUSTER;
	update_FAT();

	if(type == CREATE_DIR){
		sector_t *sector = initialize_sector(-1);
		char dir_name[MAX_FILE_NAME_SIZE];
		int i;
		for(i = 0; i < MAX_FILE_NAME_SIZE; i++){
			dir_name[i] = '\0';
		}
		

		dir_name[0] = '.';
		write_byte(sector, 2);
		for(i = 0; i < MAX_FILE_NAME_SIZE; i++){
			write_byte(sector, dir_name[i]);
		}
		write_dword(sector, 0);
		write_word(sector, fat_index);

		dir_name[1] = '.';
		write_byte(sector, 2);
		
		for(i = 0; i < MAX_FILE_NAME_SIZE; i++){
			write_byte(sector, dir_name[i]);
		}
		write_dword(sector, 0);
		if(working_directory->first_cluster == _superbloco->RootSectorStart)
			write_word(sector, 0);
		else
			write_word(sector, working_directory->first_cluster);

		update_index(sector, _superbloco->DataSectorStart + (new_entry.firstCluster-2)*_superbloco->SectorPerCluster);
		update_sector(sector);
		free(sector);
		return 0;
	}
	else{
		files[handle].active = true;
		files[handle].size = 0;
		files[handle].first_cluster = new_entry.firstCluster;
		files[handle].current_cluster = new_entry.firstCluster;
		files[handle].current_byte = 0;
		files[handle].folder_first_cluster = working_directory->first_cluster;
		if(files[handle].filename == NULL)
			files[handle].filename = (char *) calloc(MAX_FILE_NAME_SIZE + 1, sizeof(char));
		strcpy(files[handle].filename, name);
		return (int) handle;
	}
}

FILE2 open_relative(char *filename){
	FILE2 handle = get_handle(FILE_HANDLE);
	if(handle < 0){
		return handle;
	}
	Bool found = false;
	rewind_dir(working_directory);
	DIRENT2 file;
	WORD clusterNo;
	while(!found && read_next_entry(working_directory, &file, &clusterNo) != -1){
		if(file.fileType == 1 && strcmp(file.name, filename) == 0){
			if(files[handle].filename == NULL){
				if(DEBUG_ON)
					printf("Alocando memoria para o nome do arquivo.\n");
				files[handle].filename = (char *) calloc(MAX_FILE_NAME_SIZE + 1, sizeof(char));
			}
			memset(files[handle].filename, '\0', MAX_FILE_NAME_SIZE + 1);
			memcpy(files[handle].filename, filename, MAX_FILE_NAME_SIZE);
			files[handle].size = file.fileSize;
			files[handle].first_cluster = clusterNo;
			files[handle].folder_first_cluster = working_directory->first_cluster;
			files[handle].current_byte = 0;
			files[handle].current_cluster = clusterNo;
			found = true;
		}
	}
	if(found)
	{
		if(DEBUG_ON){
			printf("Nome do arquivo: ");
			puts(files[handle].filename);
			printf("Primeiro cluster do arquivo: %d\n", files[handle].first_cluster);
			printf("Primeiro cluster do diretorio que o contem: %d\n", files[handle].folder_first_cluster);
			printf("Tamanho do arquivo em bytes: %u\n",files[handle].size);
			printf("handle do arquivo: %d\n", handle);
		}
		return handle;
	}
	else
		return ERR_INVALID_ENTRY_NAME;
}

int read_bytes(FILE2 handle, int size, char *buffer){
	int bytes_lidos = 0;
	if(size == 0)
		return 0;
	sector_t *sector;
	int i;
	while(bytes_lidos < size && files[handle].current_byte < files[handle].size){

		sector = initialize_sector(_superbloco->DataSectorStart + ((files[handle].current_cluster-2) * _superbloco->SectorPerCluster)
							+ ((files[handle].current_byte % (SECTOR_SIZE * _superbloco->SectorPerCluster)) / SECTOR_SIZE));

		for(i = 0; i < files[handle].current_byte % SECTOR_SIZE; i++){
			read_byte(sector);
		}

		do{
			*(buffer + bytes_lidos) = read_byte(sector);
			bytes_lidos++;
			files[handle].current_byte++;
		}while(files[handle].current_byte % SECTOR_SIZE != 0 && bytes_lidos < size && files[handle].current_byte < files[handle].size);
		
		if(files[handle].current_byte % (SECTOR_SIZE * _superbloco->SectorPerCluster) == 0 && files[handle].is_final_cluster == false){
			files[handle].current_cluster = FAT->data[files[handle].current_cluster-2];
			if(FAT->data[files[handle].current_cluster-2] == FINAL_CLUSTER)
				files[handle].is_final_cluster = true;
		}
		free(sector);
	}
	return bytes_lidos;
}

int write_bytes(FILE2 handle, int size, char *buffer){

	int written_bytes = 0;
	if(size == 0)
		return 0;
	if(buffer == NULL)
		return ERR_BUFFER_NULL;
	sector_t *sector;
	int i;

	if(files[handle].size == files[handle].current_byte 
		&& files[handle].current_byte % (SECTOR_SIZE * _superbloco->SectorPerCluster) == 0
		&& files[handle].size > 0)
		{
			if(files[handle].is_final_cluster){
				int new_cluster = get_free_fat_index();
				FAT->data[files[handle].current_cluster-2] = new_cluster;
				FAT->data[new_cluster-2] = FINAL_CLUSTER;
				files[handle].current_cluster = new_cluster;
				update_FAT();
			}
	}

	while(written_bytes < size){
			sector = initialize_sector(_superbloco->DataSectorStart + ((files[handle].current_cluster-2) * _superbloco->SectorPerCluster)
							+ ((files[handle].current_byte % (SECTOR_SIZE * _superbloco->SectorPerCluster)) / SECTOR_SIZE));
			for(i = 0; i < files[handle].current_byte % SECTOR_SIZE; i++)
				read_byte(sector);
			do{
				write_byte(sector, (BYTE) *(buffer + written_bytes++));
				files[handle].current_byte++;
			}while(files[handle].current_byte % SECTOR_SIZE != 0 && written_bytes < size);
			
			if(files[handle].current_byte % (SECTOR_SIZE * _superbloco->SectorPerCluster) == 0 
				&& written_bytes < size){
				if(files[handle].is_final_cluster){
					int new_cluster = get_free_fat_index();
					if(new_cluster != ERR_FAT_FULL){
						FAT->data[files[handle].current_cluster-2] = new_cluster;
						FAT->data[new_cluster-2] = FINAL_CLUSTER;
						files[handle].current_cluster = new_cluster;
						update_FAT();
					}
					else
						return ERR_FAT_FULL;
				}
			}
			update_sector(sector);
			free(sector);
	}


	if(files[handle].current_byte + 1 > files[handle].size){
		working_directory->first_cluster = files[handle].folder_first_cluster;
		rewind_dir(working_directory);
		files[handle].size = files[handle].current_byte;
		DIRENT2 dentry;
		WORD clusterNo;
		Bool found = false;
		int entry_index = 0;
		sector_t *sector;
		while(!found){
			read_next_entry(working_directory, &dentry, &clusterNo);
			if(strcmp(dentry.name, files[handle].filename) == 0){
				entry_index = working_directory->current_entry - 1;
				found = true;
			}
		}
		if(working_directory->current_cluster == 0){
			if(DEBUG_ON){
				printf("Byte corrente: %lu\n", files[handle].current_byte);
				printf("Tamanho do arquivo: %u\n", files[handle].size);
			}
			sector = initialize_sector(_superbloco->RootSectorStart + (entry_index / (SECTOR_SIZE/32)));

		} 	
		else{
			sector = initialize_sector(_superbloco->DataSectorStart 
										+ (working_directory->current_cluster-2)*(_superbloco->SectorPerCluster)
										+ (entry_index % (SECTOR_SIZE*_superbloco->SectorPerCluster/32))/(SECTOR_SIZE/32));
			
		}
		seek_sector(sector, entry_index % (SECTOR_SIZE/32));
		read_byte(sector);
		for(i = 0; i < MAX_FILE_NAME_SIZE; i++)
			read_byte(sector);
		write_dword(sector, files[handle].size);
		update_sector(sector);
		free(sector);
	}
	return written_bytes;
	
}


int remove_relative(char *name, int type){
	rewind_dir(working_directory);
	WORD clusterNo;
	DIRENT2 entry;
	Bool found = false;
	sector_t *sector;
	if(type == REMOVE_FILE){
		while(!found && read_next_entry(working_directory, &entry, &clusterNo) != -1){
			if(strcmp(name, entry.name) == 0 && entry.fileType == 1){
				found = true;

			}
		}
	}
	else{
			while(!found && read_next_entry(working_directory, &entry, &clusterNo) != -1){
				if(strcmp(name, entry.name) == 0 && entry.fileType == 2){
					found = true;

				}
			}
	}
	if(found){
		if(DEBUG_ON){
			printf("Entrada encontrada!\n");
			printf("Entrada no indice %d do diretorio.\n", working_directory->current_entry -1);
			printf("Primeiro cluster: %d\n", clusterNo);
		}
		if(working_directory->first_cluster == 0){
			sector = initialize_sector(_superbloco->RootSectorStart + (working_directory->current_entry - 1) / (SECTOR_SIZE/32));
		}
		else{
			sector = initialize_sector(_superbloco->DataSectorStart + (working_directory->current_cluster-2)*_superbloco->SectorPerCluster
				+ ((working_directory->current_entry-1) % ((SECTOR_SIZE/32)*_superbloco->SectorPerCluster))/(SECTOR_SIZE/32));
		}
		seek_sector(sector, (working_directory->current_entry - 1) % (SECTOR_SIZE/32));
		write_byte(sector, 0);
		update_sector(sector);
		unsigned short fat_addr  = clusterNo;
		unsigned short next_addr = FAT->data[fat_addr-2];
		FAT->data[fat_addr-2] = 0;
		while(next_addr != FINAL_CLUSTER){
			fat_addr = next_addr;
			next_addr = FAT->data[fat_addr-2];
			FAT->data[fat_addr-2] = 0;
		}
		update_FAT();
		free(sector);
	}
	else{
		return ERR_INVALID_ENTRY_NAME;
	}
	return 0;

}