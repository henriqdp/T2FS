#include "t2fs.h"
#include "apidisk.h"
#include "aux.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CREATE_FUNCTION 10
#define DELETE_FUNCTION 12
#define OPEN_FUNCTION   13
#define MKDIR_FUNCTION  14
#define RMDIR_FUNCTION  15

#define ERR_INVALID_PATH           -1
#define ERR_INVALID_FUNCTION_CODE  -1
#define ERR_FOLDER_NOT_EMPTY       -2

int split_and_execute(int function, char *path){
if(!superblock_read){
    read_superblock();
  }
  Bool split = split_path(path);
  char *path_aux = (char *) calloc(1024, sizeof(char));
  char *argument;
  memset(path_aux, '\0', 1024);
  strcpy(path_aux, path);
  int result;
  if(split){
    if(DEBUG_ON){
      printf("Diretorio no qual sera executada a funcao: ");
      puts(path);
      printf("Nome do arquivo/diretorio: ");
      puts(path + strlen(path) + 1);
    }
    result = change_dir(path_aux, false);
    free(path_aux);
    if(result < 0)
      return ERR_INVALID_PATH;
    else
      argument = path  + strlen(path) + 1;
  }
  else{
    free(path_aux);
    if(DEBUG_ON){
      printf("Nome do arquivo/diretorio: ");
        puts(path);
      }

    if(path[0] == '/'){
      set_working_to_root();
      argument = path + 1;
    }
    else{
      mirror_paths(CURR_TO_WORK);
      argument = path;
    }
  }
  switch(function){
    case CREATE_FUNCTION:
      return create_relative(argument, CREATE_FILE);
    case DELETE_FUNCTION:
      return remove_relative(argument, REMOVE_FILE);
    case OPEN_FUNCTION:
      return open_relative(argument);
    case MKDIR_FUNCTION:
      return create_relative(argument, CREATE_DIR);
    case RMDIR_FUNCTION:
      return remove_relative(argument, REMOVE_DIR);
  }
  return ERR_INVALID_FUNCTION_CODE;
}

int identify2 (char *name, int size){
  if(!superblock_read){
    read_superblock();
  }
  if(strlen("Henrique de Paula Lopes - 205689") > size){
    return -1;
  }
  else{
    memset(name, '\0', size);
    strcpy(name, "Henrique de Paula Lopes - 205689");
    return 0;
  }
  
}

FILE2 create2 (char *filename){
  return (FILE2) split_and_execute(CREATE_FUNCTION, filename);
}

int delete2 (char *filename){
  return split_and_execute(DELETE_FUNCTION, filename);
}

FILE2 open2 (char *filename){
  return (FILE2) split_and_execute(OPEN_FUNCTION, filename);
}

int close2 (FILE2 handle){
  if(!superblock_read){
    read_superblock();
  }
  if(handle < 0 || handle >= 20 || files[handle].active == false){
    if(DEBUG_ON)
      printf("Handle de arquivo invalido.\n");
  }
  else
    files[handle].active = false;
  return 0;
}

int read2 (FILE2 handle, char *buffer, int size){
  if(!superblock_read){
    read_superblock();
  }
  if(buffer == NULL)
    return -1;

  if(!files[handle].active || buffer == NULL)
    return -1;
  else{
    return read_bytes(handle, size, buffer);
  }
  
}

int write2 (FILE2 handle, char *buffer, int size){
  if(!superblock_read){
    read_superblock();
  }
  if(buffer == NULL)
   return -1;
  
  if(!files[handle].active || buffer == NULL)
    return -1;
  else{
    return write_bytes(handle, size, buffer);
  }
}

int seek2 (FILE2 handle, unsigned int offset){
  if(!superblock_read){
    read_superblock();
  }
  if(handle < 0 || handle >= 20 || files[handle].active == false || offset > files[handle].size){
    if(DEBUG_ON)
      printf("Handle de arquivo invalido.\n");
  }

  if(offset != -1){
      files[handle].current_byte = offset;
    }
    else{
    files[handle].current_byte = files[handle].size;
  }


  return 0;
}

int mkdir2 (char *pathname){
  return split_and_execute(MKDIR_FUNCTION, pathname);
}

int rmdir2 (char *pathname){
  if(!superblock_read){
    read_superblock();
  }

 
  char *pathname_aux = (char *) calloc(1024, sizeof(char));
  memset(pathname_aux, '\0', 1014);
  strcpy(pathname_aux, pathname);
  int result = change_dir(pathname, false);
  int number_of_entries = 0;
  DIRENT2 entry;
  WORD clusterNo;
  if(result < 0)
    return ERR_INVALID_PATH;
  else{
    while(read_next_entry(working_directory, &entry, &clusterNo) != -1){
      number_of_entries++;
    }
    if(number_of_entries > 2){
      return ERR_FOLDER_NOT_EMPTY;
    }
  }
  free(pathname_aux);
  return split_and_execute(RMDIR_FUNCTION, pathname);
}

DIR2 opendir2 (char *pathname){
  if(!superblock_read){
    read_superblock();
  }
  
  char *pathname_aux = (char *) calloc(1024, sizeof(char));
  strcpy(pathname_aux, pathname);
  int result = change_dir(pathname_aux, false);
  free(pathname_aux);
  if(result < 0)
    return -1;
  else{
    DIR2 result = (DIR2) get_handle(DIR_HANDLE);
    if(result < 0)
      return -1;
    else{
      directories[result].first_cluster = working_directory->first_cluster;
      directories[result].current_cluster = working_directory->first_cluster;
      directories[result].current_entry = 0;
      if(directories[result].fullpath == NULL)
        directories[result].fullpath = (char *) calloc(1024, sizeof(char));
      strcpy(directories[result].fullpath, working_directory->fullpath);
      return result + 1;
    }
  }
}

int readdir2 (DIR2 handle, DIRENT2 *dentry){
  if(!superblock_read){
    read_superblock();
  }
  WORD cluster;
  if(handle == CURR_DIR){
    return read_next_entry(current_directory, dentry, &cluster);
  }
  else{
    if(handle > 20 || handle < 0 || directories[handle-1].active == false){
      if(DEBUG_ON)
        printf("Erro em readdir2: handle invalido!\n");
      return -1;
    }
    else{
      return read_next_entry(&(directories[handle-1]), dentry, &cluster);
    }
  }
}

int closedir2 (DIR2 handle){
  if(!superblock_read){
    read_superblock();
  }
  if(handle > 20 || handle <= 0 || directories[handle-1].active == false){
    if(DEBUG_ON)
      printf("Erro em closedir2: handle invalido!\n");
    return -1;
  }
  else{
    directories[handle-1].active = false;
    return 0; 
  }
}

int chdir2 (char *pathname){
  if(!superblock_read){
    read_superblock();
  }
  
  if(change_dir(pathname, false) == 0){
    mirror_paths(WORK_TO_CURR);
    return 0;
  }
  else{
    return -1;
  }
}

int getcwd2 (char *pathname, int size){
  if(!superblock_read){
    read_superblock();
  }
  if(strlen(current_directory->fullpath) > size)
    return -1;
  else{
    strcpy(pathname, current_directory->fullpath);
  }
  return 0;
}


