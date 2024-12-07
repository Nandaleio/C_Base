
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>

#include "../includes/db.h"
#include "../includes/utils.h"
#include "../includes/jwt.h"

#include "../libs/parson.h"
#include "../libs/mongoose.h"
#include "../libs/log.h"

#define CBASE_VERSION "0.0.1"

#define MG_API_HEADERS "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: *\r\n"

// Initialize Mongoose server options
char *s_http_port = "http://0.0.0.0:8080";

static volatile int running = 1;
void shut_down(int dummy) { running = 0; }

void admin_api(struct mg_connection *c, int ev, void *ev_data, struct mg_http_message *hm);
void standard_api(struct mg_connection *c, int ev, void *ev_data, struct mg_http_message *hm);
// TODO void websocket_api();

// HTTP event handler
static void ev_handler(struct mg_connection *c, int ev, void *ev_data)
{
    if (ev != MG_EV_HTTP_MSG) return;

    struct mg_http_message *hm = (struct mg_http_message *)ev_data;

    if (mg_strcmp(hm->method, mg_str("OPTIONS")) == 0)
    {
        return mg_http_reply(c, 204, MG_API_HEADERS "Access-Control-Allow-Headers: *\r\n\r\n", "No Content");
    }

    // ----- SECURED API -----
    if (mg_match(hm->uri, mg_str("/api/#"), NULL))
    {

        if (mg_match(hm->uri, mg_str("/api/admin/#"), NULL))
        {
            return admin_api(c, ev, ev_data, hm);
        }

        return standard_api(c, ev, ev_data, hm);
    }

    // ----- UNPROTECTED API -----
    struct mg_http_serve_opts opts;
    memset(&opts, 0, sizeof(opts));

    if (mg_match(hm->uri, mg_str("/_/#"), NULL))
    {
        hm->uri.buf += 2;
        hm->uri.len -= 2;

        opts.root_dir = "/ui/dist";
        opts.fs = &mg_fs_packed;
    }
    else
    {
        opts.root_dir = "./cb_public";
    }

    mg_http_serve_dir(c, ev_data, &opts);
}

// Initialize Mongoose
int init_mg(struct mg_mgr *mgr)
{
    struct mg_connection *nc;
    mg_mgr_init(mgr);
    nc = mg_http_listen(mgr, s_http_port, ev_handler, NULL);
    if (nc == NULL)
    {
        log_error("Failed to create listener");
        return 1;
    }
    return 0;
}

int create_directory_if_not_exists(const char *dir_name)
{
    struct stat st = {0};
    if (stat(dir_name, &st) == -1)
    {
        if (mkdir(dir_name, 0755) == 0)
        {
            log_trace("Directory '%s' created successfully.", dir_name);
        }
        else
        {
            log_error("mkdir error while creating directory: %s", dir_name);
            return 1;
        }
    }
    return 0;
}

int init_dir()
{
    int ret = 0;
    ret += create_directory_if_not_exists("cb_public");
    ret += create_directory_if_not_exists("cb_data");
    // ret += create_directory_if_not_exists("cb_hooks");

    return ret;
}

int handle_arguments(int argc, char *argv[])
{
    int ret = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
        {
            log_info("C_BASE HELP :");
            log_info("--help (-h) \t\t: Display C_BASE help args (duh)");
            log_info("--version (-v) \t: Display C_BASE and dependencies version");
            log_info("--host (-p) \t\t: Change the host address (default: http://localhost:8080)");
            log_info("--logs (-l) \t\t: Change the logs level. eg: 'trace', 'debug', 'info', 'warn', 'error', 'fatal' (default: info)");
            ret += 1;
        }
        else if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0)
        {
            log_info("C_BASE version: %s", CBASE_VERSION);
            log_info("\t- Mongoose version: %s", MG_VERSION);
            log_info("\t- SQLite version: %s", SQLITE_VERSION);
            log_info("\t- Parson version: %s", PARSON_VERSION_STRING);
            log_info("\t- Log version: %s", LOG_VERSION);
            ret += 1;
        }
        else if (strcmp(argv[i], "--host") == 0 || strcmp(argv[i], "-p") == 0)
        {
            i++;
            s_http_port = argv[i];
        }
        else if (strcmp(argv[i], "--logs") == 0 || strcmp(argv[i], "-l") == 0)
        {
            i++;
            char *level = argv[i];
            for (; *level; ++level)
                *level = tolower(*level);

            int int_level = LOG_INFO;
            if (strcmp(argv[i], "trace") == 0)
            {
                int_level = LOG_TRACE;
            }
            else if (strcmp(argv[i], "debug") == 0)
            {
                int_level = LOG_DEBUG;
            }
            else if (strcmp(argv[i], "warn") == 0)
            {
                int_level = LOG_WARN;
            }
            else if (strcmp(argv[i], "error") == 0)
            {
                int_level = LOG_ERROR;
            }
            else if (strcmp(argv[i], "fatal") == 0)
            {
                int_level = LOG_FATAL;
            }
            log_set_level(int_level);
        }
    }

    return ret;
}

int main(int argc, char *argv[])
{

    if (handle_arguments(argc, argv) != 0)
        return 0;

    struct mg_mgr mgr;

    if (init_dir() != 0)
        return 1;
    if (db_init() != 0)
        return 1;
    if (init_mg(&mgr) != 0)
        return 1;

    // Set up HTTP server
    log_info("Starting web server on %s", s_http_port);

    FILE *fptr;
    fptr = fopen(LOG_PATH, "a");
    if (fptr == NULL || log_add_fp(fptr, LOG_INFO) != 0)
    {
        log_error("Failed to open or read logs file");
    }
    log_add_callback(db_sqlite_log_callback, NULL, LOG_INFO);

    signal(SIGINT, shut_down);

    while (running)
    {
        mg_mgr_poll(&mgr, 1000);
    }

    log_info("Shuting down...");

    fclose(fptr);

    mg_mgr_free(&mgr);
    db_close();
    return 0;
}

void admin_api(struct mg_connection *c, int ev, void *ev_data, struct mg_http_message *hm)
{
    if (mg_match(hm->uri, mg_str("#/login"), NULL))
    {
        char *username = mg_json_get_str(hm->body, "$.username");
        char *password = mg_json_get_str(hm->body, "$.password");
        char *json = db_admin_login(username, password);
        mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
        json_free_serialized_string(json);
        return;
    }
    
    // ADMIN CHECK GUARD
    char user[1], token[512];
    mg_http_creds(hm, user, sizeof(user), token, sizeof(token));
    if (token == NULL || jwt_verify(token, JWT_SECRET_KEY, true) != 0)
    {
        return mg_http_reply(c, 401, MG_API_HEADERS, "Unauthorized");
    }

    if (mg_match(hm->uri, mg_str("#/logs"), NULL))
    {
        char *json = db_get_logs();
        mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
        json_free_serialized_string(json);
        return;
    }

    if (mg_match(hm->uri, mg_str("#/query"), NULL))
    {
        char *query = mg_json_get_str(hm->body, "$.query");
        char *json = db_query(query);
        mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
        json_free_serialized_string(json);
        return;
    }

    struct mg_str caps[3];
    if(mg_match(hm->uri, mg_str("#/admin/*"), caps)){
        char *admin_id = caps[20].buf;
        if (mg_strcmp(hm->method, mg_str("POST")) == 0) {
            char *username = mg_json_get_str(hm->body, "$.username");
            char *password = mg_json_get_str(hm->body, "$.password");
            char *json = db_add_admin(username, password);
            mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
            free(json);
            return;
        }
        if (mg_strcmp(hm->method, mg_str("DELETE")) == 0) {
            char *json = db_delete_admin(admin_id);
            mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
            free(json);
            return;
        }
    }

    if (mg_match(hm->uri, mg_str("#/admins"), NULL))
    {
        char *json = db_get_admins();
        mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
        json_free_serialized_string(json);
        return;
    }
}

void standard_api(struct mg_connection *c, int ev, void *ev_data, struct mg_http_message *hm)
{
    // EXCEPT REGISTER AND LOGIN
    if (mg_match(hm->uri, mg_str("/api/auth/register"), NULL))
    {
        char *username = mg_json_get_str(hm->body, "$.username");
        char *password = mg_json_get_str(hm->body, "$.password");
        char *json = db_add_user(username, password);
        mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
        free(json);
        return;
    }

    if (mg_match(hm->uri, mg_str("/api/auth/login"), NULL))
    {
        char *username = mg_json_get_str(hm->body, "$.username");
        char *password = mg_json_get_str(hm->body, "$.password");
        char *json = db_login(username, password);
        mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
        free(json);
        return;
    }

    // AUTH CHECK GUARD
    char user[1], token[512];
    mg_http_creds(hm, user, sizeof(user), token, sizeof(token));
    if (token == NULL || jwt_verify(token, JWT_SECRET_KEY, false) != 0)
    {
        return mg_http_reply(c, 401, MG_API_HEADERS, "Unauthorized");
    }

    struct mg_str caps[3];
    if (mg_match(hm->uri, mg_str("*/table/*"), caps))
    {
        char *table_name = strtok(caps[1].buf, " ");
        char *json = db_get_table(table_name);
        mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
        json_free_serialized_string(json);
        return;
    }

    if (mg_match(hm->uri, mg_str("*/tables"), NULL))
    {
        char *json = db_get_tables();
        mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
        json_free_serialized_string(json);
        return;
    }

    if (mg_match(hm->uri, mg_str("*/version"), NULL))
    {
        mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", "{\"version\": \""CBASE_VERSION"\"}");
        return;
    }

    return mg_http_reply(c, 404, MG_API_HEADERS "Access-Control-Allow-Headers: *\r\n\r\n", "Not Found");
}