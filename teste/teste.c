#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "t2fs.h"


int main(int argc, char **argv){
   system("clear");
   char *autores = (char *) malloc(sizeof(char)*512);
   identify2(autores, 512);
   
   char *pathname = (char *) calloc(1024, sizeof(char));

   char lixo;
   
   char *command   =  (char *) calloc(1024, sizeof(char));
   char *argument  =  (char *) calloc(1024, sizeof(char));
   char *argument2 =  (char *) calloc(1024, sizeof(char));
   int exit_required = 0;
   while(!exit_required){
   	getcwd2(pathname, 1024);
    
    printf("Carissimi@MEAN-machine:%s$ ", pathname);
   	scanf("%s", command);


   	if(strcmp(command, "clear") == 0){
   		system("clear");
   		continue;
   	}

   	if(strcmp(command, "exit") == 0){
   		system("clear");
   		exit_required = 1;
   		continue;
   	}

      if(strcmp(command, "create") == 0){
         scanf("%s", argument);
         FILE2 arquivo = create2(argument);
         if(arquivo < 0)
            printf("ERRO: caminho do diretorio ou nome do arquivo invalido.\n");
         continue;
      }

      if(strcmp(command, "show") == 0){
         scanf("%s", argument);
         FILE2 arquivo = open2(argument);
         if(arquivo < 0){
            printf("ERRO: caminho do diretorio ou nome do arquivo invalido.\n");
         }
         else{
            char *buffer = (char *) calloc(1024, sizeof(char));
            memset(buffer, '\0', 1024);
            int lidos = read2(arquivo, buffer, 200);
            printf("====== CONTEUDO DO ARQUIVO ========== \n\n");
            int i;
            for(i = 0; i < lidos; i++)
               printf("%c", *(buffer + i));
            printf("\n\n=====================================\n");
            printf("A funcao leu no total %d bytes\n", lidos);
            free(buffer);
         }
         continue;
      }

   	if(strcmp(command, "ls") == 0){
   		DIRENT2 *entry = (DIRENT2 *) malloc(sizeof(DIRENT2));
   		while(readdir2(CURR_DIR, entry) != -1){
   			switch(entry->fileType){
   				case 1:
   					printf("ARQ %s %ub\n", entry->name, entry->fileSize);
   					break;
   				case 2:
   					printf("DIR %s \n", entry->name);
   					break;
   			}
   		}
   		free(entry);
   		continue;
   	}

   	if(strcmp(command, "cd") == 0){
   		scanf("%s", argument);
   		int result = chdir2(argument);
   		if(result == -1)
   			printf("Erro! Caminho especificado invalido.\n");
   		continue;
   	}

   	if(strcmp(command, "id") == 0){
  		printf("Autores do trabalho: ");
  		puts(autores); 
   		continue;
   	}

      if(strcmp(command, "write") == 0){
         scanf("%s", argument);
         FILE2 file = open2(argument);
         if(file >= 0){
            scanf("%s", argument2);
            seek2(file, 0);
            int result = write2(file, argument2, strlen(argument2));
            if(result < 0){
               printf("Erro na escrita.\n");
            }
         }
         continue;

      }

      if(strcmp(command, "append") == 0){
         printf("\nEITA!!\n");
         scanf("%s", argument);
         FILE2 file = open2(argument);
         if(file >= 0){
            scanf("%s", argument2);
            seek2(file, -1);
            int result = write2(file, argument2, strlen(argument2));
            if(result < 0){
               printf("Erro na escrita.\n");
            }
         }
         continue;

      }

      if(strcmp(command, "mkdir") == 0){
         scanf("%s", argument);
         int result = mkdir2(argument);
         if(result < 0){
            printf("Erro na criacao do novo diretorio.\n");
         }
         continue;
      }

      if(strcmp(command, "rmdir") == 0){
         scanf("%s", argument);
         int result = rmdir2(argument);
         if(result < 0){
            printf("Erro na remocao do diretorio especificado.\n");
         }
         continue;
      }

      if(strcmp(command, "rm") == 0){
         scanf("%s", argument);
         int result = delete2(argument);
         if(result < 0)
            printf("Erro na remocao do arquivo %s\n", argument);
         continue;
      }
      
   	printf("Comando desconhecido! Por favor, va ler o manual.\n");
      while(lixo != '\n')
         scanf("%c", &lixo);
      lixo = '\0';
   }

  return 0;
}
