#include <magi.h>
#include <stdio.h>


void response(magi_request *r)
{
    char      *name = magi_request_param(r, "name");
    magi_file *data = magi_request_file(r, "data");
    magi_response_add(r,
        "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
        "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
        "<html xmlns='http://www.w3.org/1999/xhtml'>"
        "<head><title>Upload File</title></head>"
        "<body>");
    if (name && data) {
        rename("data", name);
        magi_response_add(r, "<p>Uploaded!</p>");
    }
    magi_response_add(r,
        "<form action='/cgi-bin/upload' method='post' "
        "enctype='multipart/form-data'><fieldset>"
        "<input type='text' name='name' value='filename'/>"
        "<input type='file' name='data'/>"
        "<input type='submit' value='Upload'/>"
        "</fieldset></form>"
        "</body>"
        "</html>");
}

void get(magi_request *r)
{
    magi_loadfiles rules;
    magi_loadfiles_init(&rules);
    magi_loadfiles_add(&rules, "data", "data", 0);
    magi_loadfiles_set(&r, &rules);
    magi_cgi(&r);
    magi_loadfiles_free(&rules);
}

int main(int argc, char const *argv[])
{
    magi_request request;
    magi_request_init(request);
    get(&request);
    if (request.error) {
        magi_response_error(&request);
    } else {
        response(&request);
    }
    magi_request_free(&request);
    return 0;
}
