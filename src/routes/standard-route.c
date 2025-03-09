
#include "standard-route.h"

#include "../utils/info.h"

#include "../controllers/standard-controller.h"
#include "../controllers/auth-controller.h"
#include "../utils/jwt.h"
#include "../db/db.h"
#include "../libs/parson.h"

void standard_routes(struct mg_connection *c, int ev, void *ev_data, struct mg_http_message *hm) {

    if (mg_match(hm->uri, mg_str("/api/auth/register"), NULL))
    {
        char *username = mg_json_get_str(hm->body, "$.username");
        char *password = mg_json_get_str(hm->body, "$.password");
        char *json = auth_add_user(username, password);
        mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
        json_free_serialized_string(json);
        return;
    }

    if (mg_match(hm->uri, mg_str("/api/auth/login"), NULL))
    {
        char *username = mg_json_get_str(hm->body, "$.username");
        char *password = mg_json_get_str(hm->body, "$.password");
        char *json = auth_login(username, password);
        mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
        json_free_serialized_string(json);
        return;
    }

    // AUTH CHECK GUARD
    char user[1], token[512];
    mg_http_creds(hm, user, sizeof(user), token, sizeof(token));
    if (token == NULL || jwt_verify(token, false) != 0)
    {
        return mg_http_reply(c, 401, MG_API_HEADERS, "Unauthorized");
    }

    struct mg_str caps[3];
    if (mg_match(hm->uri, mg_str("*/table/*"), caps))
    {
        char *table_name = strtok(caps[1].buf, " ");
        char *where_clause = mg_http_var(hm->query, mg_str("where")).buf;
        char *json = ctr_get_table(table_name, where_clause);
        mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
        json_free_serialized_string(json);
        return;
    }


    if (mg_match(hm->uri, mg_str("*/tables"), NULL))
    {
        char *json = ctr_get_tables();
        mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
        json_free_serialized_string(json);
        return;
    }

    // UTILS

    if (mg_match(hm->uri, mg_str("*/version"), NULL))
    {
        mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", "{\"version\": \""CBASE_VERSION"\"}");
        return;
    }

    return mg_http_reply(c, 404, MG_API_HEADERS "Access-Control-Allow-Headers: *\r\n\r\n", "Not Found");

}
