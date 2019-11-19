#include <cgi.h>
#include <cookie.h>
#include <request.h>
#include <response.h>
#include <stdio.h>
#include <stdlib.h>


void response_request(struct magi_request * req, struct magi_response * res)
{
    struct magi_cookie_list * cookie;

    magi_response_content_type(res, magi_xhtml);
    magi_response_content(
        res, "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
             "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
             "<html xmlns='http://www.w3.org/1999/xhtml'>"
             "<head><title>Cookie Listing and Setting</title></head>"
             "<body>");

    for (cookie = req->cookies; cookie; cookie = cookie->next) {
        printf("[%s] = [%s]<br/>", cookie->item.name, cookie->item.data);
    }

    magi_response_content(res, "</body></html>");

    magi_response_cookie_build(res, "cookie", "monstre", 0, 0, 0);
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
