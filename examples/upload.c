#include <cgi.h>
#include <multipart.h>
#include <request.h>
#include <response.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void response_request(struct magi_request * req, struct magi_response * res)
{
    char *             name = magi_param_list_get(req->params, "name");
    struct magi_file * data = magi_file_list_get(req->files, "data");
    if (name && data) {
        rename("data", name);
    }

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
}

int main(int argc, char const * argv[])
{
    struct magi_request   request;
    struct magi_tempfiles tmps = { 0, 0, 0, 0 };
    magi_request_setup(&request);
    magi_tempfiles_add(&tmps, "data", "data", 0);
    magi_request_setup_tempfiles(&request, &tmps);
    if (magi_request_cgi(&request) && magi_request_resume_cgi(&request)) {
        struct magi_response response;
        magi_response_setup(&response);
        response_request(&request, &response);
        magi_response_cgi(&response);
        magi_response_destroy(&response);
    } else {
        magi_error_cgi(request.error);
    }
    magi_request_destroy(&request);
    return 0;
}
