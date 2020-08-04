#include "urlenc.h"

#include "tools.h"
#include <ctype.h>
#include <string.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Local Shortcuts
 */
/* Call only if 0 <= num && num <= 15. */
static char to_hex(int num)
{
    static const char *const hex = "0123456789ABCDEF";
    return hex[num];
}

static int is_url(char c)
{
    return isalnum(c) || strchr("$-_.+ !*'(),", c);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * URL Encoding
 */
int magi_urlenc_size(const char *plain)
{
    int size;
    if (!plain) {
        return 0;
    }
    for (size = 0; *plain; ++plain, ++size) {
        if (!is_url(*plain)) {
            size += 2;
        }
    }
    return size;
}

void magi_urlenc(const char *plain, char *code)
{
    if (!plain || !code) {
        return;
    }
    while (*plain) {
        if (is_url(*plain)) {
            *(code++) = *plain;
        } else {
            *(code++) = '%';
            *(code++) = to_hex(*plain & 0x0F);
            *(code++) = to_hex(*plain >> 4);
        }
        ++plain;
    }
}
