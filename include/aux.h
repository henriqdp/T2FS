#ifndef AUX_H
#define AUX_H

#define DIR_HANDLE  0
#define FILE_HANDLE 1

#define CURR_TO_WORK 2
#define WORK_TO_CURR 3

#define CREATE_FILE 10
#define CREATE_DIR  11
#define REMOVE_FILE 12
#define REMOVE_DIR  13

#include "apidisk.h"
#include "t2fs.h"


/*
	FUNCOES DE LEITURA E ESCRITA EM UM SETOR: leem e escrevem bytes em um setor,
		atualizando o ponteiro deste. O setor possui um indice relativo ao numero
		do setor do disco que ele representa e um indice interno, que aponta pro byte
		prestes a ser lido/escrito. O indice pode ser manipulado.
*/

sector_t *initialize_sector(int sector_number);
int write_byte(sector_t *sector, BYTE byte);
int write_word(sector_t *sector, WORD word);
int write_dword(sector_t *sector, DWORD dword);
int update_sector(sector_t *sector);
int update_index(sector_t *sector, int new_sector_index);
WORD read_word(sector_t* sector);
BYTE read_byte(sector_t* sector);
DWORD read_dword(sector_t* sector);


//Funcao para ler o superbloco do disco e preencher a estrutura correspondente.
//Primeiro uso das funcoes que manipulam a representacao do setor em memoria.
int read_superblock();

//Funcao para ler a proxima entrada valida do diretorio representado em directory.
int read_next_entry(dir_t *directory, DIRENT2 *dentry, WORD *cluster);

/*
	FUNCOES QUE MANIPULAM OS DIRETORIOS CORRENTES:
		cada vez que uma mudanca deve acontecer no disco (como a adicao ou a remocao de uma pasta),
		o programa precisa navegar ate o diretorio onde acontecera a remocao ou a adicao da entrada.
		Para isso existe a variavel "working directory", que tenta, em um primeiro momento, chegar no local.
		Ja a variavel "current_directory" guarda o diretorio corrente no qual o programa se encontra. 
		O caminho salvo nesta estrutura que e' mostrado na funcao getcwd2.
*/
void mirror_paths();
void set_working_to_root();
void rewind_dir(dir_t *dir);
int change_dir(char *path, Bool subsequent);

//Funcao que pega um caminho de diretorio (ex: ./henrique/arquivo) e verifica se 
//no caminho indicado existe ou nao mais de um diretorio representado (no exemplo acima,
//exitem 3: ., henrique e arquivo). Se sim, retorna true e falso em caso contrario.
//A funcao e' usada em quase todas as funcoes que aceitam caminhos por extenso como parametro
Bool split_path(char *path);

//verifica se um nome fornecido para batizar um novo arquivo ou diretorio e' valido
Bool is_valid(char *name);

//verifica se o nome que batizara o arquivo prestes a ser criado ja nao e' o nome
//de outra entrada do diretorio
Bool entry_exists(char *name);

//encontra um handle de arquivo/diretorio que nao esteja sendo usado
int get_handle(int type);

//faz o ponteiro do working_directory apontar para a primeira entrada invalida do diretorio
//usada para gravar uma nova entrada
int get_first_invalid_entry();

//funcoes de criacao, remocao e abertura de arquivos/diretorios
//devem ser usadas em conjunto com a change_dir, pois so funcionam localmente
int create_relative(char *name, int type);
FILE2 open_relative(char *filename);
int remove_relative(char *pathname, int type);

//funcoes da FAT
int get_free_fat_index();
int update_FAT();

//funcoes de leitura e escrita de um arquivo
int read_bytes(FILE2 handle, int size, char *buffer);
int write_bytes(FILE2 handle, int size, char *buffer);


#endif