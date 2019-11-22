#include <cgi.h>
#include <multipart.h>
#include <request.h>
#include <response.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void tempfile_callback(struct magi_file * file,
                       char *             addon,
                       int                addon_len,
                       int                is_addon_last,
                       void *             unused)
{
    if (!strcmp(file->param_name, "data")) {
        static FILE * f = 0;
        if (!f) {
            remove(file->param_name);
            f = fopen(file->param_name, "wb");
        }
        if (addon_len) {
            fwrite(addon, 1, addon_len, f);
        }
        if (is_addon_last) {
            fclose(f);
            f = 0;
        }
    }
}

void response_request(struct magi_request * req, struct magi_response * res)
{
    magi_response_content_type(res, magi_xhtml);
    magi_response_add(
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

    struct magi_param * name = magi_param_list_get(req->params, "name");
    struct magi_param * data = magi_param_list_get(req->params, "data");
    if (name && name->data && data) {
        rename("data", name->data);
    }
}

int main(int argc, char const * argv[])
{
    struct magi_request request;
    magi_request_setup(&request);
    request.file_callback = tempfile_callback;
    if (magi_request_cgi(&request)) {
        struct magi_response response;
        response_request(&request, &response);
        magi_response_cgi(&response);
        magi_response_destroy();
    } else {
        magi_error_cgi(request.error);
    }
    magi_request_destroy(&request);
    return 0;
}
