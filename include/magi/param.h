#ifndef MAGI_INCLUDED_PARAM
#define MAGI_INCLUDED_PARAM
/** @file param.h
 * @brief Parameter as name-value pair.
 */


/** Parameter as name-value pair. */
typedef struct magi_param {
    char *name;  /**< Cannot be null. */
    char *data;  /**< Cannot be null. */
} magi_param;

/** Parameters collection.
 * Implemented as a linked list. */
typedef struct magi_params {
    struct magi_params *next;  /**< Pointer to next parameters. */
    magi_param          item;  /**< Parameter on top. */
} magi_params;


/** Add @p newitem to @p params.
 * @param[in,out] params to add into.
 * @param[in] newitem to add onto top of @p params. */
void magi_params_add(magi_params **params, magi_param *newitem);

/** Get data of parameter from @p params with @p name.
 * @param[in] params to search in.
 * @param[in] name of needed parameter.
 * @return data of the first from top of @p params parameter with @p name,
 *         null only if no such parameter. */
char *magi_params_get(magi_params *params, const char *name);

/** Free memory used by @p params.
 * @param[in,out] params to be destructed. */
void magi_params_free(magi_params *params);


#endif
