// @file: include/tree.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-18
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_TREE_H_545EF3C4EF7F11E2B95774E50BEE6214
#define _INCLUDE_TREE_H_545EF3C4EF7F11E2B95774E50BEE6214
#pragma once

#include <stddef.h>

// ---- An implementation of Treap Tree ----
typedef struct _RBNode {
  bool red;               // if red = 0, it's a black node
  struct _RBNode *parent;
  struct _RBNode *left, *right;
} RBNode;

typedef struct _RBTree {
  // compare function should return -1 if *node1 < *node2, 
  // 1 if *node1 > *node2, and 0 otherwise
  int (*compare) (RBNode * node1, RBNode * node2);
  RBNode *root, *nil;
  // Dummy nodes for root & nil.
  RBNode __root, __nil;
} RBTree;

void rb_tree_init(RBTree *tree, int (*compare) (RBNode *, RBNode *));
void rb_tree_destroy(RBTree * tree);
void rb_insert(RBTree * tree, RBNode * node);
void rb_delete(RBTree * tree, RBNode * node);

void rb_node_init(RBNode* node);
RBNode *rb_search(RBTree * tree, int (*compare) (RBNode*, void*), void* key);
RBNode *rb_upbound(RBTree* tree, int (*compare) (RBNode*, void*), void* key);
RBNode *rb_lowbound(RBTree* tree, int (*compare)(RBNode*, void*), void* key);
RBNode *rb_leftmost(RBTree* tree);
RBNode *rb_rightmost(RBTree* tree);

RBNode *rb_node_prev(RBTree * tree, RBNode * node);
RBNode *rb_node_next(RBTree * tree, RBNode * node);
RBNode *rb_node_root(RBTree * tree);
RBNode *rb_node_left(RBTree * tree, RBNode * node);
RBNode *rb_node_right(RBTree * tree, RBNode * node);

#endif // _INCLUDE_TREE_H_545EF3C4EF7F11E2B95774E50BEE6214
