/*
 * OpenOS - Virtual File System
 * 
 * Provides a unified interface for file operations across different filesystems.
 * This is a placeholder for future implementation.
 */

#ifndef OPENOS_FS_VFS_H
#define OPENOS_FS_VFS_H

#include <stdint.h>
#include <stddef.h>

/* File operations placeholder */
typedef struct vfs_node {
    char name[128];
    uint32_t inode;
    uint32_t length;
    uint32_t flags;
} vfs_node_t;

/* Initialize VFS */
void vfs_init(void);

#endif /* OPENOS_FS_VFS_H */
