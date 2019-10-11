#include <stdio.h>
#include <stdlib.h>
#include <request.h>


void handle_request()
{
    struct magi_request request;
    if (magi_request_build_cgi(&request, 0, 0)) {
        struct magi_field *a = magi_field_list_get(request.fields, "addon");
        if (a && a->data) {
            FILE *file = fopen("file_to_append", "a");
            fputs(a->data, file);
            fclose(file);
        }
        magi_request_destroy(&request);
    }
}

void print_preamble()
{
    puts("Content-type: application/xhtml+xml\r\n\r\n");
}

void print_webpage()
{
    puts(
        "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
            "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
        "<html xmlns='http://www.w3.org/1999/xhtml'>"
        "<head><title>Append to File</title></head>"
        "<body>"
        "<form action='/cgi-bin/append' method='GET'>"
        "<input type='text' name='addon' value='Whatever you want to add.'/>"
        "<input type='submit' value='Append'/>"
        "</form>"
        "</body>"
        "</html>"
    );
}

int main(int argc, char const *argv[])
{
    handle_request();
    print_preamble();
    print_webpage();
    return 0;
}
