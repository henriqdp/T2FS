#ifndef AUX_H
#define AUX_H

#define DIR_HANDLER  0
#define FILE_HANDLER 1

#define CURR_TO_WORK 2
#define WORK_TO_CURR 3

#define CREATE_FILE 10
#define CREATE_DIR  11

#include "apidisk.h"
#include "t2fs.h"

sector_t *initialize_sector(int sector_number);
int write_byte(sector_t *sector, BYTE byte);
int write_word(sector_t *sector, WORD word);
int write_dword(sector_t *sector, DWORD dword);
int update_sector(sector_t *sector);
int update_index(sector_t *sector, int new_sector_index);
WORD read_word(sector_t* sector);
BYTE read_byte(sector_t* sector);
DWORD read_dword(sector_t* sector);
int read_superblock();

int read_next_entry(dir_t *directory, DIRENT2 *dentry, WORD *cluster);

void mirror_paths();
void set_working_to_root();

void rewind_dir(dir_t *dir);
int change_dir(char *path, Bool subsequent);


Bool split_path(char *path);

Bool is_valid(char *name);
Bool entry_exists(char *name);

int get_handler(int type);
int get_first_invalid_entry();
int create_relative(char *name, int type);

int get_free_fat_index();
int update_FAT();

FILE2 open_relative(char *filename);
int read_bytes(FILE2 handle, int size, char *buffer);

#endif