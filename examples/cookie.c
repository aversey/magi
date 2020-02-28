#include <magi.h>


void list_cookies(magi_request *r)
{
    magi_cookies *current;
    magi_response_add(r, "Cookies:");
    for (current = r->cookies; current; current = current->next) {
        magi_cookie *c = &current->item;
        magi_response_format(r, "<br/>[%s] = [%s]", c->name, c->data);
    }
}

void response(magi_request *r)
{
    magi_response_cookie(r, "cookie", "monster");
    magi_response_add(r,
        "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
        "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
        "<html xmlns='http://www.w3.org/1999/xhtml'>"
        "<head><title>Cookie Listing and Setting</title></head>"
        "<body><p>");
    list_cookies(r);
    magi_response_add(r, "</p></body></html>");
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
