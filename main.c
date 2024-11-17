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

// SQLite db file location
#define DB_PATH  "cb_data/data.db"

#define PUBLIC_DIR "cb_public"

#define MG_API_HEADERS "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: *\r\n"

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
    printf("Credentials: %s: %s\r\n", user, pass);
    if (user[0] == '\0') { // token auth
        for (u = users; u->name != NULL; u++)
        if (strcmp(pass, u->token) == 0) {
            return u;
        }
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
    // else {

    //     struct mg_http_serve_opts opts = {
    //         .root_dir = PUBLIC_DIR
    //     };
    //     mg_http_serve_dir(c, ev_data, &opts);

    // }
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

int init_db() {
    
    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    sqlite3_stmt* stmt = NULL;

    int retval;
    create_tables_if_not_existing("admin");
    //create_tables_if_not_existing("users");

    return 0;
}

int create_tables_if_not_existing(const char *table_name) {
    sqlite3_stmt* stmt = NULL;

    char *sql = "CREATE TABLE IF NOT EXISTS admin ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "created TEXT DEFAULT (SELECT DATE()),"
        "email TEXT NOT NULL, "
        "password TEXT NOT NULL "
    ")";

    int retval = sqlite3_prepare_v2(db, table_name, -1, &stmt, 0);
    if(retval != SQLITE_OK) {
        printf("%s", sqlite3_errstr(retval));
        return 1;
    }

    // retval = sqlite3_bind_text(stmt, 1, table_name, sizeof(table_name), NULL);
    // if (retval != SQLITE_OK) {
    //     printf("%s", sqlite3_errstr(retval));
    //     return 1;
    // }

    retval = sqlite3_step(stmt);
    if(retval != SQLITE_DONE) {
        printf("%s", sqlite3_errstr(retval));
    }
    retval = sqlite3_finalize(stmt);
    return retval;
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
    //create_directory_if_not_exists("cb_public");
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