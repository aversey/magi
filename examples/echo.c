#include <cgi.h>
#include <cookie.h>
#include <param.h>
#include <request.h>
#include <response.h>
#include <stdio.h>
#include <stdlib.h>


void proceed_cookies(struct magi_cookie_list * cookies,
                     struct magi_response *    response)
{
    magi_response_add(response, "<h2>Cookies:</h2>");
    while (cookies) {
        magi_response_add(response, "Cookie with name [");
        magi_response_add(response, cookies->item.name);
        if (cookies->item.data) {
            magi_response_add(response, "] is [");
            magi_response_add(response, cookies->item.data);
        }
        if (cookies->item.domain) {
            magi_response_add(response, "] for domain [");
            magi_response_add(response, cookies->item.domain);
        }
        if (cookies->item.port) {
            magi_response_add(response, "] for port [");
            magi_response_add(response, cookies->item.port);
        }
        if (cookies->item.path) {
            magi_response_add(response, "] for path [");
            magi_response_add(response, cookies->item.path);
        }
        magi_response_add(response, "]<br/>");
        cookies = cookies->next;
    }
    magi_response_add(response, "<hr/>");
}

void proceed_params(struct magi_param_list * params,
                    struct magi_response *   response)
{
    while (params) {
        magi_response_add_format(response, "[%s] is [%s]<br/>",
                                 params->item.name, params->item.data);
        params = params->next;
    }
    magi_response_add(response, "<hr/>");
}

void proceed_files(struct magi_file_list * files,
                   struct magi_response *  response)
{
    while (files) {
        magi_response_add_format(response, "[%s] was [%s] on userside<br/>",
                                 files->item.param_name,
                                 files->item.file_name);
        files = files->next;
    }
    magi_response_add(response, "<hr/>");
}

void process_meta(struct magi_request * req, struct magi_response * res)
{
    magi_response_add(res,
                      "<h1>Echo CGI Script</h1>I was called with method [");
    magi_response_add(res, req->method);
    if (req->uri) {
        magi_response_add(res, "] with URL [");
        magi_response_add(res, req->uri);
    }
    if (req->server_name) {
        magi_response_add(res, "] for server [");
        magi_response_add(res, req->server_name);
    }
    if (req->server_port) {
        magi_response_add(res, "] on port [");
        magi_response_add(res, req->server_port);
    }
    if (req->server_protocol) {
        magi_response_add(res, "] with protocol [");
        magi_response_add(res, req->server_protocol);
    }
    if (req->server_software) {
        magi_response_add(res, "] and I am running on software [");
        magi_response_add(res, req->server_software);
    }
    magi_response_add(res, "]<br/>");
}

void response_request(struct magi_request * req, struct magi_response * res)
{
    magi_response_content_type(res, magi_xhtml);
    magi_response_add(
        res, "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
             "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
             "<html xmlns='http://www.w3.org/1999/xhtml'>"
             "<head><title>Echo</title></head>"
             "<body>");
    process_meta(req, res);
    proceed_cookies(req->cookies, res);
    magi_response_add(res, "<h2>Parameters:</h2>");
    proceed_params(req->params, res);
    magi_response_add(res, "<h2>URL Parameters:</h2>");
    proceed_params(req->url_params, res);
    magi_response_add(res, "<h2>HTTP Parameters:</h2>");
    proceed_params(req->http_params, res);
    magi_response_add(res, "<h2>Files:</h2>");
    proceed_files(req->files, res);
    magi_response_add(res, "</body></html>");
}

int main(int argc, char const * argv[])
{
    struct magi_request request;
    magi_request_setup(&request);
    if (magi_request_cgi(&request)) {
        struct magi_response response;
        magi_response_setup(&response);
        response_request(&request, &response);
        magi_response_cgi(&response);
        magi_response_destroy(&response);
    } else {
        magi_error_cgi(request.error);
    }
    magi_request_destroy(&request);
    return 0;
}
