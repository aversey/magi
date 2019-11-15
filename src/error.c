#include "error.h"


const char * magi_error_message(enum magi_error error)
{
    static const char * const messages[] = {
        0,                             /* magi_error_none */
        "No boundary for multipart.",  /* magi_error_nobound */
        "Content-type is unknown.",    /* magi_error_unknown */
        "Content-type is not given.",  /* magi_error_notype */
        "Mismatch of content length.", /* magi_error_length */
        "Cannot allocate memmory.",    /* magi_error_memmory */
        "Cannot read cookies.",        /* magi_error_cookies */
        "Cannot decode URL.",          /* magi_error_urlenc */
        "Cannot read multipart body."  /* magi_error_multipart */
    };
    return messages[error];
}
