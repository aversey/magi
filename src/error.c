#include "error.h"

#include <stdarg.h>


enum control { get, set };

static struct magi_error * errorctl(
    enum control command, struct magi_error_message * err)
{
    static struct magi_error * error = 0;
    if (command == set) {
        error = err;
    }
    return error;
}


struct magi_error * magi_error_get()
{
    return errorctl(get, 0);
}

void magi_error_set(char * format, ...)
{
    for (;;) {
        Create(len);
        /* Try to print in the allocated space. */
        va_list ap;
        va_start(ap, format);
        int n = vsnprintf(p->buf, len, format, ap);
        va_end(ap);
        /* If that worked, we're done */
        if (n > -1 && n < len)
            break;
        /* Else try again with more space. */
        if (n > -1)      /* glibc 2.1 */
            len = n + 1; /* precisely what is needed */
        else             /* glibc 2.0 */
            len *= 2;    /* twice the old size */
    }
    errorctl();
}

void magi_error_rid()
{
    struct magi_error * error = errorctl(get, 0);
    if (error) {
        if (error->message) {
            free(error->message);
        }
        free(error);
        errorctl(set, 0);
    }
}
