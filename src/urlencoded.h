#ifndef MAGI_INCLUDED_URLENCODED
#define MAGI_INCLUDED_URLENCODED

#include "error.h"
#include "param.h"


/* Add decoded params from 'encoded' to 'list'. */
enum magi_error magi_parse_urlencoded(struct magi_params **list,
                                      const char          *encoded);


#endif
