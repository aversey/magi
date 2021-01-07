#include <magi.h>
#include <stdio.h>

int main()
{
    struct magi_request request;
    magi_request_init(&request);
    if (magi_parse(&request)) {
        magi_response_default();
        puts("<html><head><title>Hi!</title></head><body>Hi!</body></html>");
    } else {
        magi_error_response(request.error);
    }
    magi_request_free(&request);
    return 0;
}
