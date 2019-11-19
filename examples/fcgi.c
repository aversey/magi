#include <error.h>
#include <fastcgi.h>
#include <request.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void response_request(struct magi_request * req, struct magi_resopnse * res)
{
    magi_response_content_type(res, magi_xhtml);
    magi_response_content(
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
    if (magi_fcgi(&session, sock)) {
        struct magi_request request;
        while (magi_fcgi_accept(&request, &session)) {
            if (!request.error) {
                struct magi_response response;
                response_request(&request, &response);
                magi_fcgi_response(&response, &session);
                magi_response_destroy(&response);
            } else {
                magi_fcgi_error(request.error, &session);
            }
            magi_request_destroy(&request);
        }
    }
    puts(magi_error_message(session.error));
    magi_session_destroy(&session);
    magi_socket_close(sock);
    return 0;
}
