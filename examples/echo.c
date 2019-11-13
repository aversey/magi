#include <cgi.h>
#include <cookie.h>
#include <param.h>
#include <request.h>
#include <stdio.h>
#include <stdlib.h>


void proceed_cookies(
    struct magi_cookie_list * cookies, struct magi_response * response)
{
    magi_response_content(response, "<h2>Cookies:</h2>");
    while (cookies) {
        magi_response_content(response, "Cookie with name [");
        magi_response_content(response, cookies->item.name);
        if (cookies->item.data) {
            magi_response_content(response, "] is [");
            magi_response_content(response, cookies->item.data);
        }
        if (cookies->item.domain) {
            magi_response_content(response, "] for domain [");
            magi_response_content(response, cookies->item.domain);
        }
        if (cookies->item.port) {
            magi_response_content(response, "] for port [");
            magi_response_content(response, cookies->item.port);
        }
        if (cookies->item.path) {
            magi_response_content(response, "] for path [");
            magi_response_content(response, cookies->item.path);
        }
        magi_response_content(response, "]<br/>");
        cookies = cookies->next;
    }
    magi_response_content(response, "<hr/>");
}

void proceed_fields(
    struct magi_field_list * fields, struct magi_response * response)
{
    magi_response_content(response, "<h2>Feilds:</h2>");
    while (fields) {
        magi_response_content(response, "Feild [");
        magi_response_content(response, fields->item.name);
        magi_response_content(response, "] = [");
        magi_response_content(response, fields->item.data);
        magi_response_content(response, "]<br/>");
        fields = fields->next;
    }
    magi_response_content(response, "<hr/>");
}

void proceed_params(
    struct magi_param_list * params, struct magi_response * response)
{
    magi_response_content(response, "<h2>HTTP Parameters:</h2>");
    while (params) {
        magi_response_content(response, "Param [");
        magi_response_content(response, params->item.name);
        magi_response_content(response, "] is [");
        magi_response_content(response, params->item.data);
        magi_response_content(response, "]<br/>");
        params = params->next;
    }
    magi_response_content(response, "<hr/>");
}

void process_meta(struct magi_request * req, struct magi_response * res)
{
    magi_response_content(res,
        "<h1>Echo CGI Script</h1>"
        "I was called with method [");
    magi_response_content(res, req->method);
    if (req->uri) {
        magi_response_content(res, "] with URL [");
        magi_response_content(res, req->uri);
    }
    if (req->server_name) {
        magi_response_content(res, "] for server [");
        magi_response_content(res, req->server_name);
    }
    if (req->server_port) {
        magi_response_content(res, "] on port [");
        magi_response_content(res, req->server_port);
    }
    if (req->server_protocol) {
        magi_response_content(res, "] with protocol [");
        magi_response_content(res, req->server_protocol);
    }
    if (req->server_software) {
        magi_response_content(res, "] and I am running on software [");
        magi_response_content(res, req->server_software);
    }
    magi_response_content(res, "]<br/>");
}

void response_request(struct magi_request * req, struct magi_response * res)
{
    magi_response_content_type(res, magi_xhtml);
    magi_response_content(res,
        "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
        "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
        "<html xmlns='http://www.w3.org/1999/xhtml'>"
        "<head><title>Echo</title></head>"
        "<body>");
    process_meta(req, res);
    proceed_cookies(req->cookies, res);
    proceed_fields(req->fields, res);
    proceed_params(req->http_params, res);
    magi_response_content(res, "</body></html>");
}

int main(int argc, char const * argv[])
{
    struct magi_request request;
    if (magi_cgi(&request, 0, 0)) {
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
