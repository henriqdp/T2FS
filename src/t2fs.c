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
  Bool split = split_path(pathname);
  if(split){
    printf("Caminho dividido: ");
    puts(pathname);
    printf("Nome do diretorio: ");
    puts(pathname + strlen(pathname) + 1);
  }
  else{
    printf("Nome do diretorio a ser criado localmente: ");
    puts(pathname);

    /*
      casos:
        - caminho relativo X caminho absoluto
        - caminho absoluto com subdiretorio a ser criado no raiz X qqr outro lugar
        
    */

  }return 0;
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
  if(handle == CURR_DIR){
    WORD cluster;
    return read_next_entry(current_directory, dentry, &cluster);
  }
  else{
    if(handle > 20 || handle < 0 || directories[handle-1].active == false){
      printf("Erro em readdir2: handle invalido!\n");
      return -1;
    }
    else{
      /*
        TODO
      */
      return 0;
    }
  }
}

int closedir2 (DIR2 handle){
  if(!superblock_read){
    read_superblock();
  }
  if(handle > 20 || handle < 0 || directories[handle-1].active == false){
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
  if(pathname[0] == '/')
    set_working_to_root();
  else
    mirror_paths(CURR_TO_WORK);

  return change_dir(pathname, false);
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


