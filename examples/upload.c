/* This is basic example of handling file uploading.
 * It is using magi_loadfiles callback to load file temporarily and change its
 * name into the one specified by user.
 */
#include <magi.h>
#include <stdio.h>


void upload(struct magi_request *r)
{
    char                   *name = magi_request_param(r, "name");
    const struct magi_file *data = magi_request_file(r, "data");
    if (name && data) {  /* If file and its name are in the request: */
        rename("data", name);  /* Rename loaded file to designated name. */
        printf("<p>Uploaded!</p>");  /* And display success message. */
    }
}

void response(struct magi_request *r)
{
    magi_response_default();  /* Pass default headers and send body: */
    printf("<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
           "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
           "<html xmlns='http://www.w3.org/1999/xhtml'>"
           "<head><title>Upload File</title></head>"
           "<body>");
    upload(r);
    printf("<form action='/cgi-bin/upload' method='post' "
           "enctype='multipart/form-data'><fieldset>"
           "<input type='text' name='name' value='filename' />"
           "<input type='file' name='data' />"
           "<input type='submit' value='Upload' />"
           "</fieldset></form>"
           "</body></html>");
}

void get(struct magi_request *r)
{
    struct magi_loadfiles *cb;
    magi_loadfiles_init(&cb);
    /* Setup callback to load file from "data" field into file "data": */
    magi_loadfiles_add(&cb, "data", "data", 0);
    magi_loadfiles_set(r, &cb);  /* Setup request to use the callback. */
    magi_parse(r);
    magi_loadfiles_free(&cb);  /* Free data of callback. */
}

int main()
{
    struct magi_request request;
    magi_request_init(&request);
    get(&request);        /* Parse request. */
    if (request.error) {  /* If error occurred display error message: */
        magi_error_response(request.error);
    } else {
        response(&request);  /* Overwise response request. */
    }
    magi_request_free(&request);  /* Don't forget to free everything after. */
    return 0;
}
