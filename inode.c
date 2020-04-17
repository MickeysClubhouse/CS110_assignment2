#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include "inode.h"
#include "diskimg.h"
#include <stdint.h>

#define DISKIMG_SECTOR_SIZE 512
#define BLOCKS_PER_NODE 256
#define INDIRECT_COUNT  7

int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {

  struct filsys superblock = fs->superblock;  //superblock
  int dfd = fs->dfd;            //dfd, Handle from the diskimg module to read the diskimg.
  int sector_length = DISKIMG_SECTOR_SIZE / sizeof(struct inode);  // inode start sector
  int sector_index = (inumber - 1) / sector_length;   // the inode block offset
  int index_offset = (inumber - 1) % sector_length;   // the location of the inode within the sector/block
  struct inode inodes_array [sector_length];
  if (diskimg_readsector(dfd, INODE_START_SECTOR + sector_index, inodes_array) < 0) return -1;  // Read blocks into buffer

  *inp = inodes_array[index_offset];

  return 0;
}

int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {

  struct filsys superblock = fs->superblock;  // superblock
  int dfd = fs->dfd;   // dfd file descriptor
  if ((inp->i_mode & IALLOC) == 0) return -1;   //Check if the inode is unalloacted

  if ((inp->i_mode & ILARG) == 0) {

    return  inp->i_addr[blockNum];       // Inode points to direct blocks
  } else {
    int n = blockNum / BLOCKS_PER_NODE;     // 7 indirect and 1 doubly indirect
    int k = blockNum % BLOCKS_PER_NODE;     // blockNumber = n * 256 + k;
    if (n < INDIRECT_COUNT) {          // if blockNumber is found in one of the first 7 blocks
      int block = inp->i_addr[n];
      short buf[BLOCKS_PER_NODE];      // 256 block addresses in the specified block
      if( diskimg_readsector(dfd, block, buf) < 0) return -1;

      return buf[k];
    } else {
      int block = inp->i_addr[INDIRECT_COUNT];       //if the blockNumber is found in the last block
      short doubly_buf[BLOCKS_PER_NODE];             //last block contains 256 indirect block addresses
      if (diskimg_readsector(dfd, block, doubly_buf) < 0) return -1;

      int m = doubly_buf[n - INDIRECT_COUNT];
      short singly_buf[BLOCKS_PER_NODE];             // 256 block addresses in the indirect block
      if (diskimg_readsector(dfd, m, singly_buf) < 0) return -1;

      return singly_buf[k];
    }
  }

  return -1;
}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1);
}
