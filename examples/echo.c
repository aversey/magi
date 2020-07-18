/* This is useful example echoing request data in response.
 */
#include <magi.h>
#include <stdio.h>


void list_cookies(struct magi_request *r)
{
    struct magi_cookies *current = r->cookies;
    printf("<p>");
    /* Iterate through all cookies in request to show them in body: */
    for (current = r->cookies; current; current = current->next) {
        struct magi_cookie *c = &current->item;
        printf("Cookie with name [%s", c->name);
        if (c->data) {
            printf("] is [%s", c->data);
        }
        if (c->domain) {
            printf("] for domain [%s", c->domain);
        }
        if (c->path) {
            printf("] for path [%s", c->path);
        }
        printf("]<br />");
    }
    printf("</p>");
}

void list_params(struct magi_params *current)
{
    printf("<p>");
    /* Iterate through specified params to show them in body: */
    for (; current; current = current->next) {
        struct magi_param *p = &current->item;
        printf("[%s] is [%s]<br />", p->name, p->data);
    }
    printf("</p>");
}

void list_files(struct magi_request *r)
{
    struct magi_files *current;
    printf("<p>");
    /* Iterate through all field files in request to show them in body: */
    for (current = r->files; current; current = current->next) {
        struct magi_file *f = &current->item;
        printf("[%s] was [%s] on clientside<br />", f->field, f->filename);
    }
    printf("</p>");
}

void show_meta(struct magi_request *r)
{
    printf("<p>I was called ");
    if (r->is_secure) {
        printf("securely ");
    }
    printf("with method [%s", r->method);
    if (r->host) {
        printf("] on server [%s", r->host);
    }
    if (r->script) {
        printf("] being script on [%s", r->script);
    }
    if (r->path) {
        printf("] with requested path [%s", r->path);
    }
    printf("]</p>");
}

void response(struct magi_request *r)
{
    magi_response_default();  /* Pass default headers and send body: */
    printf("<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
           "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
           "<html xmlns='http://www.w3.org/1999/xhtml'>"
           "<head><title>Echo</title></head>"
           "<body>");

    printf("<h1>Echo CGI Script</h1>");
    show_meta(r);

    printf("<h2>Cookies:</h2>");
    list_cookies(r);

    printf("<h2>URL Parameters:</h2>");
    list_params(r->head);

    printf("<h2>Body Parameters:</h2>");
    list_params(r->body);

    printf("<h2>Files:</h2>");
    list_files(r);

    printf("</body></html>");
}

int main()
{
    struct magi_request request;
    magi_request_init(&request);  /* Setting defaults. */
    if (magi_parse(&request)) {   /* If parsing was done successful */
        response(&request);       /* we need to response the request. */
    } else {                      /* And display error overwise: */
        magi_error_response(request.error);
    }
    magi_request_free(&request);  /* Don't forget to free everything after. */
    return 0;
}
