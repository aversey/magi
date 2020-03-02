#include <magi.h>


void list_cookies(magi_request *r)
{
    magi_cookies *current = r->cookies;
    for (current = r->cookies; current; current = current->next) {
        magi_cookie *c = &current->item;
        magi_response(r, "Cookie with name [");
        magi_response(r, c->name);
        if (c->data) {
            magi_response(r, "] is [");
            magi_response(r, c->data);
        }
        if (c->domain) {
            magi_response(r, "] for domain [");
            magi_response(r, c->domain);
        }
        if (c->path) {
            magi_response(r, "] for path [");
            magi_response(r, c->path);
        }
        magi_response(r, "]<br/>");
    }
}

void list_params(magi_request *r, magi_params *current)
{
    for (; current; current = current->next) {
        magi_param *p = &current->item;
        magi_response_format(r, "[%s] is [%s]<br/>", p->name, p->data);
    }
}

void list_files(magi_request *r)
{
    magi_files *current;
    for (current = r->files; current; current = current->next) {
        magi_file *f = &current->item;
        magi_response_format(r, "[%s] was [%s] on clientside<br/>",
                             f->param_name, f->file_name);
    }
}

void show_meta(magi_request *r)
{
    magi_response(r, "I was called with method [");
    magi_response(r, r->method);
    if (r->uri) {
        magi_response(r, "] with URL [");
        magi_response(r, r->uri);
    }
    if (r->server_name) {
        magi_response(r, "] for server [");
        magi_response(r, r->server_name);
    }
    if (r->server_port) {
        magi_response(r, "] on port [");
        magi_response(r, r->server_port);
    }
    if (r->server_protocol) {
        magi_response(r, "] with protocol [");
        magi_response(r, r->server_protocol);
    }
    if (r->server_software) {
        magi_response(r, "] and I am running on software [");
        magi_response(r, r->server_software);
    }
    magi_response(r, "]<br/>");
}

void response(magi_request *r)
{
    magi_response(r,
        "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
        "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
        "<html xmlns='http://www.w3.org/1999/xhtml'>"
        "<head><title>Echo</title></head>"
        "<body>");

    magi_response(r, "<h1>Echo CGI Script</h1>");
    show_meta(r);

    magi_response(r, "<h2>Cookies:</h2>");
    list_cookies(r);

    magi_response(r, "<h2>Parameters:</h2>");
    list_params(r, r->params);

    magi_response(r, "<h2>URL Parameters:</h2>");
    list_params(r, r->url_params);

    magi_response(r, "<h2>HTTP Parameters:</h2>");
    list_params(r, r->http_params);

    magi_response(r, "<h2>Files:</h2>");
    list_files(r);

    magi_response(r, "</body></html>");
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
