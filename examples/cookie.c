#include <magi.h>
#include <stdio.h>
#include <stdlib.h>


void response_request(magi_request *req, magi_response *res)
{
    magi_cookie_list *current;

    magi_response_add(res,
        "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
        "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
        "<html xmlns='http://www.w3.org/1999/xhtml'>"
        "<head><title>Cookie Listing and Setting</title></head>"
        "<body>");

    for (current = req->cookies; current; current = current->next) {
        magi_response_add_format(res,
            "[%s] = [%s]<br/>", current->item.name, current->item.data);
    }

    magi_response_add(res, "</body></html>");

    magi_response_cookie_easy(res, "cookie", "monster");
}

int main(int argc, char const *argv[])
{
    magi_request request;
    magi_request_setup(&request);
    if (magi_request_full_cgi(&request)) {
        magi_response response;
        magi_response_setup(&response);
        response_request(&request, &response);
        magi_response_cgi_clear(&response);
    } else {
        magi_error_cgi(request.error);
    }
    magi_request_destroy(&request);
    return 0;
}
