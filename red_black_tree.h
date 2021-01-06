/***********************************************************************
 * Copyright (c) 2008-2080 pepstack.com, 350137278@qq.com
 *
 * ALL RIGHTS RESERVED.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************/

/**
 * @filename   red_black_tree.h
 *  Container class for a red-black tree.
 *
 * Download From:
 *   http://www.cs.tau.ac.il/~efif/courses/Software1_Summer_03/code/rbtree/
 *
 * Container class for a red-black tree: A binary tree that satisfies the
 * following properties:
 * 1. Each node has a color, which is either red or black.
 * 2. A red node cannot have a red parent.
 * 3. The number of black nodes from every path from the tree root to a leaf
 *    is the same for all tree leaves (it is called the 'black depth' of the
 *    tree).
 * Due to propeties 2-3, the depth of a red-black tree containing n nodes
 *   is bounded by 2*log_2(n).
 *
 * The red_black_tree_t template requires two template parmeters:
 * - The contained TYPE class represents the objects stored in the tree.
 *   It has to support the copy constructor and the assignment operator
 *   (operator=).
 *
 * - fn_comp_func is a functor used to define the order of objects of
 *   class TYPE:
 * This class has to support an operator() that recieves two objects from
 *   the TYPE class and returns a negative, zero or a positive integer,
 *   depending on the comparison result.
 *
 * Insert, Delete, and Get are O(log(n)).
 * Advantages
 *  Red-black trees are self-balancing so these operations are guaranteed
 *    to be O(log(n)); a simple binary search tree, on the other hand,
 *    could potentially become unbalanced, degrading to O(n) performance
 *    for Insert, Delete, and Get.
 *  Particularly useful when inserts and/or deletes are relatively frequent.
 *  Relatively low constants in a wide variety of scenarios.
 *  All the advantages of binary search trees.
 *
 * @author     Liang Zhang <350137278@qq.com>
 * @version    0.0.13
 * @create     2012-05-21 12:00:00
 * @update     2019-12-03 17:08:10
 */
#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "thread_rwlock.h"

/*! Color enumeration for nodes of red-black tree */
typedef enum _red_black_color_enum
{
    rbclrRed   = 0,
    rbclrBlack = 1
} red_black_color_enum;


/*! Representation of a node in a red-black tree */
typedef struct _red_black_node_t {
    void                     * object;       /* the stored object user defined */
    red_black_color_enum       color;        /* the color of the node */
    struct _red_black_node_t * parent;       /* points to the parent node */
    struct _red_black_node_t * right;        /* points to the right child */
    struct _red_black_node_t * left;         /* points to the left child */
} red_black_node_t, RBTreeNode_t, *RBTreeNode;


typedef int  (rbtreenode_cmp_func)(void *newObject, void *nodeObject);
typedef void (rbtreenode_oper_func)(void *object, void *param);


/*! Representation of a red-black tree */
typedef struct _red_black_tree_t {
    red_black_node_t   * root;                /* pointer to the tree root */
    size_t               size;                /* number of objects stored */
    int(*keycmp)(void *, void *);             /* keys compare callback function */

    ThreadRWLock_t       lock;                /* concurrent lock for threads */
} red_black_tree_t, RBTreeRoot_t, * RBTreeRoot;


/*! Construct of a red-black tree node
*  param object The object stored in the node
*  param color The color of the node
*/
extern red_black_node_t * rbnode_construct(void * object, red_black_color_enum color);

/*! Recursive destructor for the entire sub-tree */
extern void rbnode_destruct(red_black_node_t * node);

/*! Initialize a red-black tree with a comparision function
 *  param tree The tree
 *  param keycmp The comparision function
 */
extern void rbtree_init (red_black_tree_t * tree, int(*keycmp)(void *, void *));

/*! Construct a red-black tree with a comparison object
 *  param keycmp A pointer to the comparison object to be used by the tree
 *  return The newly constructed  tree
 */
extern red_black_tree_t * rbtree_construct (int(*keycmp)(void *, void *));

/*! Clean a red-black tree [takes O(n) operations]
 *  param tree The tree
 */
extern void rbtree_clean (red_black_tree_t * tree);

/*! Destruct a red-black tree
 *  param tree The tree
 */
extern void rbtree_destruct (red_black_tree_t * tree);

/*! Get the size of the tree [takes O(1) operations]
 *  param tree The tree
 *  return The number of objects stored in the tree
 */
extern size_t rbtree_size (red_black_tree_t * tree);

/*! Get the depth of the tree [takes O(n) operations]
 *  param tree The tree
 *  return The length of the longest path from the root to a leaf node
 */
extern int rbtree_depth (red_black_tree_t * tree);

/*! Insert an object to the tree [takes O(log n) operations]
 *  param tree The tree
 *  param object The object to be inserted
 *  return the inserted object node
 */
extern red_black_node_t * rbtree_insert (red_black_tree_t * tree, void * object);

/*! Insert an unique object to the tree */
extern red_black_node_t * rbtree_insert_unique (red_black_tree_t * tree, void * object, int *is_new_node);

/*! Insert a new object to the tree as the a successor of a given node
 *  param tree The tree
 *  return The new node
 */
extern red_black_node_t * rbtree_insert_succ (red_black_tree_t * tree, red_black_node_t * at_node, void * object);

/*! Insert a new object to the tree as the a predecessor of a given node
 *  param tree The tree
 *  return The new node
 */
extern red_black_node_t * rbtree_insert_pred (red_black_tree_t * tree, red_black_node_t * at_node, void * object);

/*! Remove an object from the tree [takes O(log n) operations]
 *  param tree The tree
 *  param object The object to be removed
 *  pre The object should be contained in the tree
 */
extern void * rbtree_remove (red_black_tree_t * tree, void * object);

/*! Remove the object stored in the given tree node
*  param tree The tree
*  param node The node storing the object to be removed from the tree
*/
extern void rbtree_remove_at (red_black_tree_t * tree, red_black_node_t * node);

/*! Get a handle to the tree minimum [takes O(log n) operations]
 *  param tree The tree
 *  return the minimal object in the tree, or a 0 if the tree is empty
 */
extern red_black_node_t * rbtree_minimum (red_black_tree_t * tree);

/*! Get a handle to the tree maximum [takes O(log n) operations]
 *  param tree The tree
 *  return the maximal object in the tree, or a 0 if the tree is empty
 */
extern red_black_node_t * rbtree_maximum (red_black_tree_t * tree);

/*! Find a node that contains the given object
 *  param tree The tree
 *  param object The desired object
 *  return A node that contains the given object, or 0 if no such object
 * is found in the tree
 */
extern red_black_node_t * rbtree_find (red_black_tree_t * tree, void * object);

/*! Traverse a red-black tree left first
 *  param tree The tree
 *  param op The operation to perform on every object of the tree (according to
 * the tree order)
 */
extern void rbtree_traverse (red_black_tree_t * tree, void(*opfunc)(void *, void *), void *param);

/*! Traverse a red-black tree right first */
extern void rbtree_traverse_right (red_black_tree_t * tree, void(*opfunc)(void *, void *), void *param);

#define rbtree_traverse_left    rbtree_traverse

#ifdef __cplusplus
}
#endif
#endif /* RED_BLACK_TREE_H */
