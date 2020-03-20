#include "fastcgi.h"


int magi_fastcgi_head(magi_session *s, magi_request *r)
{
    (void)s;
    (void)r;
    return 1;
}


int magi_fastcgi_body(magi_session *s, magi_request *r)
{
    (void)s;
    (void)r;
    return 1;
}


int magi_fastcgi(magi_session *s, magi_request *r)
{
    return magi_fastcgi_head(s, r) && magi_fastcgi_body(s, r);
}
