#define _GNU_SOURCE
#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS 64

#ifdef _WIN32
  #define fseeko _fseeki64
  #define ftello _ftelli64
#endif

#include <stdio.h>
#include <stdlib.h>   // Required for alloca and rand
#include "db.h"
#include "libs/mongoose.h" // Include Mongoose header file

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

    if(mg_match(hm->uri, mg_str("/api/#"), NULL)) {

        struct user *u = getuser(hm);
        if(u == NULL) {
           return mg_http_reply(c, 403, "", "Denied\n");
        }
        
        if(mg_match(hm->uri, mg_str("*/login"), NULL)) {
            return mg_http_reply(c, 200, MG_API_HEADERS, "{\"token\": %m}\n", MG_ESC(u->token));
        }
        else if (mg_match(hm->uri, mg_str("/api/tables"), NULL)) {
            //TODO Handle table operations
            char *query = mg_json_get_str(hm->body, "$query");
            printf("/api/table: %s\r\n", hm->body);
            //const char* sql = "SELECT name FROM sqlite_master WHERE type='table';";
            mg_http_reply(c, 200, MG_API_HEADERS, "{%m:%m}\n", mg_print_esc, 0, "result", "coucou");
        }

    } else {
        struct mg_http_serve_opts opts = {
            .root_dir = "/ui/dist",
            .fs = &mg_fs_packed
        };
        return mg_http_serve_dir(c, ev_data, &opts);
    }
}

// Initialize Mongoose
int init_mg(struct mg_mgr *mgr){
    struct mg_connection *nc;
    mg_mgr_init(mgr);
    nc = mg_http_listen(mgr, s_http_port, ev_handler, NULL);
    if (nc == NULL) {
        fprintf(stderr, "Failed to create listener\n");
        return 1;
    }
    return 0;
}

int create_directory_if_not_exists(const char *dir_name) {
    struct stat st = {0};
    if (stat(dir_name, &st) == -1) {
        if (mkdir(dir_name, 0755) == 0) {
            fprintf(stdout, "Directory '%s' created successfully.\n", dir_name);
        } else {
            fprintf(stderr, "mkdir error while creating directory: %s\n", dir_name);
            return 1;
        }
    }
    return 0;
}

int init_dir() {
    int ret = 0;
    // ret += create_directory_if_not_exists("cb_public");
    ret += create_directory_if_not_exists("cb_data");
    return ret;
}

int main(void) {
    struct mg_mgr mgr;

    if(init_dir() != 0) return 1;
    if(init_db() != 0) return 1;
    if(init_mg(&mgr) != 0) return 1;

    // Set up HTTP server
    fprintf(stdout, "Starting web server on %s\n", s_http_port);

    // Event loop
    while (1) {
        mg_mgr_poll(&mgr, 1000);
    }

    // Cleanup
    mg_mgr_free(&mgr);
    close_db();
    return 0;
}