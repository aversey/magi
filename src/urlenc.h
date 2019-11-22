#ifndef MAGI_INCLUDED_URLENC
#define MAGI_INCLUDED_URLENC

#include "request.h"


void magi_urlenc(struct magi_request * request, const char * encoded);

int  magi_urlenc_size(const char * plain);
void magi_urlencode(const char * plain, char * code);


#endif
