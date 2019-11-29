#ifndef MAGI_INCLUDED_URLENC
#define MAGI_INCLUDED_URLENC

#include "request.h"


/* Fills request->url_params via parsing encoded data. */
void magi_urlenc(struct magi_param_list ** list,
                 struct magi_request *     request,
                 const char *              encoded);

/* 'code' from urlencode must have size of at least magi_urlenc_size(plain). */
int  magi_urlenc_size(const char * plain);
void magi_urlencode(const char * plain, char * code);


#endif
