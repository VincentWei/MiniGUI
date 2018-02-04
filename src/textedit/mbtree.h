/*
 *   This file is part of MiniGUI, a mature cross-platform windowing 
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 * 
 *   Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
 *   Copyright (C) 1998~2002, WEI Yongming
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */
/*
** btree.h: this head file declares btree structure.
**
** Create date: 2010/03/10
*/

#ifndef _COMM_BTREE_H
#define _COMM_BTREE_H

#define BTREE_ERRNO_SCHILD  1
#define BTREE_ERRNO_SNEXT   0
#define BTREE_ERRNO_SNULL  -1 

DECLARE_OBJECT(mCommBTreeNode)
DECLARE_OBJECT(mCommBTree)
DECLARE_OBJECT(mCommBTreeLeafIterator)

#define mCommBTreeLeafIteratorHeader(Clss)       \
    mObjectHeader(Clss)                         \
    mCommBTree       *tree;                      \
    mCommBTreeNode   *current;

#define mCommBTreeLeafIteratorClassHeader(Clss, Super)\
    mObjectClassHeader(Clss, Super)             \
    mCommBTreeNode* (*getCurrent)(Clss *self);   \
    BOOL (*hasNext)(Clss *self);                 \
    mCommBTreeNode* (*next)(Clss *self);         \
    void (*remove)(Clss *self);                 \
    void (*insert)(Clss *self, mCommBTreeNode *leaf);


#define mCommBTreeNodeHeader(Clss)               \
    mObjectHeader(Clss)                         \
    Clss    *parent;                            \
    Clss    *next;                              \
    Clss    *children;                          \
    unsigned int level:16;                      \
    /* Number of children of this node. */      \
    unsigned int numChildren:16;                \
    /* Total number of leaves in the subtree rooted here. */\
    unsigned int numLeaves; 


#define mCommBTreeNodeClassHeader(Clss, Super)       \
    mObjectClassHeader(Clss, Super)                 \
    void (*resetKey)(Clss *self);                   \
    int (*compareNode)(Clss *self, void *cmpInfo);  \
    void (*recompute)(Clss *self, Clss *child);     \
    void (*changeKey)(Clss *self, void *diffInfo, int deltaChild); \
    BOOL (*isNullKey)(Clss *self);

#define mCommBTreeHeader(Clss)   \
	mObjectHeader(Clss)         \
    mCommBTreeNode *rootNode;    \
    int minChildrenNum;         \
    int maxChildrenNum;         \
    int refCount;

#define mCommBTreeClassHeader(Clss, Super)       \
    mObjectClassHeader(Clss, Super)             \
    void (*ref)(Clss *self);                    \
    void (*unref)(Clss *self);                  \
    mCommBTreeNode* (*getRoot)(Clss *self);      \
    void (*rebalanceNode)(Clss *self, mCommBTreeNode *node);     \
    void (*recomputeNodeInfo)(Clss *self, mCommBTreeNode *node); \
    void (*deleteNode)(Clss *self, mCommBTreeNode *node);        \
    BOOL (*insertLeaf)(Clss *self, mCommBTreeNode *parent,       \
            mCommBTreeNode *sibLeaf, mCommBTreeNode *leaf);       \
    mCommBTreeNode* (*search)(Clss *self, void* searchInfo, int flags);     \
    mCommBTreeNode* (*newNode)(Clss *self);                      \
    mCommBTreeNode* (*newLeaf)(Clss *self);                  

//search flags
enum {
	BTSF_RETLAST_IF_OUTOFRANGE = 0x1
};

DEFINE_OBJECT(mCommBTreeNode, mObject)
DEFINE_OBJECT(mCommBTree, mObject)
DEFINE_OBJECT(mCommBTreeLeafIterator, mObject)
#endif
