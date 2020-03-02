#include <magi.h>
#include <stdio.h>


void response(magi_request *r)
{
    char *data = magi_request_param(r, "addon");
    magi_response(r,
        "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
        "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
        "<html xmlns='http://www.w3.org/1999/xhtml'>"
        "<head><title>Append to File</title></head>"
        "<body>");
    if (data) {
        FILE *file = fopen("file_to_append", "a");
        fputs(data, file);
        fclose(file);
        magi_response(r, "<p>Appended!</p>");
    }
    magi_response(r,
        "<form action='/cgi-bin/append' method='post'><fieldset>"
        "<input type='text' name='addon' value='Whatever you want to add.'/>"
        "<input type='submit' value='Append'/>"
        "</fieldset></form>"
        "</body>"
        "</html>");
}

int main(int argc, char const *argv[])
{
    magi_request request;
    magi_request_init(&request);
    if (magi_cgi(&request)) {
        response(&request);
    } else {
        magi_response_error(&request);
    }
    magi_request_free(&request);
    return 0;
}
