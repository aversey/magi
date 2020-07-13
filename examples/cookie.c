/* Basic example of cookie handling through response headers.
 * It lists all recieved cookies and set one via magi_response,
 * showing work with headers in magi.
 */
#include <magi.h>
#include <stdio.h>


void list_cookies(struct magi_request *r)
{
    struct magi_cookies *current;
    printf("Cookies:");
    /* Iterate through all cookies in request to show them in body: */
    for (current = r->cookies; current; current = current->next) {
        struct magi_cookie *c = &current->item;
        printf("<br />[%s]=[%s]", c->name, c->data);
    }
}

void response(struct magi_request *r)
{
    struct magi_response head;
    magi_response_init(&head);  /* Setting defaults. */
    /* Set cookie "cookie" with value "monster" on clientside: */
    magi_response_cookie(&head, "cookie", "monster");
    magi_response_send(&head);  /* Send headers, */
    magi_response_free(&head);  /* free them and start sending body: */
    printf("<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
           "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
           "<html xmlns='http://www.w3.org/1999/xhtml'>"
           "<head><title>Cookie Listing and Setting</title></head>"
           "<body><p>");
    list_cookies(r);
    printf("</p></body></html>");
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
