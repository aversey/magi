#ifndef MAGI_INCLUDED_URLENC
#define MAGI_INCLUDED_URLENC


/* 'code' from urlenc must have size of at least magi_urlenc_size(plain). */
int  magi_urlenc_size(const char *plain);
void magi_urlenc(const char *plain, char *code);


#endif
