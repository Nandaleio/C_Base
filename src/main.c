
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>

#include "db/db.h"

#include "libs/parson.h"
#include "libs/mongoose.h"
#include "libs/log.h"

#include "utils/info.h"

#include "routes/admin-route.h"
#include "routes/standard-route.h"

// Initialize Mongoose server options
char *s_http_port = "http://0.0.0.0:8080";

static volatile int running = 1;
void shut_down(int dummy) { running = 0; }

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
            return admin_routes(c, ev, ev_data, hm);
        }

        return standard_routes(c, ev, ev_data, hm);
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



