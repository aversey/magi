#ifndef MAGI_INCLUDED_URLENC
#define MAGI_INCLUDED_URLENC
/** @file urlenc.h
 * @brief Realisation of url-encoding.
 *
 * Can be helpful in forming urls in response.
 */


/* 'code' from urlenc must have size of at least magi_urlenc_size(plain). */
int  magi_urlenc_size(const char *plain);
void magi_urlenc(const char *plain, char *code);


#endif
