/*
** $Id: mbtree.h 13050 2010-07-26 08:18:49Z dongjunjie $
**
** btree.h: this head file declares btree structure.
**
** Copyright (C) 2010 Feynman Software.
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
