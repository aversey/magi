/* This is the very basic example, showing work with form fields to append
 * addon specified by user in form into some file (i.e. "file_to_append").
 */
#include <magi.h>
#include <stdio.h>


void append(struct magi_request *r)
{
    char *data = magi_request_param(r, "addon");
    if (data) {  /* If field "addon" was in request: */
        FILE *file = fopen("file_to_append", "a");
        fputs(data, file);  /* Append addon into the file. */
        fclose(file);
        printf("<p>Appended!</p>");  /* And display success message. */
    }
}

void response(struct magi_request *r)
{
    magi_response_default();  /* Pass default headers and send body: */
    printf("<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
           "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
           "<html xmlns='http://www.w3.org/1999/xhtml'>"
           "<head><title>Append to File</title></head>"
           "<body>");
    append(r);
    printf("<form action='/cgi-bin/append' method='post'><fieldset>"
           "<input type='text' name='addon' value='Enter your addon.' />"
           "<input type='submit' value='Append' />"
           "</fieldset></form>"
           "</body></html>");
}

int main()
{
    struct magi_request request;
    magi_request_init(&request);  /* Setting defaults. */
    if (magi_parse(&request)) {   /* If parsing was done successful */
        response(&request);       /* we need to response the request. */
    } else {                      /* And display error overwise: */
        magi_error_response(request.error);
    }
    magi_request_free(&request);  /* Don't forget to free everything after. */
    return 0;
}
