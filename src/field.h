#ifndef MAGI_INCLUDED_FIELD
#define MAGI_INCLUDED_FIELD


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Form Field
 */
struct magi_field {
    char *                   name;   /* name: free(name) is valid. */
    char *                   data;   /* data: free(data) is valid. */
    int                      len;    /* Length of data. */
    struct magi_param_list * params; /* Only used if field is for file. */
};

/* Null is valid "struct magi_field_list *" object. */
struct magi_field_list {
    struct magi_field_list * next;
    struct magi_field        item;
};

/*
 * Adds *item to the begining of *list, item and list are dereferencable;
 * Returns null in case of error.
 */
int magi_field_list_add(
    struct magi_field_list ** list, struct magi_field * item);

/*
 * Searchs for first node in list: node.name == name, name is C-string;
 * Returns node itself if succeed, otherwise result is null.
 */
struct magi_field * magi_field_list_get(
    struct magi_field_list * list, const char * name);

/*
 * Destroys list; list is not valid after destruction.
 */
void magi_field_list_destroy(struct magi_field_list * list);


#endif
