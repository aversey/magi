#ifndef MAGI_INCLUDED_PARAM
#define MAGI_INCLUDED_PARAM
/* Parameter as name-value pair.
 */


struct magi_param {
    char *name;  /* Cannot be null. */
    char *data;  /* Cannot be null. */
};

/* Parameters collection, implemented as a linked list. */
struct magi_params {
    struct magi_params *next;  /* Pointer to next parameters. */
    struct magi_param   item;  /* Parameter on top. */
};


/* Free memory used by params. */
void magi_params_free(struct magi_params *params);

/* Add newitem onto top of params. */
void magi_params_add(struct magi_params     **params,
                     const struct magi_param *newitem);

/* Set newitem in params.
 * If param with name of newitem is in params
 * it will be replaced with newitem,
 * otherwise newitem will be added into the end of params. */
void magi_params_set(struct magi_params     **params,
                     const struct magi_param *newitem);

/* Get data of the first from top of params parameter with name,
 * null if no such parameter. */
char *magi_params_get(const struct magi_params *params,
                      const char               *name);


#endif
