#include <cgi.h>
#include <multipart.h>
#include <request.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void tempfile_callback(struct magi_field * field, char * buffer, int len)
{
    static FILE * file = 0;
    if (!strcmp(field->name, "data")) {
        if (!file) {
            remove(field->name);
            file = fopen(field->name, "wb");
        }
        if (len) {
            fwrite(buffer, 1, len, file);
        }
        if (len < magi_parse_multipart_callback_size) {
            fclose(file);
            file = 0;
        }
    } else if (len) {
        field->data = realloc(field->data, field->len + len + 1);
        memcpy(field->data + field->len, buffer, len);
        field->len += len;
        field->data[field->len] = 0;
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
    if (magi_cgi(&request, tempfile_callback, 0)) {
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
