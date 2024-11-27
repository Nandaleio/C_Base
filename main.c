#define _GNU_SOURCE
#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS 64

#ifdef _WIN32
  #define fseeko _fseeki64
  #define ftello _ftelli64
#endif

#include <stdio.h>
#include <stdlib.h>   // Required for alloca and rand
#include "modules/db.h"
#include "modules/utils.h"
#include "libs/mongoose.h"
#include "libs/log.h"
#include "modules/jwt.h"

//#define PUBLIC_DIR "cb_public"

#define MG_API_HEADERS "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: *\r\n"

// Initialize Mongoose server options
char *s_http_port = "http://localhost:8080";

// HTTP event handler
static void ev_handler(struct mg_connection *c, int ev, void *ev_data) {
    if (ev != MG_EV_HTTP_MSG) return;

    struct mg_http_message *hm = (struct mg_http_message *) ev_data;

    if (mg_strcmp(hm->method, mg_str("OPTIONS")) == 0) {
            return mg_http_reply(c, 204, MG_API_HEADERS"Access-Control-Allow-Headers: *\r\n\r\n", "{\"res\": \"No Content\"}");
    } 

    if(mg_match(hm->uri, mg_str("/api/auth/register"), NULL)) {
        char *username = mg_json_get_str(hm->body, "$.username");
        char *password = mg_json_get_str(hm->body, "$.password");
        char *result = db_add_user(username, password) ;
        return mg_http_reply(c, 200, MG_API_HEADERS, result);
    }
    
    if(mg_match(hm->uri, mg_str("/api/auth/login"), NULL)) {
        char *username = mg_json_get_str(hm->body, "$.username");
        char *password = mg_json_get_str(hm->body, "$.password");
        char *json = db_login(username, password);
        return mg_http_reply(c, 200, MG_API_HEADERS, "{\"token\": \"%s\"}\n", json);
    }

    // ----- SECURED API -----
    if(mg_match(hm->uri, mg_str("/api/#"), NULL)) {

        struct mg_str *auth_header = mg_http_get_header(hm, "Authorization");
        if (auth_header != NULL) {
            const char *token = auth_header->buf + 7;
            size_t token_len = auth_header->len - 7;

            log_debug("Bearer token: %.*s", (int) token_len, token);

            if(jwt_verify(token, JWT_SECRET_KEY) != 0) return;

        } else { 
            return;
        }

        
        if(mg_match(hm->uri, mg_str("*/query"), NULL)) {
            char *query = mg_json_get_str(hm->body, "$.query");
            char *json = db_query(query);
            mg_http_reply(c, 200, MG_API_HEADERS, json);
            free(json);
            return;
        }

        if (mg_match(hm->uri, mg_str("*/tables/*"), NULL)) {
            char **json = db_get_tables();
            mg_http_reply(c, 200, MG_API_HEADERS, json);
            free(json);
            return;
        }
        
        if (mg_match(hm->uri, mg_str("*/tables"), NULL)) {
            char **json = db_get_tables();
            mg_http_reply(c, 200, MG_API_HEADERS, json);
            free(json);
            return;
        }

        return mg_http_reply(c, 204, MG_API_HEADERS"Access-Control-Allow-Headers: *\r\n\r\n", "{\"res\": \"No Content\"}");
    } 

    // ----- UNPROTECTED API -----

    struct mg_http_serve_opts opts;
    memset(&opts, 0, sizeof(opts));

    if(mg_match(hm->uri, mg_str("/_/#"), NULL)) {
            hm->uri.buf += 2;
            hm->uri.len -= 2;
            
            opts.root_dir = "/ui/dist";
            opts.fs = &mg_fs_packed;
    } else {
            opts.root_dir = "./cb_public";
    }

    mg_http_serve_dir(c, ev_data, &opts);
    
}

// Initialize Mongoose
int init_mg(struct mg_mgr *mgr){
    struct mg_connection *nc;
    mg_mgr_init(mgr);
    nc = mg_http_listen(mgr, s_http_port, ev_handler, NULL);
    if (nc == NULL) {
        log_error("Failed to create listener");
        return 1;
    }
    return 0;
}

int create_directory_if_not_exists(const char *dir_name) {
    struct stat st = {0};
    if (stat(dir_name, &st) == -1) {
        if (mkdir(dir_name, 0755) == 0) {
            log_trace(stdout, "Directory '%s' created successfully.", dir_name);
        } else {
            log_error("mkdir error while creating directory: %s", dir_name);
            return 1;
        }
    }
    return 0;
}

int init_dir() {
    int ret = 0;
    ret += create_directory_if_not_exists("cb_public");
    ret += create_directory_if_not_exists("cb_data");
    return ret;
}

int main(void) {
    struct mg_mgr mgr;

    if(init_dir() != 0) return 1;
    if(db_init() != 0) return 1;
    if(init_mg(&mgr) != 0) return 1;

    // Set up HTTP server
    log_info("Starting web server on %s", s_http_port);

    // Event loop
    while (1) {
        mg_mgr_poll(&mgr, 1000);
    }

    // Cleanup
    mg_mgr_free(&mgr);
    db_close();
    return 0;
}