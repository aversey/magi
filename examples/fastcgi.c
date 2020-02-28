#include <magi.h>


void response(magi_request *r)
{
    magi_response_add(r,
        "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
            "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
        "<html xmlns='http://www.w3.org/1999/xhtml'>"
        "<head><title>Fast CGI</title></head>"
        "<body>Hi!</body>"
        "</html>");
}

int main(int argc, char const *argv[])
{
    magi_session session;
    magi_request request;
    magi_request_init(&request);
    magi_session_init(&session);
    magi_session_inet(&session, "localhost", "9973");
    magi_session_unix(&session, "unix.sock");
    while (magi_fastcgi(&session, &request)) {
        if (request.error) {
            magi_response_error(&request);
        } else {
            response(&request);
        }
        magi_request_free(&request);
    }
    magi_request_free(&request);
    magi_session_free(&session);
    return 0;
}
