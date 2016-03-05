#ifndef __EX_RBTREE_H_
#define __EX_RBTREE_H_

#include <stdio.h>
#include <stdlib.h>

#include "rbtree.h"
#include "Context.h"
#include <string>
using std::string;

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 */
#define container_of(ptr, type, member) ({            \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)


struct data_type{
    string url;
    Context *context;
};

//新建自己的数据
struct node_type {
    struct rb_node  my_node;
    struct data_type *data_content;
};

#ifdef __cplusplus
    extern "C" {
#endif

struct rb_root rbtree_init();
struct node_type *rbtree_search(struct rb_root *root, struct data_type *data_content);
int rbtree_insert(struct rb_root *root, struct data_type *data_content);
//void rbtree_delete(struct rb_root *root,struct data_type *data_content);
void print_rbtree(struct rb_root *tree);

#ifdef __cplusplus
}
#endif


#endif // __EX_RBTREE_H_
