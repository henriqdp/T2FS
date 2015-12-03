#ifndef __LIBT2FS___
#define __LIBT2FS___


#include <stdlib.h>
#include "apidisk.h"

#define DEBUG_ON 0
#define CURR_DIR 0

#define FINAL_CLUSTER 65535
#define FREE_CLUSTER  0
#define BAD_CLUSTER   65534


//definicao de erros diversos
#define ERR_END_OF_DIRECTORY     -1
#define ERR_NOT_A_DIRECTORY      -2
#define ERR_HANDLE_NOT_AVAILABLE -3
#define ERR_ROOT_DIR_FULL        -4
#define ERR_FAT_FULL             -5
#define ERR_DIR_FULL             -6
#define ERR_INVALID_ENTRY_NAME   -7
#define ERR_BUFFER_NULL          -8
#define ERR_ENTRY_ALREADY_EXISTS -9


typedef int FILE2;
typedef int DIR2;

typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef unsigned int DWORD;

typedef enum{
  false = 0,
  true = 1	
} Bool;

/** Superbloco */
struct t2fs_superbloco {
    char    Id[4];            /* Identificação do sistema de arquivo. É formado pelas letras "T2FS". */
    WORD    Version;          /* Versão atual desse sistema de arquivos: (valor fixo 7DF=2015; 2=2 semestre). */
    WORD    SuperBlockSize;   /* Quantidade de setores  que formam o superbloco. (fixo em 1 setor lógico) */
    DWORD   DiskSize;         /* Tamanho total, em bytes (superbloco, FAT, diretório raiz e clusters de dados). */
    DWORD   NofSectors;       /* Quantidade total de setores lógicos (superbloco, FAT, diretório raiz e blocos de dados). */
    DWORD   SectorPerCluster; /* Número de setores lógicos que formam 1 cluster. */
    DWORD   pFATSectorStart;  /* Primeiro setor lógico da FAT principal. */
    DWORD   sFATSectorStart;  /* Primeiro setor lógico da FAT secundária (cópia). */
    DWORD   RootSectorStart;  /* Primeiro setor lógico do diretório raiz. */
    DWORD   DataSectorStart;  /* Primeiro setor lógico da área de blocos de dados (cluster). */
    DWORD   NofDirEntries;    /* Quantidade de entradas no diretório raiz. */
} __attribute__((packed));


#define MAX_FILE_NAME_SIZE 25

/** Registro de diretório (entrada de diretório) */
#define TYPEVAL_INVALIDO    0x00
#define TYPEVAL_REGULAR     0x01
#define TYPEVAL_DIRETORIO   0x02
struct t2fs_record {
    BYTE    TypeVal;                   /* Tipo da entrada. Indica se o registro é inválido (0x00), arquivo (0x01) ou diretório (0x02) */
    char    name[MAX_FILE_NAME_SIZE];  /* Nome do arquivo. : string com caracteres ASCII (0x21 até 0x7A), case sensitive. */
	DWORD	bytesFileSize;	           /* Tamanho do arquivo, em bytes. */
	WORD	firstCluster;	           /* número do primeiro cluster de dados correspondente a essa entrada de diretório. */
} __attribute__((packed));


typedef struct {
    char name[MAX_FILE_NAME_SIZE+1];	/* Nome do arquivo, conforme aparece na entrada de diretório */
    int fileType;                       /* Indica se é arquivo (0) ou diretório (1) */
    unsigned int fileSize;             /* número de bytes do arquivo */
} DIRENT2;

typedef struct t2fs_superbloco superbloco_t;
typedef struct t2fs_record     record_t;

superbloco_t *_superbloco;
Bool superblock_read;

typedef struct FAT_descriptor{
	unsigned int size_in_sectors;
	WORD *data;
}fat_t;

typedef struct directory_descriptor{
	char *fullpath;
	unsigned short first_cluster;
	unsigned short current_cluster;
	unsigned int current_entry;
	unsigned short cluster_index;
	Bool active;
	Bool is_final_cluster;
} dir_t;

typedef struct file_descriptor{
	char *filename;
	unsigned int size;
	unsigned short first_cluster;
	unsigned short current_cluster;
	unsigned long current_byte;
	unsigned short folder_first_cluster;
	Bool active;
	Bool is_final_cluster;
}file_t;

typedef struct sector_descriptor{
	short unsigned int current_index;
	unsigned short sector_index;
	char buffer[SECTOR_SIZE];
} sector_t;

fat_t *FAT;
dir_t *current_directory;
dir_t *working_directory;

dir_t  directories[20];
file_t files[20];



/*-----------------------------------------------------------------------------
função: Usada para identificar os desenvolvedores do T2FS.
		Essa função copia um string de identificação para o ponteiro indicado por "name".
		Essa cópia não pode exceder o tamanho do buffer, informado pelo parâmetro "size".
		O string deve ser formado apenas por caracteres ASCII (Valores entre 0x20 e 0x7A) e terminado por '\0'.
		O string deve conter o nome e número do cartào dos participantes do grupo.

Entra:	name -> buffer onde colocar o string de identificação.
		size -> tamanho do buffer "name" (número máximo de bytes a serem copiados).

saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int identify2 (char *name, int size);


/*-----------------------------------------------------------------------------
função: Criar um novo arquivo.
		O nome desse novo arquivo é aquele informado pelo parâmetro "filename".
		O contador de posição do arquivo (current pointer) deve ser colocado na posição zero.
		Caso já exista um arquivo ou diretório com o mesmo nome, a função deverá retornar um erro de criação.
		A função deve retornar o identificador (handle) do arquivo.
		Esse handle será usado em chamadas posteriores do sistema de arquivo para fins de manipulação do arquivo criado.

Entra:	filename -> nome do arquivo a ser criado.

saída:	Se a operação foi realizada com sucesso, a função retorna o handle do arquivo (número positivo).
		Em caso de erro, deve ser retornado um valor negativo.
-----------------------------------------------------------------------------*/
FILE2 create2 (char *filename);


/*-----------------------------------------------------------------------------
função:	Apagar um arquivo do disco.
		O nome do arquivo a ser apagado é aquele informado pelo parâmetro "filename".

Entra:	filename -> nome do arquivo a ser apagado.

saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int delete2 (char *filename);


/*-----------------------------------------------------------------------------
função:	Abre um arquivo existente no disco.
		O nome desse novo arquivo é aquele informado pelo parâmetro "filename".
		Ao abrir um arquivo, o contador de posição do arquivo (current pointer) deve ser colocado na posição zero.
		A função deve retornar o identificador (handle) do arquivo.
		Esse handle será usado em chamadas posteriores do sistema de arquivo para fins de manipulação do arquivo criado.
		Todos os arquivos abertos por esta chamada são abertos em leitura e em escrita.
		O ponto em que a leitura, ou escrita, será realizada é fornecido pelo valor current_pointer (ver função seek2).

Entra:	filename -> nome do arquivo a ser apagado.

saída:	Se a operação foi realizada com sucesso, a função retorna o handle do arquivo (número positivo)
		Em caso de erro, deve ser retornado um valor negativo
-----------------------------------------------------------------------------*/
FILE2 open2 (char *filename);


/*-----------------------------------------------------------------------------
função:	Fecha o arquivo identificado pelo parâmetro "handle".

Entra:	handle -> identificador do arquivo a ser fechado

saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int close2 (FILE2 handle);


/*-----------------------------------------------------------------------------
função:	Realiza a leitura de "size" bytes do arquivo identificado por "handle".
		Os bytes lidos são colocados na área apontada por "buffer".
		Após a leitura, o contador de posição (current pointer) deve ser ajustado para o byte seguinte ao último lido.

Entra:	handle -> identificador do arquivo a ser lido
		buffer -> buffer onde colocar os bytes lidos do arquivo
		size -> número de bytes a serem lidos

saída:	Se a operação foi realizada com sucesso, a função retorna o número de bytes lidos.
		Se o valor retornado for menor do que "size", então o contador de posição atingiu o final do arquivo.
		Em caso de erro, será retornado um valor negativo.
-----------------------------------------------------------------------------*/
int read2 (FILE2 handle, char *buffer, int size);


/*-----------------------------------------------------------------------------
função:	Realiza a escrita de "size" bytes no arquivo identificado por "handle".
		Os bytes a serem escritos estão na área apontada por "buffer".
		Após a escrita, o contador de posição (current pointer) deve ser ajustado para o byte seguinte ao último escrito.

Entra:	handle -> identificador do arquivo a ser escrito
		buffer -> buffer de onde pegar os bytes a serem escritos no arquivo
		size -> número de bytes a serem escritos

saída:	Se a operação foi realizada com sucesso, a função retorna o número de bytes efetivamente escritos.
		Em caso de erro, será retornado um valor negativo.
-----------------------------------------------------------------------------*/
int write2 (FILE2 handle, char *buffer, int size);


/*-----------------------------------------------------------------------------
função:	Reposiciona o contador de posições (current pointer) do arquivo identificado por "handle".
		A nova posição é determinada pelo parâmetro "offset".
		O parâmetro "offset" corresponde ao deslocamento, em bytes, contados a partir do início do arquivo.
		Se o valor de "offset" for "-1", o current_pointer deverá ser posicionado no byte seguinte ao final do arquivo,
			Isso é útil para permitir que novos dados sejam adicionados no final de um arquivo ja existente.

Entra:	handle -> identificador do arquivo a ser escrito
		offset -> deslocamento, em bytes, onde posicionar o "current pointer".

saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int seek2 (FILE2 handle, unsigned int offset);


/*-----------------------------------------------------------------------------
função:	Criar um novo diretório.
		O caminho desse novo diretório é aquele informado pelo parâmetro "pathname".
			O caminho pode ser ser absoluto ou relativo.
		A criação de um novo subdiretório deve ser acompanhada pela criação, automática, das entradas "." e ".."
		A entrada "." corresponde ao descritor do subdiretório recém criado
		A entrada ".." corresponde é entrada de seu diretório pai.
		são considerados erros de criação quaisquer situações em que o diretório não possa ser criado.
			Isso inclui a existência de um arquivo ou diretório com o mesmo "pathname".

Entra:	pathname -> caminho do diretório a ser criado

saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int mkdir2 (char *pathname);


/*-----------------------------------------------------------------------------
função:	Apagar um subdiretório do disco.
		O caminho do diretório a ser apagado é aquele informado pelo parâmetro "pathname".
		são considerados erros quaisquer situações que impeçam a operação.
			Isso inclui:
				(a) o diretório a ser removido não está vazio;
				(b) "pathname" não existente;
				(c) algum dos componentes do "pathname" não existe (caminho inválido);
				(d) o "pathname" indicado não é um arquivo;
				(e) o "pathname" indica os diretórios "." ou "..".

Entra:	pathname -> caminho do diretório a ser criado

saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int rmdir2 (char *pathname);


/*-----------------------------------------------------------------------------
função:	Abre um diretório existente no disco.
		O caminho desse diretório é aquele informado pelo parâmetro "pathname".
		Se a operação foi realizada com sucesso, a função:
			(a) deve retornar o identificador (handle) do diretório
			(b) deve posicionar o ponteiro de entradas (current entry) na primeira posição válida do diretório "pathname".
		O handle retornado será usado em chamadas posteriores do sistema de arquivo para fins de manipulação do diretório.

Entra:	pathname -> caminho do diretório a ser aberto

saída:	Se a operação foi realizada com sucesso, a função retorna o identificador do diretório (handle).
		Em caso de erro, será retornado um valor negativo.
-----------------------------------------------------------------------------*/
DIR2 opendir2 (char *pathname);


/*-----------------------------------------------------------------------------
função:	Realiza a leitura das entradas do diretório identificado por "handle".
		A cada chamada da função é lida a entrada seguinte do diretório representado pelo identificador "handle".
		Algumas das informações dessas entradas devem ser colocadas no parâmetro "dentry".
		Após realizada a leitura de uma entrada, o ponteiro de entradas (current entry) deve ser ajustado para a próxima entrada válida, seguinte é última lida.
		são considerados erros:
			(a) qualquer situação que impeça a realização da operação
			(b) término das entradas válidas do diretório identificado por "handle".

Entra:	handle -> identificador do diretório cujas entradas deseja-se ler.
		dentry -> estrutura de dados onde a função coloca as informações da entrada lida.

saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero ( e "dentry" não será válido)
-----------------------------------------------------------------------------*/
int readdir2 (DIR2 handle, DIRENT2 *dentry);


/*-----------------------------------------------------------------------------
função:	Fecha o diretório identificado pelo parâmetro "handle".

Entra:	handle -> identificador do diretório que se deseja fechar (encerrar a operação).

saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int closedir2 (DIR2 handle);


/*-----------------------------------------------------------------------------
função:	Altera o diretório atual de trabalho (working directory).
		O caminho desse diretório é informado no parâmetro "pathname".
		são considerados erros:
			(a) qualquer situação que impeça a realização da operação
			(b) não existência do "pathname" informado.

Entra:	pathname -> caminho do novo diretório de trabalho.

saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int chdir2 (char *pathname);


/*-----------------------------------------------------------------------------
função:	Informa o diretório atual de trabalho.
		O "pathname" do diretório de trabalho deve ser copiado para o buffer indicado por "pathname".
			Essa cópia não pode exceder o tamanho do buffer, informado pelo parâmetro "size".
		são considerados erros:
			(a) quaisquer situações que impeçam a realização da operação
			(b) espaço insuficiente no buffer "pathname", cujo tamanho está informado por "size".

Entra:	pathname -> buffer para onde copiar o pathname do diretório de trabalho
		size -> tamanho do buffer pathname

saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int getcwd2 (char *pathname, int size);


#endif
