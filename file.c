#include <stdio.h>
#include <assert.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

// remove the placeholder implementation and replace with your own
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {

  int dfd = fs->dfd;   // file descriptor
  struct inode inp;
  if (inode_iget(fs, inumber, &inp) < 0) return -1;       // load data into inode

  int number = inode_indexlookup(fs, &inp, blockNum);     // find a block with the given block number
  if (number < 0) return -1;             // error reading block

  int size = inode_getsize(&inp);        // size of the inode
  if(diskimg_readsector(dfd, number, buf) < 0) return -1;    //load data into buffer

  if (size % DISKIMG_SECTOR_SIZE == 0) return DISKIMG_SECTOR_SIZE;  // if size of the inode is a multiple of 512, return 512

  if (size % DISKIMG_SECTOR_SIZE != 0 && blockNum == size/DISKIMG_SECTOR_SIZE){  //if the size is not, and the block is the last block
    return size % DISKIMG_SECTOR_SIZE;       //return the size of the valid data in the last block
  }

  return DISKIMG_SECTOR_SIZE;           // otherwise, always return 512
}
