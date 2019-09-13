#ifndef MAGI_INCLUDED_PARAM_H
#define MAGI_INCLUDED_PARAM_H


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Parameter
 */
struct magi_param {
    char *name;  /* name: free(name) is valid. */
    char *data;  /* data: free(data) is valid. */
};

/* Null is valid "struct magi_param_list *" object. */
struct magi_param_list {
    struct magi_param_list *next;
    struct magi_param      item;
};

/*
 * Adds *item to the begining of *list, item and list are dereferencable;
 * Returns null in case of error.
 */
int magi_param_list_add(
    struct magi_param_list **list,
    struct magi_param      *item
);

/*
 * Searchs for first node in list: node.name == name, name is C-string;
 * Returns node itself if succeed, otherwise result is null.
 */
struct magi_param *magi_param_list_get(
    struct magi_param_list *list,
    const char            *name
);

/*
 * Destroys list; list is not valid after destruction.
 */
void magi_param_list_destroy(struct magi_param_list *list);


#endif
