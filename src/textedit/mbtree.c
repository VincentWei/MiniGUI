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
 * btree.c: btree implementation.
 * 
 * Author: Yan Xiaowei
 *
 * Create date: 2004/03/01
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef __MINIGUI_LIB__
#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"

#include "object.h"
#elif __MGNCS_LIB__
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include "mgncsconfig.h"
#include "mcommon.h"
#include "mobject.h"
#include "mcomponent.h"
#endif

#if defined(_MGCTRL_TEXTEDIT_USE_NEW_IMPL) || defined(__MGNCS_TEXTEDITOR)

#include "mbtree.h"

static mCommBTreeNode* mCommBTree_newLeaf(mCommBTree *self)
{
    return NEW(mCommBTreeNode);
}

static mCommBTreeNode* mCommBTree_newNode(mCommBTree *self)
{
    return NEW(mCommBTreeNode);
}

static BOOL mCommBTree_insertLeaf(mCommBTree *self, 
        mCommBTreeNode *parent, mCommBTreeNode *sibLeaf, mCommBTreeNode *leaf)
{
    mCommBTreeNode *node = NULL;

    if (!self || !leaf)
        return FALSE;

    if (parent && (!sibLeaf || sibLeaf->parent == parent))
        node = parent;
    else if (!parent && sibLeaf)
        node = sibLeaf->parent;

    //insert a leaf
    if (node && node->level == 1) {
        leaf->level = 0;
        leaf->parent = node;

        if (sibLeaf) {
            //append to sibLeaf
            leaf->next = sibLeaf->next;
            sibLeaf->next = leaf;
        }
        else {
            //append to last
            mCommBTreeNode *child = parent->children;

            if (child) {
                while(child) {
                    if (child->next == NULL)
                        break;
                    child = child->next;
                }
                if (!child)
                    return FALSE;
                child->next = leaf;
            }
            else
                parent->children = leaf;
        }

        //TODO: numChildren, numLeaves
        _c(leaf)->changeKey(leaf, NULL, 1);

        if (node->numChildren > self->maxChildrenNum) {
            _c(self)->rebalanceNode(self, node);
        }
        return TRUE;
    }
    return FALSE;
}

static void mCommBTree_deleteNode(mCommBTree *self, mCommBTreeNode *node)
{
    mCommBTreeNode *parent;
    if (!self || !node)
        return;

    parent = (mCommBTreeNode*)node->parent;
    //root should have two children at least.
    if (parent == self->rootNode && parent->numChildren <= 2 && parent->level == 1)
	{
        _c(node)->changeKey(node, (void*)-1, -1);

		_c(node)->resetKey(node);
		node->children = NULL;
		node->level = 0;
		node->numLeaves = 0;

        _c(self)->recomputeNodeInfo(self, parent);
        return;
	}

    if (parent) {
        //delete node from children
        mCommBTreeNode *child = parent->children;

        if (child == node) {
            parent->children = child->next;
        }
        else {
            while(child) {
                if (child->next == node)
                    break;
                child = child->next;
            }
            if (!child) {
                return;
            }

            child->next = node->next;
        }

        if (node->level > 0) {
            _c(self)->recomputeNodeInfo(self, parent);
        }
        else {
            _c(node)->changeKey(node, (void*)-1, -1);
        }

        if (parent->numChildren < self->minChildrenNum) {
            _c(self)->rebalanceNode(self, parent);
        }
    }

    DELETE(node);
}

static void mCommBTree_recomputeNodeInfo(mCommBTree *self, mCommBTreeNode *node)
{
    mCommBTreeNode *leaf;
    if (!self || !node)
        return;

    node->numChildren = 0;
    node->numLeaves = 0;

    _c(node)->resetKey(node);

    leaf = node->children;
    while (leaf) {
        node->numChildren++;

        if (node->level <= 1) {
            node->numLeaves++;
        }
        else {
            node->numLeaves += leaf->numLeaves;
        }

        _c(node)->recompute(node, leaf);
        leaf = leaf->next;
    }
}

static void mCommBTree_rebalanceNode(mCommBTree *self, mCommBTreeNode *node)
{
    /*
    * Loop over the entire ancestral chain of the mCommBTreeNode, working
    * up through the self one mCommBTreeNode at a time until the root
    * mCommBTreeNode has been processed.
    */

    while (node) {
        mCommBTreeNode *newNode, *child;
        int i;

        /*
        * Check to see if the mCommBTreeNode has too many children.  If it does,
        * then split off all but the first MIN_CHILDREN into a separate
        * mCommBTreeNode following the original one. Then repeat until the
        * mCommBTreeNode has a decent size.
        */
        if (node->numChildren > self->maxChildrenNum) {
            while (1) {
                /*
                * If the mCommBTreeNode being split is the root
                * mCommBTreeNode, then make a new root mCommBTreeNode above
                * it first.
                */
                if (node->parent == NULL) {
                    newNode = _c(self)->newNode(self);
                    newNode->level = node->level + 1;
                    newNode->children = (mCommBTreeNode *)node;

                    _c(self)->recomputeNodeInfo(self, newNode);
                    self->rootNode = newNode;
                }

                /* append a sibling node after node. */
                newNode = _c(self)->newNode(self);
                newNode->parent = node->parent;
                newNode->next   = node->next;
                node->next      = newNode;
                newNode->level  = node->level;
                newNode->numChildren = node->numChildren - self->minChildrenNum;

                //found child in specified position.
                {
                    mCommBTreeNode *leaf;
                    for (i = self->minChildrenNum - 1,
                            leaf = node->children;
                            i > 0; i--, leaf = leaf->next) 
                    {
                        /* Empty loop body. */
                    }
                    newNode->children = leaf->next;
                    leaf->next = NULL;
                }

                _c(self)->recomputeNodeInfo(self, node);
                node->parent->numChildren++;
                node = newNode;

                if (node->numChildren <= self->maxChildrenNum) {
                    _c(self)->recomputeNodeInfo(self, node);
                    break;
                }
            } //end while(1)
        } //end if (node->numChildren > self->maxChildrenNum) 

        while (node->numChildren < self->minChildrenNum) {
            mCommBTreeNode *other, *halfwaynode = NULL;
            int total, first, i;

            /*
             * Too few children for this mCommBTreeNode. If this is the root then,
             * it's OK for it to have less than self->minChildrenNum children
             * as lon as it's at least two. If it has only one (and isn't at 
             * level 1), then chop the root mCommBTreeNode out of the tree and 
             * use its child as the new root.
             */

            if (node->parent == NULL) {
                if ((node->numChildren == 1) && (node->level > 1)) {
                    self->rootNode = (mCommBTreeNode*) node->children;
                    self->rootNode->parent = NULL;

                    node->children = NULL;
                    DELETE(node);
                }
                return;
            }

            /* Not root node. Make sure that there are siblings to balance with.*/
            if (node->parent->numChildren < 2) {
                _c(self)->rebalanceNode(self, node->parent);
                continue;
            }

            /* Find a sibling neighbour to borrow from, and arrange for
             * node to be the earlier of the pair.*/
            if (node->next == NULL) {
                for (other = (mCommBTreeNode*)(((mCommBTreeNode*)(node->parent))->children);
                        other->next != node;
                        other = other->next) 
                {
                    /* Empty loop body. */
                }
                node = other;
            }
            other = node->next;

            /*
            * We're going to either merge the two siblings together
            * into one mCommBTreeNode or redivide the children among them to
            * balance their loads.  As preparation, join their two
            * child lists into a single list and remember the half-way
            * point in the list.
            */
            total = node->numChildren + other->numChildren;
            first = total /2;

            if (node->children == NULL) {
                node->children = other->children;
                other->children = NULL;
            }

            for (child = node->children, i = 1;
                    child->next != NULL;
                    child = child->next, i++) {
                if (i == first)
                    halfwaynode = child;
            }
            child->next = other->children;

            while (i <= first) {
                halfwaynode = child;
                child = child->next;
                i++;
            }

            /* If the two siblings can simply be merged together, do it. */
            if (total <= self->maxChildrenNum) {
                _c(self)->recomputeNodeInfo(self, node);

                node->next = other->next;
                node->parent->numChildren--;

                other->children = NULL;
                DELETE(other);
                continue;
            }

            /* The siblings can't be merged, so just divide their
             * children evenly between them. */

            other->children = (mCommBTreeNode*)halfwaynode->next;
            halfwaynode->next = NULL;

            _c(self)->recomputeNodeInfo(self, node);
            _c(self)->recomputeNodeInfo(self, other);
        }
        node = node->parent;
    }
}

static mCommBTreeNode* _search_leaf(mCommBTreeNode *node, void *searchInfo, int flags)
{
    int ret;
    if (!node || !searchInfo)
        return NULL;

    ret = _c(node)->compareNode(node, searchInfo);

    if (ret == BTREE_ERRNO_SCHILD) {
        if (node->level == 0)
            return node;
        return _search_leaf(node->children, searchInfo,flags);
    } 
    else if (ret == BTREE_ERRNO_SNEXT) {
		if(!node->next && (flags & BTSF_RETLAST_IF_OUTOFRANGE))
		{
			while(node->level>0)
				node = node->children;
			return node;
		}
        return _search_leaf(node->next, searchInfo, flags);
    }

    return NULL;
}

static void* mCommBTree_search(mCommBTree *self, void* searchInfo, int flags)
{
    if (self) {
        return _search_leaf(self->rootNode, searchInfo, flags);
    }

    return NULL;
}

static void mCommBTree_construct(mCommBTree* self, va_list va)
{
    mCommBTreeNode *rootNode;
    _SUPER(mObject, self, construct, 0);

    self->refCount = 1;
    self->minChildrenNum = 6;
    self->maxChildrenNum = 12;

    /*The tree will initially have two empty lines. The second
     * line isn't actually part of the tree's contents, but its
     * presence makes several operations easier.*/

    /*Create the root node. */
    rootNode = _c(self)->newNode(self);
    if (rootNode) {
        mCommBTreeNode *leaf, *leaf2;

        leaf    = _c(self)->newLeaf(self);
        leaf2   = _c(self)->newLeaf(self);

        rootNode->level         = 1;
        rootNode->numChildren   = 2;
        rootNode->numLeaves     = 2;
        rootNode->children      = leaf;

        leaf->parent    = rootNode;
        leaf->next      = leaf2;
        leaf2->parent   = rootNode;
        leaf2->next     = NULL;
        
        self->rootNode = rootNode;
    }
}

static void mCommBTree_destroy(mCommBTree* self)
{
    _c(self)->unref(self);

    if (!self || self->refCount > 0)
        return;

    DELETE(self->rootNode);
    _SUPER(mObject, self, destroy);
}

static void mCommBTree_ref(mCommBTree *self)
{
    if (!self || self->refCount <= 0)
        return;

    self->refCount += 1;
}

static void mCommBTree_unref(mCommBTree *self)
{
    if (!self || self->refCount <= 0)
        return;

    self->refCount -= 1;
}

static mCommBTreeNode* mCommBTree_getRoot(mCommBTree *self)
{
    return self? self->rootNode : NULL;
}

BEGIN_MINI_CLASS(mCommBTree, mObject)
	CLASS_METHOD_MAP(mCommBTree, construct);
	CLASS_METHOD_MAP(mCommBTree, destroy);

	CLASS_METHOD_MAP(mCommBTree, ref);
	CLASS_METHOD_MAP(mCommBTree, unref);
	CLASS_METHOD_MAP(mCommBTree, getRoot);

	CLASS_METHOD_MAP(mCommBTree, newNode);
	CLASS_METHOD_MAP(mCommBTree, deleteNode);

	CLASS_METHOD_MAP(mCommBTree, newLeaf);
	CLASS_METHOD_MAP(mCommBTree, insertLeaf);

	CLASS_METHOD_MAP(mCommBTree, recomputeNodeInfo);
	CLASS_METHOD_MAP(mCommBTree, rebalanceNode);
	CLASS_METHOD_MAP(mCommBTree, search);
END_MINI_CLASS


static mCommBTreeNode* _get_left_leaf(mCommBTreeNode *node)
{
    mCommBTreeNode *child;
    if (!node)
        return NULL;

    child = node->children;
    if (!child || child->level == 0) {
        return child;
    }

    return _get_left_leaf(child);
}

static void mCommBTreeLeafIterator_construct(mCommBTreeLeafIterator* self, va_list va)
{
	ncsParseConstructParams(va, "pp", &self->tree, &self->current);
    _SUPER(mObject, self, construct, 0);

	if(self->tree && !self->current)
        self->current = _get_left_leaf(self->tree->rootNode);

}

static void mCommBTreeLeafIterator_remove(mCommBTreeLeafIterator* self)
{
    mCommBTreeNode *delNode = self->current;
    self->current = _c(self)->next(self);
    _c(self->tree)->deleteNode(self->tree, delNode);
}

static void mCommBTreeLeafIterator_insert(mCommBTreeLeafIterator* self, 
        mCommBTreeNode *leaf)
{
    if (!self || !self->tree || !self->current)
        return;

    if (_c(self->tree)->insertLeaf(self->tree, NULL, self->current, leaf))
        self->current = leaf;
}

static mCommBTreeNode* _get_next_parent(mCommBTreeNode *node)
{
    if (!node)
        return NULL;

    if (node->next)
        return node->next;

    return _get_next_parent(node->parent);
}

static mCommBTreeNode* mCommBTreeLeafIterator_getCurrent(mCommBTreeLeafIterator* self)
{
    if (self && self->tree)
        return self->current;

    return NULL;
}

static BOOL mCommBTreeLeafIterator_hasNext(mCommBTreeLeafIterator* self)
{
    if (self && self->tree && self->current) {
        if (self->current->next 
                || _get_left_leaf(_get_next_parent(self->current->parent))) 
            return TRUE;
    }
    return FALSE;
}

static mCommBTreeNode* mCommBTreeLeafIterator_next(mCommBTreeLeafIterator* self)
{
    if (self && self->tree && self->current) {
        if (self->current->next) 
            self->current = self->current->next;
        else 
            self->current = _get_left_leaf(_get_next_parent(self->current->parent));
        return self->current;
    }

    return NULL;
}

BEGIN_MINI_CLASS(mCommBTreeLeafIterator, mObject)
	CLASS_METHOD_MAP(mCommBTreeLeafIterator, construct);
	CLASS_METHOD_MAP(mCommBTreeLeafIterator, hasNext);
	CLASS_METHOD_MAP(mCommBTreeLeafIterator, next);
	CLASS_METHOD_MAP(mCommBTreeLeafIterator, remove);
	CLASS_METHOD_MAP(mCommBTreeLeafIterator, insert);
	CLASS_METHOD_MAP(mCommBTreeLeafIterator, getCurrent);
END_MINI_CLASS

///////////////////////////////////////////////////////////////////////////////
static void mCommBTreeNode_construct(mCommBTreeNode* self, va_list va)
{
    _SUPER(mObject, self, construct, 0);
    self->parent        = NULL;
    self->next          = NULL;
    self->children      = NULL;

    self->level         = 0;
    self->numChildren   = 0;
    self->numLeaves     = 0;
}

static void mCommBTreeNode_destroy(mCommBTreeNode *self)
{
    mCommBTreeNode *child;

    while (self->children) {
        child = self->children;
        self->children = child->next;
        DELETE(child);
    }

    _SUPER(mObject, self, destroy);
}

static void mCommBTreeNode_resetKey(mCommBTreeNode *self)
{
}

static int mCommBTreeNode_compareNode(mCommBTreeNode *self, void *cmpInfo)
{
    return BTREE_ERRNO_SNULL;
}

static void mCommBTreeNode_recompute(mCommBTreeNode *self, mCommBTreeNode *child)
{
    if (child->parent != self)
        child->parent = self;
}

static void mCommBTreeNode_changeKey(mCommBTreeNode *self, void *diffInfo, int deltaChild)
{
    if (!self || !INSTANCEOF(self, mCommBTreeNode))
        return;

    if (deltaChild) {
        if (self->level == 0) {
            if (self->parent) self->parent->numChildren += deltaChild;
        }
        else {
            self->numLeaves += deltaChild;
        }
    }

    if (self->parent) 
		_c(self->parent)->changeKey(self->parent, diffInfo, deltaChild);
}

BEGIN_MINI_CLASS(mCommBTreeNode, mObject)
	CLASS_METHOD_MAP(mCommBTreeNode, construct);
	CLASS_METHOD_MAP(mCommBTreeNode, destroy);
	CLASS_METHOD_MAP(mCommBTreeNode, compareNode);
	CLASS_METHOD_MAP(mCommBTreeNode, recompute);
	CLASS_METHOD_MAP(mCommBTreeNode, resetKey);
	CLASS_METHOD_MAP(mCommBTreeNode, changeKey);
END_MINI_CLASS

#endif /* defined(_MGCTRL_TEXTEDIT_USE_NEW_IMPL) || defined(__MGNCS_TEXTEDITOR) */
