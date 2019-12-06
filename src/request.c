#include "request.h"

#include "cookie.h"
#include "file.h"
#include "param.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void magi_request_setup(struct magi_request * request)
{
    if (request) {
        request->file_callback           = 0;
        request->file_callback_userdata  = 0;
        request->file_callback_addon_max = 64;
        request->cookies_max             = 1024 * 8;
        request->url_params_max          = 1024 * 2;
        request->http_params_max         = 1024 * 16;
        request->params_max              = 0;
    }
}


static void request_free(struct magi_request * request)
{
    free(request->cookies);
    free(request->http_params);
    free(request->url_params);
    free(request->params);
    free(request->files);
    free(request->method);
    free(request->uri);
    free(request->document_root);
    free(request->document_uri);
    free(request->script_name);
    free(request->script_filename);
    free(request->remote_addr);
    free(request->remote_port);
    free(request->server_addr);
    free(request->server_name);
    free(request->server_port);
    free(request->server_protocol);
    free(request->server_software);
    free(request->path_info);
}

static void request_annul(struct magi_request * request)
{
    request->cookies         = 0;
    request->http_params     = 0;
    request->url_params      = 0;
    request->params          = 0;
    request->files           = 0;
    request->method          = 0;
    request->uri             = 0;
    request->document_root   = 0;
    request->document_uri    = 0;
    request->script_name     = 0;
    request->script_filename = 0;
    request->remote_addr     = 0;
    request->remote_port     = 0;
    request->server_addr     = 0;
    request->server_name     = 0;
    request->server_port     = 0;
    request->server_protocol = 0;
    request->server_software = 0;
    request->path_info       = 0;
}

void magi_request_destroy(struct magi_request * request)
{
    if (request) {
        magi_cookie_list_destroy(request->cookies);
        magi_param_list_destroy(request->http_params);
        magi_param_list_destroy(request->url_params);
        magi_param_list_destroy(request->params);
        magi_file_list_destroy(request->files);
        request_free(request);
        request_annul(request);
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Tempfiles Callback
 */
void magi_tempfiles_add(struct magi_tempfiles * tmps,
                        const char *            name,
                        const char *            path,
                        int                     max)
{
    if (!tmps) {
        return;
    }
    if (tmps->count++) {
        tmps->tmps = realloc(tmps->tmps, sizeof(*tmps->tmps) * tmps->count);
    } else {
        tmps->tmps = malloc(sizeof(*tmps->tmps));
    }
    tmps->tmps[tmps->count - 1].param_name = name;
    tmps->tmps[tmps->count - 1].location   = path;
    tmps->tmps[tmps->count - 1].maximum    = max;
}

void magi_tempfiles_destroy(struct magi_tempfiles * tmps)
{
    if (!tmps) {
        return;
    }
    free(tmps->tmps);
}

static void tempfiles(struct magi_file * file,
                      char *             addon,
                      int                addon_len,
                      int                is_addon_last,
                      void *             userdata)
{
    struct magi_tempfiles * table = userdata;
    int                     pos;
    for (pos = 0; pos != table->count; ++pos) {
        if (!strcmp(table->tmps[pos].param_name, file->param_name)) {
            static FILE * f = 0;
            static int    unlimited;
            static int    left;
            if (!f) {
                const char * loc = table->tmps[pos].location;
                f                = fopen(loc, "wb");
                left             = table->tmps[pos].maximum;
                unlimited        = !left;
            }
            if (unlimited) {
                fwrite(addon, 1, addon_len, f);
            } else {
                int min = left < addon_len ? left : addon_len;
                fwrite(addon, 1, min, f);
                left -= min;
            }
            if (is_addon_last) {
                fclose(f);
                f = 0;
            }
            return;
        }
    }
}

void magi_request_setup_tempfiles(struct magi_request *   request,
                                  struct magi_tempfiles * table)
{
    request->file_callback          = tempfiles;
    request->file_callback_userdata = table;
}
