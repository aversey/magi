#include "error.h"

#include <stdio.h>


const char *magi_error_message(enum magi_error error)
{
    static const char *const messages[] = {
        0,                             /* magi_error_none */
        "No boundary for multipart.",  /* magi_error_nobound */
        "Content-Type is unknown.",    /* magi_error_unknown */
        "Content-Type is not given.",  /* magi_error_notype */
        "Mismatch of content length.", /* magi_error_length */
        "Cannot read cookies.",        /* magi_error_cookies */
        "Cannot decode URL.",          /* magi_error_urlenc */
        "Cannot read multipart body.", /* magi_error_multipart */
        "Part of request was too big." /* magi_error_limit */
    };
    return messages[error];
}

void magi_error_response(enum magi_error error)
{
    fputs("Status: 400 Bad Request\r\n"
          "Content-Type: text/html\r\n\r\n"
          "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
          "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
          "<html xmlns='http://www.w3.org/1999/xhtml'>"
          "<head><title>400 Bad Request</title></head>"
          "<body>"
          "<h1>400 <i>Bad Request</i></h1>"
          "<h2>", stdout);
    fputs(magi_error_message(error), stdout);
    fputs("</h2></body></html>", stdout);
}
