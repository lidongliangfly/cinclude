#ifndef _CINCLUDE_DS_LIST_H_
#define _CINCLUDE_DS_LIST_H_

#include <stddef.h>
#include "common.h"

#ifdef __cplusplus
EXTERN_C_BEGIN
#endif

struct list_t {
    struct list_t *next;
};

typedef bool (*list_map_1_f)(struct list_t *);
typedef bool (*list_map_2_f)(struct list_t *, void *);

struct list_head_t {
    struct list_t *list;
    struct list_t *tail;
    list_map_1_f cleaner;
};


/* obtain a base pointer from an embedded field pointer */
#ifndef container_of
#define container_of(type,ptr,member) ({ const typeof( ((type *)0)->member) *__mptr=(ptr); (type *) ( (char *)__mptr - offsetof(type,member));})
#endif

/* casual static type cast using container_of macro */
#define list_downcast(p,l) ({ p = container_of(typeof(*p),l,list_entry);})

/* O(n):pseudo-syntax for list iteration:
 * var must be a temporal pointer of a subtype of list_t which has list_entry on its filed.
 * var is set to NULL at the end of iteration. */
#define list_foreach(head,var)  \
    for((head)->list ? list_downcast(var,((head)->list)) : ((var) = NULL ); \
	(var); \
	(var)->list_entry.next ? list_downcast(var,((var)->list_entry.next)) : ((var) = NULL ))

/* O(n):query length of the list by recursive tail call. 
   (potential performance penalty on -O0 compiler flag) */
static inline bool 
list_length(struct list_head_t *head)
{
    DASSERT(head, return -1);

    struct list_t *p = head->list;
    int cnt = 0;

    while(p != NULL) {
        p = p->next;
        ++cnt;
    }

    return cnt;
}

/* O(n):perform callback function 'func' with parameter 'param' iteratively. */
static inline bool 
list_map2(struct list_head_t *head, list_map_2_f func, void *param)
{
    struct list_t *p;
    p = head->list;

    while(p) {
        ASSERT(func(p,param), return false);
        p = p->next;
    }

    return true;
}

static inline bool 
list_map1(struct list_head_t *head, list_map_1_f func)
{
    struct list_t *p;
    p = head->list;

    while(p) {
        ASSERT(func(p), return false);
        p = p->next;
    }

    return true;
}

/* O(n):remove a given elememt from a list. 
 * if deallocator is set, then call it. otherwise do nothing */ 
static inline bool 
list_remove(struct list_head_t *head, struct list_t *item)
{
    DASSERT(head, return false);

    if (item == NULL) return false;

    struct list_t *prev = NULL;

    struct list_t *list = head->list;

    while(list!=NULL) {

        if(item == list){
            if(prev==NULL) {
                head->list = item->next;
            } else {
                prev->next = item->next;
            }
            
            if(item == head->tail) {
                head->tail = prev;
            }

            if(head->cleaner != NULL) {
                ASSERT(head->cleaner(item), return false);
            }

            return true;
        }

        prev = list;
        list = list->next;
    }

    return false;
}

/* O(n):apply list_remove until destroy all elems.
   we assume all the deletion must succeed as long as an elem was given :p */
static inline void 
list_destroy(struct list_head_t *head)
{
    while(list_remove(head, head->list));
} 

/* O(n):iterate elements by 'is_dead' callback,
   successively delete the elem if 'is_dead' results non-0 */
static inline bool 
list_sweep(struct list_head_t *head, list_map_1_f is_dead)
{
    struct list_t *p, *prev = NULL;

    p = head->list;

    while(p) {
        if(is_dead(p)) {

            ASSERT(list_remove(head,p), return false);

            if(prev) {
                p = prev->next;
            } else {
                p = head->list;
            }
        } else {
            prev = p;
            p = p->next;
        }
    }

    return true;
}

/* list_remove without clean function */
static inline struct list_t *
list_extract(struct list_head_t *head,struct list_t *item)
{
    DASSERT(head,return NULL);

    if(item == NULL) return NULL;

    struct list_t * l    = head->list;
    struct list_t * prev = NULL;

    while(l!=NULL) {

        if(item == l){

            if(prev==NULL) {
                head->list = item->next;
            } else {
                prev->next = item->next;
            }

            if(item == head->tail) {
                head->tail = prev;
            }

            return item;
        }

        prev = l;
        l = l->next;
    }

    return NULL;
}

/* O(1):add elem to the tail */
static inline bool 
list_append(struct list_head_t *head, struct list_t *elem)
{
    DASSERT(head, return false);
    DASSERT(elem, return false);
    
    if(!head->list) {
        head->list = elem;
        head->tail = elem;
    } else {
        head->tail->next = elem;
        head->tail       = elem;
    }

    /* this is critical: if not set to null, endless loop will occur in mapping functions when list elements are rearranged */
    elem->next = NULL;

    return true;
}

/* O(1):add elem to the head */
static inline bool 
list_push(struct list_head_t *head, struct list_t *elem)
{
    DASSERT(head,return false);
    DASSERT(elem,return false);
    
    struct list_t *old_list = head->list;
    head->list = elem;
    elem->next = old_list;

    if(!head->tail) {
        head->tail = elem;
    }

    return true;
}

/* O(1):pop elem at the head */
static inline struct list_t *
list_pop(struct list_head_t *head)
{
    DASSERT(head,return NULL);

    struct list_t *ret = NULL;
    
    ret = head->list;

    if(ret) {
        head->list = ret->next;
        ret->next = NULL;

        if(ret == head->tail) {
            head->tail = NULL;
        }
    }

    return ret;
}

/* O(n): reverse list */
static inline bool 
list_reverse(struct list_head_t *head)
{
    DASSERT(head,return false);

    struct list_t *prev = NULL;
    struct list_t *next;
    struct list_t *e;

    next = head->list;
    head->tail = head->list;

    while(next) {
        e = next;
        next = e->next;
        e->next = prev;
        prev = e;
    }

    head->list = prev;


    return true;
}

EXTERN_C_END

#endif
