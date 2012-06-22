#ifndef _CINCLUDE_DS_BTREE_H_
#define _CINCLUDE_DS_BTREE_H_

#include "common.h"

EXTERN_C_BEGIN

struct btree_root_t {
    struct btree_t *root;
    int (*cleaner)(struct btree_t *);
};

struct btree_t {
    struct btree_t *left;
    struct btree_t *right;
};

#ifndef container_of
#define container_of(type,ptr,member) ({ const typeof( ((type *)0)->member) *__mptr=(ptr); (type *) ( (char *)__mptr - offsetof(type,member));})
#endif

#ifndef tree_downcast
#define tree_downcast(p,l) ({ p = container_of(typeof(*p),l,tree_entry);})
#endif

static inline bool 
btree_is_leaf(struct btree_t *e)
{
    return (e && (!e->left && !e->right));
}

/* map function on tree, left to right, depth first, parent to children */
static inline bool
btree_map(struct btree_t *e, bool (*func)(struct btree_t *, void *v), void *v)
{
    if(!e) return true;

    ASSERT(func(e,v), return false);

    /* inline twice beforehand */
    if(e->left) {
        ASSERT(func(e->left,v), return false);
        ASSERT(btree_map(e->left->left,func,v), return false);
        ASSERT(btree_map(e->left->right,func,v), return false);
    } 

    if(e->right) {
        ASSERT(func(e->right,v), return false);
        ASSERT(btree_map(e->right->left,func,v), return false);
        ASSERT(btree_map(e->right->right,func,v), return false);
    }

    return true;
}

/* map function on tree, left to right, depth first, child to parent */
static inline bool 
btree_bottomup_map(struct btree_t *e, int (*func)(struct btree_t *, void *v), void *v)
{
    if(!e) return true;

    /* inline twice beforehand */
    if(e->left) {
        ASSERT(btree_map(e->left->left,func,v), return false);
        ASSERT(btree_map(e->left->right,func,v), return false);
        ASSERT(func(e->left,v), return false);
    } 

    if(e->right) {
        ASSERT(btree_map(e->right->left,func,v), return false);
        ASSERT(btree_map(e->right->right,func,v), return false);
        ASSERT(func(e->right,v), return false);
    }
    ASSERT(func(e,v), return false);

    return true;
}


static inline struct btree_t *
btree_select_leaf(struct btree_t *e, 
        struct btree_t *(*branch)(struct btree_t *, void *),
        void *v)
{
    DASSERT(branch, return NULL);

    if(!e) return NULL;
    if(!e->left && !e->right) return e;

    /* unroll 8times */
    e = branch(e,v);
    if(!e) return NULL;
    if(!e->left && !e->right) return e;

    e = branch(e,v);
    if(!e) return NULL;
    if(!e->left && !e->right) return e;

    e = branch(e,v);
    if(!e) return NULL;
    if(!e->left && !e->right) return e;

    e = branch(e,v);
    if(!e) return NULL;
    if(!e->left && !e->right) return e;

    e = branch(e,v);
    if(!e) return NULL;
    if(!e->left && !e->right) return e;

    e = branch(e,v);
    if(!e) return NULL;
    if(!e->left && !e->right) return e;

    e = branch(e,v);
    if(!e) return NULL;
    if(!e->left && !e->right) return e;

    e = branch(e,v);
    if(!e) return NULL;
    if(!e->left && !e->right) return e;

    return btree_select_leaf(branch(e,v), branch, v);
}

static inline bool
__btree_destroy_iter(struct btree_t *e, int (*cleaner)(struct btree_t *))
{
    if(!e) return true;

    struct btree_t *left = e->left;
    struct btree_t *right = e->right;

    e->left = NULL;
    e->right = NULL;

    if(cleaner) {
        ASSERT(cleaner(e), return false);
    }

    ASSERT(__btree_destroy_iter(left, cleaner), return false);
    ASSERT(__btree_destroy_iter(right, cleaner), return false);

    return true;
}

static inline bool 
btree_destroy(struct btree_root_t *root)
{
    DASSERT(root, return false);

    ASSERT(__btree_destroy_iter(root->root, root->cleaner), return false);

    root->root = NULL;

    return true;
}

static inline bool 
btree_add_left(struct btree_t *parent, struct btree_t *child)
{
    DASSERT(parent, return false);

    parent->left = child;

    return true;
}

static inline bool 
btree_add_right(struct btree_t *parent, struct btree_t *child)
{
    DASSERT(parent, return false);

    parent->right = child;

    return true;
}

static inline bool 
__btree_count_iter(struct btree_t *e, void *v)
{
    int *p_i = v;
    *p_i += 1;
    return true;
}

static inline int
btree_count(struct btree_t *root)
{
    if(!root) return 0;

    int i = 0;

    btree_map(root,__btree_count_iter,&i);

    return i;
}

static inline bool 
btree_depth_tc(struct btree_t *e, int c)
{
    if(!e) return c;

    return max(btree_depth_tc(e->left, c + 1), 
            btree_depth_tc(e->right, c + 1));
}

static inline bool 
btree_depth(struct btree_t *e)
{
    if(!e) return 0;

    return btree_depth_tc(e, 0);
}

static inline int 
__tmap_count_leaf(struct btree_t *e, void *v)
{
    int *p_cnt = v;

    if(btree_is_leaf(e)) {
        *p_cnt += 1;
    }

    return true;
}

static inline bool 
btree_count_leaf(struct btree_t *e)
{
    if(!e) return 0;
    int ret = 0;

    ASSERT(btree_map(e,__tmap_count_leaf, &ret),
            return -1);

    return ret;
}

EXTERN_C_END

#endif
