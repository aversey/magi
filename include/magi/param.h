#ifndef MAGI_INCLUDED_PARAM
#define MAGI_INCLUDED_PARAM
/** @file param.h
 * @brief Parameter as name-value pair.
 */


/** @brief Parameter as name-value pair. */
struct magi_param {
    char *name;  /**<@brief Cannot be null. */
    char *data;  /**<@brief Cannot be null. */
};
typedef struct magi_param magi_param;

/** @brief Parameters collection.
 *
 * Implemented as linked list. */
struct magi_params {
    struct magi_params *next;  /**<@brief Pointer to next parameter. */
    magi_param          item;  /**<@brief Top parameter. */
};
typedef struct magi_params magi_params;


/** @brief Add @p newitem to @p params.
 * @param[in,out] params to add into.
 * @param[in] newitem to add onto top of @p params. */
void magi_params_add(magi_params **params, magi_param *newitem);

/** @brief Get data of parameter from @p params with @p name.
 * @param[in] params to search in.
 * @param[in] name of needed parameter.
 * @return data of the first from top of @p params parameter with @p name,
 *         null only if no parameter with @p name is in @p params. */
char *magi_params_get(magi_param_list *params, const char *name);

/* Freeing and invalidation of list. */
void magi_params_free(magi_params *params);


#endif
