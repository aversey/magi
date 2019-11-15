#ifndef MAGI_INCLUDED_MULTIPART
#define MAGI_INCLUDED_MULTIPART

#include "field.h"


enum { magi_parse_multipart_callback_size = 64 };


int magi_parse_multipart(
    struct magi_field_list ** list,
    int (*get_next)(void *),
    void * get_next_arg,
    char * boundary,
    /* End if size < magi_parse_multipart_callback_size. */
    /* Null callback means filling list. */
    void (*callback)(struct magi_field * field, char * buffer, int size));


#endif
