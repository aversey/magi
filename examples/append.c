#include <cgi.h>
#include <request.h>
#include <stdio.h>
#include <stdlib.h>


void response_request(struct magi_request * req, struct magi_response * res)
{
    magi_response_content_type(res, magi_xhtml);
    magi_response_content(
        res,
        "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
        "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
        "<html xmlns='http://www.w3.org/1999/xhtml'>"
        "<head><title>Append to File</title></head>"
        "<body>"
        "<form action='/cgi-bin/append' method='get'><fieldset>"
        "<input type='text' name='addon' value='Whatever you want to add.'/>"
        "<input type='submit' value='Append'/>"
        "</fieldset></form>"
        "</body>"
        "</html>");

    struct magi_field * a = magi_field_list_get(req->fields, "addon");
    if (a && a->data) {
        FILE * file = fopen("file_to_append", "a");
        fputs(a->data, file);
        fclose(file);
    }
}

int main(int argc, char const * argv[])
{
    struct magi_request request;
    if (magi_cgi(&request, 0, 0, 0)) {
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
