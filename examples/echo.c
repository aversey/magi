#include <cgi.h>
#include <cookie.h>
#include <param.h>
#include <request.h>
#include <stdio.h>
#include <stdlib.h>


void print_preamble()
{
    puts("Content-type: application/xhtml+xml\r\n\r\n"
         "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
         "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
         "<html xmlns='http://www.w3.org/1999/xhtml'>"
         "<head><title>Echo</title></head>"
         "<body>");
}

void proceed_cookies(struct magi_cookie_list * cookies)
{
    puts("<h2>Cookies:</h2>");
    while (cookies) {
        puts("Cookie with name [");
        puts(cookies->item.name);
        if (cookies->item.data) {
            puts("] is [");
            puts(cookies->item.data);
        }
        if (cookies->item.domain) {
            puts("] for domain [");
            puts(cookies->item.domain);
        }
        if (cookies->item.port) {
            puts("] for port [");
            puts(cookies->item.port);
        }
        if (cookies->item.path) {
            puts("] for path [");
            puts(cookies->item.path);
        }
        puts("]<br/>");
        cookies = cookies->next;
    }
    puts("<hr/>");
}

void proceed_fields(struct magi_field_list * fields)
{
    puts("<h2>Feilds:</h2>");
    while (fields) {
        puts("Feild [");
        puts(fields->item.name);
        puts("] = [");
        puts(fields->item.data);
        puts("]<br/>");
        fields = fields->next;
    }
    puts("<hr/>");
}

void proceed_params(struct magi_param_list * params)
{
    puts("<h2>HTTP Parameters:</h2>");
    while (params) {
        puts("Param [");
        puts(params->item.name);
        puts("] is [");
        puts(params->item.data);
        puts("]<br/>");
        params = params->next;
    }
    puts("<hr/>");
}

void handle_request()
{
    struct magi_request request;
    if (magi_cgi(&request, 0, 0)) {
        puts("<h1>Echo CGI Script</h1>");
        puts("I was called with method [");
        puts(request.method);
        if (request.uri) {
            puts("] with URL [");
            puts(request.uri);
        }
        if (request.server_name) {
            puts("] for server [");
            puts(request.server_name);
        }
        if (request.server_port) {
            puts("] on port [");
            puts(request.server_port);
        }
        if (request.server_protocol) {
            puts("] with protocol [");
            puts(request.server_protocol);
        }
        if (request.server_software) {
            puts("] and I am running on software [");
            puts(request.server_software);
        }
        puts("]<br/>");
        proceed_cookies(request.cookies);
        proceed_fields(request.fields);
        proceed_params(request.http_params);
        magi_request_destroy(&request);
    }
}

void print_footer()
{
    puts("</body></html>");
}

int main(int argc, char const * argv[])
{
    print_preamble();
    handle_request();
    print_footer();
    return 0;
}
