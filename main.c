#define _GNU_SOURCE
#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS 64

#ifdef _WIN32
  #define fseeko _fseeki64
  #define ftello _ftelli64
#endif

#include <stdio.h>
#include <stdlib.h>   // Required for alloca and rand
#include "libs/sqlite3.h"  // Include SQLite header file
#include "libs/mongoose.h" // Include Mongoose header file

#define CBASE_PREFIX "cb_"

// SQLite db file location
#define DATA_PATH CBASE_PREFIX"data"
#define DB_PATH  DATA_PATH"/data.db"

#define PUBLIC_DIR CBASE_PREFIX"public"

// Initialize Mongoose server options
char *s_http_port = "http://localhost:8080";

sqlite3 *db;

struct user {
  const char *name, *pass, *token;
};

// Parse HTTP requests, return authenticated user or NULL
static struct user *getuser(struct mg_http_message *hm) {
    //TODO use SQLite DB
    static struct user users[] = {
        {"admin", "admin", "admin_token"},
        {"user1", "pass1", "user1_token"},
        {"user2", "pass2", "user2_token"},
        {NULL, NULL, NULL},
    };

    char user[256], pass[256];
    struct user *u;
    mg_http_creds(hm, user, sizeof(user), pass, sizeof(pass));

    if (user[0] == '\0') { // token auth
        for (u = users; u->name != NULL; u++)
        if (strcmp(pass, u->token) == 0) return u;
    } else if (user[0] != '\0' && pass[0] != '\0') { // login user & pass
        for (u = users; u->name != NULL; u++)
        if (strcmp(user, u->name) == 0 && strcmp(pass, u->pass) == 0) {
            return u;
        }
    }
    return NULL;
}

// HTTP event handler
static void ev_handler(struct mg_connection *c, int ev, void *ev_data) {
    if (ev != MG_EV_HTTP_MSG) return;

    struct mg_http_message *hm = (struct mg_http_message *) ev_data;

    // UNAUTHENTICATED API :
    if(mg_match(hm->uri, mg_str("/public"), NULL)) {

        // struct mg_http_serve_opts opts = {
        //     .root_dir = "/ui/dist",
        //     .fs = &mg_fs_packed
        // };
        // return mg_http_serve_dir(c, ev_data, &opts);

        struct mg_http_serve_opts opts = {
            .root_dir = PUBLIC_DIR
        };
        return mg_http_serve_dir(c, ev_data, &opts);
    }

    struct user *u = getuser(hm);

    // AUTHENTICATED API :
    if(u != NULL) {

        // Admin 
        if(mg_match(hm->uri, mg_str("/api/login"), NULL)) {
            return mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"token\": %m}\n", MG_ESC(u->token));
        }

        // API endpoint
        else if (mg_match(hm->uri, mg_str("/api/tables"), NULL)) {
            //TODO Handle table operations
            const char* sql = "SELECT name FROM sqlite_master WHERE type='table';";
        }

    } // END AUTHENTICATED API -x-

    struct mg_http_serve_opts opts = {
        .root_dir = "/ui/dist",
        .fs = &mg_fs_packed
    };
    mg_http_serve_dir(c, ev_data, &opts);
    // struct mg_http_serve_opts opts = {
    //     .root_dir = PUBLIC_DIR
    // };
    // mg_http_serve_dir(c, ev_data, &opts);
    
}

int init_mg(struct mg_mgr *mgr){
    struct mg_connection *nc;
    // Initialize Mongoose
    mg_mgr_init(mgr);
    mg_log_set(MG_LL_DEBUG);
    nc = mg_http_listen(mgr, s_http_port, ev_handler, NULL);
    if (nc == NULL) {
        fprintf(stderr, "Failed to create listener\n");
        return 1;
    }
    return 0;
}

int init_db() {
    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    //TODO add init tables if not existing
    return 0;
}

void create_directory_if_not_exists(const char *dir_name) {
    struct stat st = {0};
    if (stat(dir_name, &st) == -1) {
        if (mkdir(dir_name, 0755) == 0) {
            fprintf(stdout, "Directory '%s' created successfully.\n", dir_name);
        } else {
            fprintf(stderr, "mkdir error while creating directory: %s\n", dir_name);
        }
    }
}

void init_dir() {
    create_directory_if_not_exists("cb_public");
    create_directory_if_not_exists("cb_data");
}

int main(void) {
    struct mg_mgr mgr;

    init_dir();
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
    sqlite3_close(db);
    return 0;
}