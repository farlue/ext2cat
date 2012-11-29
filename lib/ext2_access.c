// ext2 definitions from the real driver in the Linux kernel.
#include "ext2fs.h"

// This header allows your project to link against the reference library. If you
// complete the entire project, you should be able to remove this directive and
// still compile your code.
#include "reference_implementation.h"

// Definitions for ext2cat to compile against.
#include "ext2_access.h"



///////////////////////////////////////////////////////////
//  Accessors for the basic components of ext2.
///////////////////////////////////////////////////////////

// Return a pointer to the primary superblock of a filesystem.
struct ext2_super_block * get_super_block(void * fs) {
  // The super block is at the super block offset.
  return fs + SUPERBLOCK_OFFSET;
}


// Return the block size for a filesystem.
__u32 get_block_size(void * fs) {
  // Use the block size metadata within the super block.
  return EXT2_BLOCK_SIZE(get_super_block(fs));
}


// Return a pointer to a block given its number.
// get_block(fs, 0) == fs;
void * get_block(void * fs, __u32 block_num) {
  // Return fs if block_num is 0.
  if (block_num == 0) {
    return fs;
  }
  // Return super block address if block_num is 1.
  if (block_num == 1) {
    return (void*)get_super_block(fs);
  }
  // Get the pointer to block 2, which is right after the super block.
  void * block_2_ptr = (void*)get_super_block(fs) + SUPERBLOCK_SIZE;
  // Calculate the address of the given block_num.
  return block_2_ptr + (block_num - 2) * get_block_size(fs);
}


// Return a pointer to the first block group descriptor in a filesystem. Real
// ext2 filesystems will have several of these, but, for simplicity, we will
// assume there is only one.
struct ext2_group_desc * get_block_group(void * fs, __u32 block_group_num) {
  // return the pointer to block 2, right after the super block.
  if (block_group_num == 0) {
    return (void*)get_super_block(fs) + SUPERBLOCK_SIZE;
  } else {
    return NULL;
  }
}


// Return a pointer to an inode given its number. In a real filesystem, this
// would require finding the correct block group, but you may assume it's in the
// first one.
struct ext2_inode * get_inode(void * fs, __u32 inode_num) {
  // Get the address of the block group descriptor.
  struct ext2_group_desc * group_desc_ptr = get_block_group(fs, 0);
  // Get the address of the inode table.
  void * inode_tbl_ptr = get_block(fs, group_desc_ptr->bg_inode_table);
  // Calculate the address of the inode.
  return inode_tbl_ptr + (inode_num - 1) * EXT2_INODE_SIZE(get_super_block(fs));
}



///////////////////////////////////////////////////////////
//  High-level code for accessing filesystem components by path.
///////////////////////////////////////////////////////////

// Chunk a filename into pieces.
// split_path("/a/b/c") will return {"a", "b", "c"}.
//
// This one's a freebie.
char ** split_path(char * path) {
    int num_slashes = 0;
    for (char * slash = path; slash != NULL; slash = strchr(slash + 1, '/')) {
        num_slashes++;
    }

    // Copy out each piece by advancing two pointers (piece_start and slash).
    char ** parts = (char **) calloc(num_slashes, sizeof(char *));
    char * piece_start = path + 1;
    int i = 0;
    for (char * slash = strchr(path + 1, '/');
         slash != NULL;
         slash = strchr(slash + 1, '/')) {
        int part_len = slash - piece_start;
        parts[i] = (char *) calloc(part_len, sizeof(char));
        strncpy(parts[i], piece_start, part_len);
        piece_start = slash + 1;
        i++;
    }
    // Get the last piece.
    parts[i] = (char *) calloc(strlen(piece_start) + 1, sizeof(char));
    strncpy(parts[i], piece_start, strlen(piece_start));
    return parts;
}


// Convenience function to get the inode of the root directory.
struct ext2_inode * get_root_dir(void * fs) {
    return get_inode(fs, EXT2_ROOT_INO);
}


// Given the inode for a directory and a filename, return the inode number of
// that file inside that directory, or 0 if it doesn't exist there.
// 
// name should be a single component: "foo.txt", not "/files/foo.txt".
__u32 get_inode_from_dir(void * fs, struct ext2_inode * dir, char * name) 
{   
  void *block = get_block(fs, dir->i_block[0]);
  struct ext2_dir_entry *directory = (struct ext2_dir_entry*)block;
  int i, equal;
  
  while(directory->inode != 0)
  {
     equal = 1;
     for(i=0; directory->name[i]>31;i++)
     {
       if(name[i]!=directory->name[i])
         equal = 0;
     }
     if(equal)
       return directory->inode;
        
     directory = (struct ext2_dir_entry*)((long int)directory + directory->rec_len);
  }
  
  return 0;
   
}


// Find the inode number for a file by its full path.
// This is the functionality that ext2cat ultimately needs.
__u32 get_inode_by_path(void * fs, char * path) {
    // FIXME: Uses reference implementation.
    return _ref_get_inode_by_path(fs, path);
}

