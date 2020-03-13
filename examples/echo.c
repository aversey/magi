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
        magi_response(r, "[");
        magi_response(r, p->name);
        magi_response(r, "] is [");
        magi_response(r, p->data);
        magi_response(r, "]<br/>");
    }
}

void list_files(magi_request *r)
{
    magi_files *current;
    for (current = r->files; current; current = current->next) {
        magi_file *f = &current->item;
        magi_response(r, "[");
        magi_response(r, f->field);
        magi_response(r, "] was [");
        magi_response(r, f->filename);
        magi_response(r, "] on clientside<br/>");
    }
}

void show_meta(magi_request *r)
{
    magi_response(r, "I was called ");
    if (r->is_secure) {
        magi_response(r, "securely ");
    }
    magi_response(r, "with method [");
    magi_response(r, r->method);
    if (r->host) {
        magi_response(r, "] on server [");
        magi_response(r, r->host);
    }
    if (r->script) {
        magi_response(r, "] being script on [");
        magi_response(r, r->script);
    }
    if (r->path) {
        magi_response(r, "] with requested path [");
        magi_response(r, r->path);
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
    list_params(r, r->meta);

    magi_response(r, "<h2>URL Parameters:</h2>");
    list_params(r, r->head);

    magi_response(r, "<h2>Body Parameters:</h2>");
    list_params(r, r->body);

    magi_response(r, "<h2>Files:</h2>");
    list_files(r);

    magi_response(r, "</body></html>");
}

int main()
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
