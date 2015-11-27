#ifndef AUX_H
#define AUX_H

#define RELATIVE 0
#define ABSOLUTE 1

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

void rewind_dir(dir_t *dir);
int change_dir(int mode, char *path);


#endif