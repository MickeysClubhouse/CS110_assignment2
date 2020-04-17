#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define DIRENTV6_SIZE  16
#define DIRENTV6_PER_BLOCK  32

int directory_findname(struct unixfilesystem *fs, const char *name, int dirinumber, struct direntv6 *dirEnt) {
  //inode to store all the dirents
  struct inode inp;
  // get inode with inumber dirinumber
  if (inode_iget(fs, dirinumber, &inp) < 0) return -1;
  // get sizes of inode
  int size = inode_getsize(&inp);
  // check if the inode is a valid directory
  if ((inp.i_mode & IFMT) == IFDIR) {
    int block_num = size / DISKIMG_SECTOR_SIZE;
    // increment the block number if the size of the inode is not a multiple of 512
    if (size % DISKIMG_SECTOR_SIZE != 0) block_num++;
    //for each block
    for (int i = 0; i < block_num; i++) {
      //load the block data into 512 = 32 * "size 16 direntv6 array"
      struct direntv6 dirent[DIRENTV6_PER_BLOCK];
      if (file_getblock(fs, dirinumber, i, dirent) < 0) continue;
      // for each direntv6, check if the d_name matches the given name
      for (int i = 0; i < DIRENTV6_PER_BLOCK; i++) {
	int d_inumber = dirent[i].d_inumber;
	char* d_name = dirent[i].d_name;
	// If the names match, load the directory entry into space addressed by dirEnt
	if(strncmp(name, d_name, strlen(name)) == 0) {
	  *dirEnt = dirent[i];
	  return 0;
	}
      }
    }
  }
  return -1;
}
