#include <cgi.h>
#include <cookie.h>
#include <request.h>
#include <stdio.h>
#include <stdlib.h>


void print_preamble()
{
    puts("Set-Cookie:cookie=monstre\r\n" /* Important to set cookies before: */
         "Content-Type: application/xhtml+xml\r\n\r\n");
}

void print_webpage_top()
{
    puts("<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
         "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
         "<html xmlns='http://www.w3.org/1999/xhtml'>"
         "<head><title>Cookie Listing and Setting</title></head>"
         "<body>");
}

void read_and_print_cookies()
{
    struct magi_request request;
    if (magi_cgi(&request, 0, 0)) {
        struct magi_cookie_list * cookie;
        for (cookie = request.cookies; cookie; cookie = cookie->next) {
            printf("[%s] = [%s]<br/>", cookie->item.name, cookie->item.data);
        }
        magi_request_destroy(&request);
    }
}

void print_webpage_bottom()
{
    puts("</body>"
         "</html>");
}

int main(int argc, char const * argv[])
{
    print_preamble();
    /* Following probably will be much more pleasant with use of templates. */
    print_webpage_top();
    read_and_print_cookies();
    print_webpage_bottom();
    return 0;
}
