#include "error.h"

#include "utils.h"
#include <stdarg.h>


void magi_error_add(struct magi_error ** error, char * format, ...)
{
    if (error && format) {
        struct magi_error * head = malloc(sizeof(*head));
        if (head) {
            int     size;
            va_list args;

            va_start(args, format);
            size = vsnprintf(0, 0, format, args);
            va_end(args);

            if (size >= 0) {
                head->message = malloc(++size);
                if (head->message) {
                    va_start(args, format);
                    size = vsnprintf(head->message, size, format, args);
                    va_end(args);
                    if (size >= 0) {
                        head->prev = *error;
                        *error     = head;
                        return;
                    } else {
                        free(head->message);
                    }
                }
            }

            free(head);
        }
    }
}

void magi_error_destroy(struct magi_error * error)
{
    if (error) {
        magi_error_destroy(error->prev);
        free(error->prev);
        free(error->message);
        error->prev    = 0;
        error->message = 0;
    }
}
