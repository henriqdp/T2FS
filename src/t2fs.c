#include "t2fs.h"
#include "apidisk.h"
#include "aux.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  if(handle < 0 || handle >= 20 || files[handle].active == false || offset > files[handle].size){
    if(DEBUG_ON)
      printf("Handle de arquivo invalido.\n");
  }

  if(offset != -1)
    files[handle].current_byte = offset;
  else{
    files[handle].current_byte = files[handle].size;
  }


  return 0;
}

int mkdir2 (char *pathname){
  if(!superblock_read){
    read_superblock();
  }
  Bool split = split_path(pathname);
  int result;
  if(split){
    if(!DEBUG_ON){
      printf("Diretorio no qual sera criado o arquivo: ");
      puts(pathname);
      printf("Nome do diretorio: ");
      puts(pathname + strlen(pathname) + 1);
    }
    result = change_dir(pathname, false);
    if(result < 0)
      return -1;
    else
      return mkdir_relative(pathname + strlen(pathname) + 1);
  }
  else{
    if(!DEBUG_ON){
      printf("Nome do diretorio a ser criado: ");
        puts(pathname);
      }

    if(pathname[0] == '/'){
      set_working_to_root();
      return mkdir_relative(pathname + 1);
    }
    else{
      mirror_paths(CURR_TO_WORK);
      return mkdir_relative(pathname);
    }
  }
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
  
  char *pathname_aux = (char *) calloc(1024, sizeof(char));
  strcpy(pathname_aux, pathname);
  int result = change_dir(pathname_aux, false);
  free(pathname_aux);
  if(result < 0)
    return -1;
  else{
    DIR2 result = (DIR2) get_handler(DIR_HANDLER);
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
      return read_next_entry(&(directories[handle]), dentry, &cluster);
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


