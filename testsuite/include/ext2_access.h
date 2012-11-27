#ifndef __EXT2_ACCESS_H
#define __EXT2_ACCESS_H

#include "ext2fs.h"


__u32 get_block_size(void * fs);

void * get_block(void * fs, __u32 block_num);

struct ext2_inode * get_inode(void * fs, __u32 inode_num);

__u32 get_inode_by_path(void * fs, char * path);

#endif

