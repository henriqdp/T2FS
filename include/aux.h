#ifndef AUX_H
#define AUX_H

#define DIR_HANDLER  0
#define FILE_HANDLER 1

#define CURR_TO_WORK 2
#define WORK_TO_CURR 3

#include "apidisk.h"
#include "t2fs.h"

sector_t *initialize_sector(int sector_number);
WORD read_word(sector_t* sector);
BYTE read_byte(sector_t* sector);
DWORD read_dword(sector_t* sector);
int read_superblock();

int read_next_entry(dir_t *directory, DIRENT2 *dentry, WORD *cluster);

void mirror_paths();
void set_working_to_root();

void rewind_dir(dir_t *dir);
int change_dir(char *path, Bool subsequent);

int get_first_invalid_entry();
Bool split_path(char *path);

Bool is_valid(char *name);

int get_handler(int type);
#endif