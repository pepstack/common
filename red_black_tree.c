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
 * @filename   red_black_tree.c
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
 * @author     Liang Zhang <350137278@qq.com>
 * @version    0.0.14
 * @create     2012-05-21 12:00:00
 * @update     2019-12-03 17:08:10
 */

#include "red_black_tree.h"

/*!
 * Operations on red_black_node_t struct
 */

/*! Construct of a red-black tree node
*  param object The object stored in the node
*  param color The color of the node
*/
red_black_node_t * rbnode_construct(void * object, red_black_color_enum color)
{
    red_black_node_t * node = (red_black_node_t *) malloc(sizeof(red_black_node_t));
    if (! node) {
        printf("(%s:%d) FATAL: out of memory.\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    node->object = object;
    node->color = color;
    node->parent = node->right = node->left = 0;
    return node;
}

/*! Recursive destructor for the entire sub-tree */
void rbnode_destruct(red_black_node_t * node)
{
    if (node) {
        rbnode_destruct(node->right);
        rbnode_destruct(node->left);
        free(node);
    }
}

/*! Calculate the depth of the sub-tree spanned by the given node
*  param node The sub-tree root
*  return The sub-tree depth
*/
NOWARNING_UNUSED(static) int rbnode_depth(red_black_node_t * node)
{
    /* Recursively calculate the depth of the left and right sub-trees */
    int  iRightDepth = (node->right) ? rbnode_depth(node->right) : 0;
    int  iLeftDepth = (node->left) ? rbnode_depth(node->left) : 0;

    /* Return the maximal child depth + 1 (the current node) */
    return ((iRightDepth > iLeftDepth) ? (iRightDepth + 1) : (iLeftDepth + 1));
}

/*! Get the leftmost node in the sub-tree spanned by the given node
*  param node The sub-tree root
*  return The sub-tree minimum
*/
NOWARNING_UNUSED(static) red_black_node_t * rbnode_minimum(red_black_node_t * node)
{
    while (node->left) {
        node = node->left;
    }
    return node;
}

/*! Get the rightmost node in the sub-tree spanned by the given node
*  param node The sub-tree root
*  return The sub-tree maximum
*/
NOWARNING_UNUSED(static) red_black_node_t * rbnode_maximum(red_black_node_t * node)
{
    while (node->right) {
        node = node->right;
    }
    return node;
}

/* Replace the object at node */
NOWARNING_UNUSED(static) void rbnode_replace(red_black_node_t * node, void * object)
{
    /* Make sure the replacement does not violate the tree order */
    node->object = object;
}

/*! Get the next node in the tree (according to the tree order)
*  param node The current node
*  return The successor node, or 0 if node is the tree maximum
*/
NOWARNING_UNUSED(static) red_black_node_t * rbnode_successor(red_black_node_t * node)
{
    red_black_node_t * succ_node = 0;

    if (node) {
        if (node->right) {
            /* If there is a right child, the successor is the minimal object in
             * the sub-tree spanned by this child.
             */
            succ_node = node->right;
            while (succ_node->left) {
                succ_node = succ_node->left;
            }
        }
        else {
            /* Otherwise, go up the tree until reaching the parent from the left
             * direction.
             */
            red_black_node_t * prev_node = node;
            succ_node = node->parent;
            while (succ_node && prev_node == succ_node->right) {
                prev_node = succ_node;
                succ_node = succ_node->parent;
            }
        }
    }

    return (succ_node);
}

/*! Get the previous node in the tree (according to the tree order)
*  param node The current node
*  return The predecessor node, or 0 if node is the tree minimum
*/
NOWARNING_UNUSED(static) red_black_node_t * rbnode_predecessor(red_black_node_t * node)
{
    red_black_node_t * pred_node = 0;

    if (node) {
        if (node->left) {
            /* If there is a left child, the predecessor is the maximal object in
             * the sub-tree spanned by this child.
             */
            pred_node = node->left;
            while (pred_node->right) {
                pred_node = pred_node->right;
            }
        } else {
            /* Otherwise, go up the tree until reaching the parent from the right
             * direction.
             */
            red_black_node_t * prev_node = node;
            pred_node = node->parent;
            while (pred_node && prev_node == pred_node->left) {
                prev_node = pred_node;
                pred_node = pred_node->parent;
            }
        }
    }

    return (pred_node);
}

/*! Duplicate the entire sub-tree rooted at the given node
*  param node The sub-tree root
*  return A pointer to the duplicated sub-tree root
*/
NOWARNING_UNUSED(static) red_black_node_t * rbnode_duplicate(red_black_node_t * node)
{
    /* Create a node of the same color, containing the same object */
    red_black_node_t * dup_node = rbnode_construct(node->object, node->color);

    /* Duplicate the children recursively */
    if (node->right) {
        dup_node->right = rbnode_duplicate (node->right);
        dup_node->right->parent = dup_node;
    } else {
        dup_node->right = 0;
    }

    if (node->left) {
        dup_node->left = rbnode_duplicate(node->left);
        dup_node->left->parent = dup_node;
    } else {
        dup_node->left = 0;
    }

    return dup_node;                      /* Return the duplicated node */
}

/*! Traverse a red-black sub-tree left first
*  param node The sub-tree root
*  param op The operation to perform on each object in the sub-tree
*/
NOWARNING_UNUSED(static) void rbnode_traverse(red_black_node_t * node, void(*opfunc)(void *, void *), void* param)
{
    if (node) {
        rbnode_traverse(node->left, opfunc, param);
        opfunc(node->object, param);
        rbnode_traverse(node->right, opfunc, param);
    }
}

/*! Left-rotate the sub-tree spanned by the given node
*  param tree The tree
*  param node The sub-tree root
*/
/* Left-rotate the sub-tree spanned by the given node:
*
*          |          RoateRight(y)            |
*          y         -------------->           x
*        /   \                               /   \       .
*       x     T3       RoatateLeft(x)       T1    y      .
*     /   \          <--------------            /   \    .
*    T1    T2                                  T2    T3
*/
NOWARNING_UNUSED(static) void rbtree_rotate_left(red_black_tree_t * tree, red_black_node_t * x_node)
{
    /* Get the right child of the node */
    red_black_node_t * y_node = x_node->right;

    /* Change its left subtree (T2) to x's right subtree */
    x_node->right = y_node->left;

    /* Link T2 to its new parent x */
    if (y_node->left != 0) {
        y_node->left->parent = x_node;
    }

    /* Assign x's parent to be y's parent */
    y_node->parent = x_node->parent;

    if (!(x_node->parent)) {
        /* Make y the new tree root */
        tree->root = y_node;
    }
    else {
        /* Assign a pointer to y from x's parent */
        if (x_node == x_node->parent->left) {
            x_node->parent->left = y_node;
        }
        else {
            x_node->parent->right = y_node;
        }
    }

    /* Assign x to be y's left child */
    y_node->left = x_node;
    x_node->parent = y_node;
}


/*! Right-rotate the sub-tree spanned by the given node
*  param tree The tree
*  param node The sub-tree root
*/
NOWARNING_UNUSED(static) void rbtree_rotate_right(red_black_tree_t * tree, red_black_node_t * y_node)
{
    /* Get the left child of the node */
    red_black_node_t * x_node = y_node->left;

    /* Change its right subtree (T2) to y's left subtree */
    y_node->left = x_node->right;

    /* Link T2 to its new parent y */
    if (x_node->right != 0) {
        x_node->right->parent = y_node;
    }

    /* Assign y's parent to be x's parent */
    x_node->parent = y_node->parent;

    if (!(y_node->parent)) {
        /* Make x the new tree root */
        tree->root = x_node;
    }
    else {
        /* Assign a pointer to x from y's parent */
        if (y_node == y_node->parent->left) {
            y_node->parent->left = x_node;
        }
        else {
            y_node->parent->right = x_node;
        }
    }

    /* Assign y to be x's right child */
    x_node->right = y_node;
    y_node->parent = x_node;
}

/* Right-first traverse a red-black subtree */
NOWARNING_UNUSED(static) void rbnode_traverse_right(red_black_node_t * node, void(*opfunc)(void *, void *), void* param)
{
    if (node) {
        rbnode_traverse_right(node->right, opfunc, param);
        opfunc(node->object, param);
        rbnode_traverse_right(node->left, opfunc, param);
    }
}

/* Fix-up the tree so it maintains the red-black properties after insertion */
NOWARNING_UNUSED(static) void rbtree_insert_fixup(red_black_tree_t * tree, red_black_node_t * node)
{
    /* Fix the red-black propreties: we may have inserted a red leaf as the
    * child of a red parent - so we have to fix the coloring of the parent
    * recursively.
    */
    red_black_node_t * curr_node = node;
    red_black_node_t * grandparent;
    red_black_node_t *uncle;

    /* use -DNDEBUG to close assert() */
    assert(node && node->color == rbclrRed);

    while (curr_node != tree->root && curr_node->parent->color == rbclrRed) {
        /* Get a pointer to the current node's grandparent (notice the root is
        * always black, so the red parent must have a parent).
        */
        grandparent = curr_node->parent->parent;

        if (curr_node->parent == grandparent->left) {
            /* If the red parent is a left child, the uncle is the right child of
            * the grandparent.
            */
            uncle = grandparent->right;

            if (uncle && uncle->color == rbclrRed) {
                /* If both parent and uncle are red, color them black and color the
                * grandparent red.
                * In case of a 0 uncle, we treat it as a black node.
                */
                curr_node->parent->color = rbclrBlack;
                uncle->color = rbclrBlack;
                grandparent->color = rbclrRed;

                /* Move to the grandparent */
                curr_node = grandparent;
            }
            else {
                /* Make sure the current node is a right child. If not, left-rotate
                * the parent's sub-tree so the parent becomes the right child of the
                * current node (see _rotate_left).
                */
                if (curr_node == curr_node->parent->right) {
                    curr_node = curr_node->parent;
                    rbtree_rotate_left(tree, curr_node);
                }

                /* Color the parent black and the grandparent red */
                curr_node->parent->color = rbclrBlack;
                grandparent->color = rbclrRed;

                /* Right-rotate the grandparent's sub-tree */
                rbtree_rotate_right(tree, grandparent);
            }
        }
        else {
            /* If the red parent is a right child, the uncle is the left child of
            * the grandparent.
            */
            uncle = grandparent->left;

            if (uncle && uncle->color == rbclrRed) {
                /* If both parent and uncle are red, color them black and color the
                * grandparent red.
                * In case of a 0 uncle, we treat it as a black node.
                */
                curr_node->parent->color = rbclrBlack;
                uncle->color = rbclrBlack;
                grandparent->color = rbclrRed;

                /* Move to the grandparent */
                curr_node = grandparent;
            }
            else {
                /* Make sure the current node is a left child. If not, right-rotate
                * the parent's sub-tree so the parent becomes the left child of the
                * current node.
                */
                if (curr_node == curr_node->parent->left) {
                    curr_node = curr_node->parent;
                    rbtree_rotate_right(tree, curr_node);
                }

                /* Color the parent black and the grandparent red */
                curr_node->parent->color = rbclrBlack;
                grandparent->color = rbclrRed;

                /* Left-rotate the grandparent's sub-tree */
                rbtree_rotate_left(tree, grandparent);
            }
        }
    }

    /* Make sure that the root is black */
    tree->root->color = rbclrBlack;
}

NOWARNING_UNUSED(static) void rbtree_remove_fixup(red_black_tree_t * tree, red_black_node_t * node)
{
    red_black_node_t * curr_node = node;
    red_black_node_t * sibling;

    while (curr_node != tree->root && curr_node->color == rbclrBlack) {
        /* Get a pointer to the current node's sibling (notice that the node's
        * parent must exist, since the node is not the root).
        */
        if (curr_node == curr_node->parent->left) {
            /* If the current node is a left child, its sibling is the right
            * child of the parent.
            */
            sibling = curr_node->parent->right;

            /* Check the sibling's color. Notice that 0 nodes are treated
            * as if they are colored black.
            */
            if (sibling && sibling->color == rbclrRed) {
                /* In case the sibling is red, color it black and rotate.
                * Then color the parent red (and the grandparent is now black).
                */
                sibling->color = rbclrBlack;
                curr_node->parent->color = rbclrRed;
                rbtree_rotate_left(tree, curr_node->parent);
                sibling = curr_node->parent->right;
            }

            if (sibling &&
                (!(sibling->left) || sibling->left->color == rbclrBlack) &&
                (!(sibling->right) || sibling->right->color == rbclrBlack)) {
                /* If the sibling has two black children, color it red */
                sibling->color = rbclrRed;
                if (curr_node->parent->color == rbclrRed) {
                    /* If the parent is red, we can safely color it black and terminate
                    * the fix-up process.
                    */
                    curr_node->parent->color = rbclrBlack;
                    curr_node = tree->root;      /* In order to stop the while loop */
                }
                else {
                    /* The black depth of the entire sub-tree rooted at the parent is
                    * now too small - fix it up recursively.
                    */
                    curr_node = curr_node->parent;
                }
            }
            else {
                if (!sibling) {
                    /* Take special care of the case of a 0 sibling */
                    if (curr_node->parent->color == rbclrRed) {
                        curr_node->parent->color = rbclrBlack;
                        curr_node = tree->root;    /* In order to stop the while loop */
                    }
                    else {
                        curr_node = curr_node->parent;
                    }
                }
                else {
                    /* In this case, at least one of the sibling's children is red.
                    * It is therfore obvious that the sibling itself is black.
                    */
                    if (sibling->right && sibling->right->color == rbclrRed) {
                        /* If the right child of the sibling is red, color it black and
                        * rotate around the current parent.
                        */
                        sibling->right->color = rbclrBlack;
                        rbtree_rotate_left(tree, curr_node->parent);
                    }
                    else {
                        /* If the left child of the sibling is red, rotate around the
                        * sibling, then rotate around the new sibling of our current
                        * node.
                        */
                        rbtree_rotate_right(tree, sibling);
                        sibling = curr_node->parent->right;
                        rbtree_rotate_left(tree, sibling);
                    }

                    /* It is now safe to color the parent black and to terminate the
                    * fix-up process.
                    */
                    if (curr_node->parent->parent) {
                        curr_node->parent->parent->color = curr_node->parent->color;
                    }
                    curr_node->parent->color = rbclrBlack;
                    curr_node = tree->root;      /* In order to stop the while loop */
                }
            }
        }
        else {
            /* If the current node is a right child, its sibling is the left
            * child of the parent.
            */
            sibling = curr_node->parent->left;

            /* Check the sibling's color. Notice that 0 nodes are treated
            * as if they are colored black.
            */
            if (sibling && sibling->color == rbclrRed) {
                /* In case the sibling is red, color it black and rotate.
                * Then color the parent red (and the grandparent is now black).
                */
                sibling->color = rbclrBlack;
                curr_node->parent->color = rbclrRed;
                rbtree_rotate_right(tree, curr_node->parent);

                sibling = curr_node->parent->left;
            }

            if (sibling &&
                (!(sibling->left) || sibling->left->color == rbclrBlack) &&
                (!(sibling->right) || sibling->right->color == rbclrBlack)) {
                /* If the sibling has two black children, color it red */
                sibling->color = rbclrRed;
                if (curr_node->parent->color == rbclrRed) {
                    /* If the parent is red, we can safely color it black and terminate
                    * the fix-up process.
                    */
                    curr_node->parent->color = rbclrBlack;
                    curr_node = tree->root;      /* In order to stop the while loop */
                }
                else {
                    /* The black depth of the entire sub-tree rooted at the parent is
                    * now too small - fix it up recursively.
                    */
                    curr_node = curr_node->parent;
                }
            }
            else {
                if (!sibling) {
                    /* Take special care of the case of a 0 sibling */
                    if (curr_node->parent->color == rbclrRed) {
                        curr_node->parent->color = rbclrBlack;
                        curr_node = tree->root;    /* In order to stop the while loop */
                    }
                    else {
                        curr_node = curr_node->parent;
                    }
                }
                else {
                    /* In this case, at least one of the sibling's children is red.
                    * It is therfore obvious that the sibling itself is black.
                    */
                    if (sibling->left && sibling->left->color == rbclrRed) {
                        /* If the left child of the sibling is red, color it black and
                        * rotate around the current parent
                        */
                        sibling->left->color = rbclrBlack;
                        rbtree_rotate_right(tree, curr_node->parent);
                    }
                    else {
                        /* If the right child of the sibling is red, rotate around the
                        * sibling, then rotate around the new sibling of our current
                        * node
                        */
                        rbtree_rotate_left(tree, sibling);
                        sibling = curr_node->parent->left;
                        rbtree_rotate_right(tree, sibling);
                    }

                    /* It is now safe to color the parent black and to terminate the
                    * fix-up process.
                    */
                    if (curr_node->parent->parent) {
                        curr_node->parent->parent->color = curr_node->parent->color;
                    }
                    curr_node->parent->color = rbclrBlack;
                    curr_node = tree->root;       /* In order to stop the while loop */
                }
            }
        }
    }

    /* The root can always be colored black */
    curr_node->color = rbclrBlack;
}


/**********************************************************************
 *               public api for red_black_tree_t                      *
 *                                                                    *
 *********************************************************************/

/* Intialize a tree */
void rbtree_init(red_black_tree_t * tree, int(*keycmp)(void *, void *))
{
    tree->keycmp = keycmp;
    tree->size = 0;
    tree->root = 0;
}

/* Construct a tree given a comparison function */
red_black_tree_t * rbtree_construct(int(*keycmp)(void *, void *))
{
    red_black_tree_t * tree = (red_black_tree_t *) malloc(sizeof(red_black_tree_t));
    if (!tree) {
        printf("(%s:%d) FATAL: out of memory.\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    rbtree_init(tree, keycmp);
    return tree;
}

/* Remove all objects from a black-red tree */
void rbtree_clean(red_black_tree_t * tree)
{
    if (tree) {
        rbnode_destruct(tree->root);
        tree->root = 0;
        tree->size = 0;
    }
}

/* Destruct a red-black tree */
void rbtree_destruct(red_black_tree_t * tree)
{
    rbtree_clean(tree);
    free(tree);
}

/* Returns the size of the tree */
size_t rbtree_size(red_black_tree_t * tree)
{
    return tree->size;
}

/* Returns the depth of the tree */
int rbtree_depth(red_black_tree_t * tree)
{
    int depth = 0;
    if (tree->root) {
        depth = rbnode_depth(tree->root);
    }
    return depth;
}

/* Insert an object to the tree */
red_black_node_t * rbtree_insert(red_black_tree_t * tree, void * object)
{
    red_black_node_t * cur_node;
    red_black_node_t * new_node = NULL;

    if (!(tree->root)) {
        /* Assign a new root node. Notice that the root is always black */
        new_node = rbnode_construct(object, rbclrBlack);
        tree->root = new_node;
        tree->size = 1;
        return new_node;
    }

    /* Find a place for the new object, and insert it as a red leaf */
    cur_node = tree->root;

    while (cur_node) {
        /* Compare inserted object with the object stored in the current node */
        if (tree->keycmp(object, cur_node->object) > 0) {
            if (!(cur_node->left)) {
                /* Insert the new leaf as the left child of the current node */
                new_node = rbnode_construct(object, rbclrRed);
                cur_node->left = new_node;
                new_node->parent = cur_node;
                cur_node = NULL;                /* terminate the while loop */
            } else {
                cur_node = cur_node->left;      /* Go to the left sub-tree */
            }
        } else {
            if (!(cur_node->right)) {
                /* Insert the new leaf as the right child of the current node */
                new_node = rbnode_construct(object, rbclrRed);
                cur_node->right = new_node;
                new_node->parent = cur_node;
                cur_node = NULL;                /* terminate the while loop */
            } else {
                cur_node = cur_node->right;     /* Go to the right sub-tree */
            }
        }
    }

    /* Mark that a new node was added */
    tree->size++;

    /* Fix up the tree properties */
    rbtree_insert_fixup(tree, new_node);
    return new_node;
}


/* Insert an unique object to the tree */
red_black_node_t * rbtree_insert_unique(red_black_tree_t * tree, void * object, int *is_new_node)
{
    int cmp;
    red_black_node_t * cur_node;
    red_black_node_t * new_node = NULL;

    *is_new_node = 1;

    if (!(tree->root)) {
        /* Assign a new root node. Notice that the root is always black */
        new_node = rbnode_construct(object, rbclrBlack);
        tree->root = new_node;
        tree->size = 1;
        return new_node;
    }

    /* Find a place for the new object, and insert it as a red leaf */
    cur_node = tree->root;

    while (cur_node) {
        cmp = tree->keycmp(object, cur_node->object);
        if (cmp == 0) {
            /* there already has an object with the same id as object to be inserted */
            *is_new_node = 0;
            return cur_node;
        }

        /* Compare inserted object with the object stored in the current node */
        if (cmp > 0) {
            if (!(cur_node->left)) {
                /* Insert the new leaf as the left child of the current node */
                new_node = rbnode_construct(object, rbclrRed);
                cur_node->left = new_node;
                new_node->parent = cur_node;
                cur_node = NULL;                /* terminate the while loop */
            } else {
                cur_node = cur_node->left;      /* Go to the left sub-tree */
            }
        } else {
            if (!(cur_node->right)) {
                /* Insert the new leaf as the right child of the current node */
                new_node = rbnode_construct(object, rbclrRed);
                cur_node->right = new_node;
                new_node->parent = cur_node;
                cur_node = NULL;                /* terminate the while loop */
            } else {
                cur_node = cur_node->right;     /* Go to the right sub-tree */
            }
        }
    }

    /* Mark that a new node was added */
    tree->size++;

    /* Fix up the tree properties */
    rbtree_insert_fixup(tree, new_node);
    return new_node;
}

/* Insert a new object to the tree as the a successor of a given node */
red_black_node_t * rbtree_insert_succ(red_black_tree_t * tree, red_black_node_t * at_node, void * object)
{
    red_black_node_t * parent;
    red_black_node_t * new_node;

    if (!(tree->root)) {
        /* Assign a new root node. Notice that the root is always black */
        new_node = rbnode_construct(object, rbclrBlack);
        tree->root = new_node;
        tree->size = 1;
        return new_node;
    }

    /* Insert the new object as a red leaf, being the successor of node */
    new_node = rbnode_construct(object, rbclrRed);

    if (!at_node) {
        /* The new node should become the tree minimum: Place is as the left
         * child of the current minimal leaf.
         */
        parent = rbnode_minimum(tree->root);
        parent->left = new_node;
    } else {
        /* Make sure the insertion does not violate the tree order */

        /* In case given node has no right child, place the new node as its
         * right child. Otherwise, place it at the leftmost position at the
         * sub-tree rooted at its right side.
         */
        if (!at_node->right) {
            parent = at_node;
            parent->right = new_node;
        } else {
            parent = rbnode_minimum(at_node->right);
            parent->left = new_node;
        }
    }

    new_node->parent = parent;

    /* Mark that a new node was added */
    tree->size++;

    /* Fix up the tree properties */
    rbtree_insert_fixup(tree, new_node);
    return new_node;
}

/* Insert a new object to the tree as the a predecessor of a given node */
red_black_node_t * rbtree_insert_pred(red_black_tree_t * tree, red_black_node_t * at_node, void * object)
{
    red_black_node_t * parent;
    red_black_node_t * new_node;

    if (!(tree->root)) {
        /* Assign a new root node. Notice that the root is always black */
        new_node = rbnode_construct(object, rbclrBlack);
        tree->root = new_node;
        tree->size = 1;
        return new_node;
    }

    /* Insert the new object as a red leaf, being the predecessor of at_node */
    new_node = rbnode_construct(object, rbclrRed);

    if (!at_node) {
        /* The new node should become the tree maximum: Place is as the right
         * child of the current maximal leaf.
         */
        parent = rbnode_maximum(tree->root);
        parent->right = new_node;
    } else {
        /* Make sure the insertion does not violate the tree order */

        /* In case given node has no left child, place the new node as its
         * left child. Otherwise, place it at the rightmost position at the
         * sub-tree rooted at its left side.
         */
        if (!(at_node->left)) {
            parent = at_node;
            parent->left = new_node;
        } else {
            parent = rbnode_maximum (at_node->left);
            parent->right = new_node;
        }
    }

    new_node->parent = parent;

    /* Mark that a new node was added */
    tree->size++;

    /* Fix up the tree properties */
    rbtree_insert_fixup(tree, new_node);

    return new_node;
}

/* Remove an object from the tree */
void * rbtree_remove(red_black_tree_t * tree, void * object)
{
    red_black_node_t * node;

    /* find the node */
    node = rbtree_find(tree, object);
    if (node) {
        void * objectToFree = node->object;

        /* remove the node */
        rbtree_remove_at(tree, node);

        return objectToFree;
    }

    return NULL;
}

/* Remove the object pointed by the given node. */
void rbtree_remove_at(red_black_tree_t * tree, red_black_node_t * node)
{
    red_black_node_t * child = NULL;

    /* In case of deleting the single object stored in the tree, free the root,
     * thus emptying the tree.
     */
    if (tree->size == 1) {
        rbnode_destruct(tree->root);
        tree->root = 0;
        tree->size = 0;
        return;
    }

    /* Remove the given node from the tree */
    if (node->left && node->right) {
        /* If the node we want to remove has two children, find its successor,
         * which is the leftmost child in its right sub-tree and has at most
         * one child (it may have a right child).
         */
        red_black_node_t * succ_node = rbnode_minimum(node->right);

        /* Now physically swap node and its successor. Notice this may temporarily
         * violate the tree properties, but we are going to remove node anyway.
         * This way we have moved node to a position were it is more convinient
         * to delete it.
         */
        int immediate_succ = (node->right == succ_node);
        red_black_node_t * succ_parent = succ_node->parent;
        red_black_node_t * succ_left = succ_node->left;
        red_black_node_t * succ_right = succ_node->right;
        red_black_color_enum succ_color = succ_node->color;

        succ_node->parent = node->parent;
        succ_node->left = node->left;
        succ_node->right = immediate_succ ? node : node->right;
        succ_node->color = node->color;

        node->parent = immediate_succ ? succ_node : succ_parent;
        node->left = succ_left;
        node->right = succ_right;
        node->color = succ_color;

        if (!immediate_succ) {
            if (succ_node == node->parent->left) {
                node->parent->left = node;
            }
            else {
                node->parent->right = node;
            }
        }

        if (node->left) {
            node->left->parent = node;
        }
        if (node->right) {
            node->right->parent = node;
        }

        if (succ_node->parent) {
            if (node == succ_node->parent->left) {
	            succ_node->parent->left = succ_node;
            }
            else {
	            succ_node->parent->right = succ_node;
            }
        } else {
            tree->root = succ_node;
        }

        if (succ_node->left) {
            succ_node->left->parent = succ_node;
        }

        if (succ_node->right) {
            succ_node->right->parent = succ_node;
        }
    }

    /* At this stage, the node we are going to remove has at most one child */
    child = (node->left) ? node->left : node->right;

    /* Splice out the node to be removed, by linking its parent straight to the
     * removed node's single child.
     */
    if (child) {
        child->parent = node->parent;
    }

    if (!(node->parent)) {
        /* If we are deleting the root, make the child the new tree node */
        tree->root = child;
    } else {
        /* Link the removed node parent to its child */
        if (node == node->parent->left) {
            node->parent->left = child;
        } else {
            node->parent->right = child;
        }
    }

    /* Fix-up the red-black properties that may have been damaged: If we have
     * just removed a black node, the black-depth property is no longer valid.
     */
    if (node->color == rbclrBlack && child) {
        rbtree_remove_fixup(tree, child);
    }

    /* Delete the un-necessary node (we 0 ify both its children because the
     * node's destructor is recursive).
     */
    node->left = 0;
    node->right = 0;
    free(node);

    /* Descrease the number of objects in the tree */
    tree->size--;
}

/* Get the tree minimum */
red_black_node_t * rbtree_minimum(red_black_tree_t * tree)
{
    red_black_node_t * node = NULL;
    if (tree->root) {
        /* Return the leftmost leaf in the tree */
        node = rbnode_minimum(tree->root);
    }
    return node;
}

/* Get the tree maximum */
red_black_node_t * rbtree_maximum(red_black_tree_t * tree)
{
    red_black_node_t * node = NULL;
    if (tree->root) {
        /* Return the rightmost leaf in the tree */
        node = rbnode_maximum(tree->root);
    }
    return node;
}

/* Return a pointer to the node containing the given object */
red_black_node_t * rbtree_find(red_black_tree_t * tree, void * object)
{
    int comp_result;
    red_black_node_t * cur_node;

    cur_node = tree->root;
    while (cur_node) {
        /* In case of equality, we can return the current node. */
        if ((comp_result = tree->keycmp(object, cur_node->object)) == 0) {
            return cur_node;
        }

        /* Go down to the left or right child. */
        cur_node = (comp_result > 0) ? cur_node->left : cur_node->right;
    }

    /* If we reached here, the object is not found in the tree */
    return NULL;
}

/* Traverse a red-black tree */
void rbtree_traverse(red_black_tree_t * tree, void(*opfunc)(void *, void *), void *param)
{
    rbnode_traverse(tree->root, opfunc, param);
}

/* Right-first traverse a red-black tree */
void rbtree_traverse_right(red_black_tree_t * tree, void(*opfunc)(void *, void *), void *param)
{
    rbnode_traverse_right(tree->root, opfunc, param);
}
