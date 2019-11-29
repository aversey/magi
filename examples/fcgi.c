#include <error.h>
#include <fastcgi.h>
#include <request.h>
#include <response.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void response_request(struct magi_request * req, struct magi_resopnse * res)
{
    magi_response_content_type(res, magi_xhtml);
    magi_response_add(
        res, "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
             "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
             "<html xmlns='http://www.w3.org/1999/xhtml'>"
             "<head><title>Fast CGI</title></head>"
             "<body>Hi!</body>"
             "</html>");
}

int main(int argc, char const * argv[])
{
    struct magi_session session;
    int                 sock = magi_socket_inet("localhost", 9973);
    /* E.g. also magi_socket_file("fcgi.sock") can be used. */
    if (magi_session_fcgi(&session, sock)) {
        struct magi_request request;
        magi_request_setup(&request);
        while (magi_request_fcgi(&request, &session)) {
            if (!request.error) {
                struct magi_response response;
                magi_response_setup(&response);
                response_request(&request, &response);
                magi_response_fcgi(&response, &session);
                magi_response_destroy(&response);
            } else {
                magi_error_fcgi(request.error, &session);
            }
            magi_request_destroy(&request);
        }
    }
    fputs(magi_error_message(session.error), stderr);
    magi_session_destroy(&session);
    magi_socket_close(sock);
    return 0;
}
