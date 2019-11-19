#include <cgi.h>
#include <multipart.h>
#include <request.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void tempfile_callback(struct magi_file * file,
                       char *             buffer,
                       int                len,
                       void *             _)
{
    if (!strcmp(file->param_name, "data")) {
        static FILE * f = 0;
        if (!f) {
            remove(file->param_name);
            f = fopen(file->param_name, "wb");
        }
        if (len) {
            fwrite(buffer, 1, len, f);
        }
        if (len < magi_file_callback_portion_max) {
            fclose(f);
            f = 0;
        }
    }
}

void response_request(struct magi_request * req, struct magi_response * res)
{
    magi_response_content_type(res, magi_xhtml);
    magi_response_content(
        res, "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
             "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
             "<html xmlns='http://www.w3.org/1999/xhtml'>"
             "<head><title>Upload File</title></head>"
             "<body>"
             "<form action='/cgi-bin/upload' method='post' "
             "enctype='multipart/form-data'><fieldset>"
             "<input type='text' name='name' value='filename'/>"
             "<input type='file' name='data'/>"
             "<input type='submit' value='Upload'/>"
             "</fieldset></form>"
             "</body>"
             "</html>");

    struct magi_field * name = magi_field_list_get(req->fields, "name");
    struct magi_field * data = magi_field_list_get(req->fields, "data");
    if (name && name->data && data) {
        rename("data", name->data);
    }
}

int main(int argc, char const * argv[])
{
    struct magi_request request;
    if (magi_cgi(&request, tempfile_callback, 0, 0)) {
        struct magi_response response;
        response_request(&request, &response);
        magi_cgi_response(&response);
        magi_response_destroy();
    } else {
        magi_cgi_error(request.error);
    }
    magi_request_destroy(&request);
    return 0;
}
