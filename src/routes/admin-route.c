
#include "admin-route.h"

#include "../controllers/admin-controller.h"
#include "../controllers/auth-controller.h"
#include "../utils/jwt.h"
#include "../db/db.h"
#include "../utils/utils.h"
#include "../libs/parson.h"

void admin_routes(struct mg_connection *c, int ev, void *ev_data, struct mg_http_message *hm)
{

    if (mg_match(hm->uri, mg_str("#/login"), NULL))
    {
        char *username = mg_json_get_str(hm->body, "$.username");
        char *password = mg_json_get_str(hm->body, "$.password");
        char *json = auth_admin_login(username, password);
        mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
        json_free_serialized_string(json);
        return;
    }

    // ADMIN CHECK GUARD
    char user[1], token[512];
    mg_http_creds(hm, user, sizeof(user), token, sizeof(token));
    if (token == NULL || jwt_verify(token, true) != 0)
    {
        return mg_http_reply(c, 401, MG_API_HEADERS, "Unauthorized");
    }


    struct mg_str caps[3];

    // --- LOGS ---
    if (mg_match(hm->uri, mg_str("#/logs"), NULL))
    {
        char *json = ctr_get_logs();
        mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
        json_free_serialized_string(json);
        return;
    }

    // --- QUERIES ---
    if (mg_match(hm->uri, mg_str("#/query"), NULL))
    {
        char *query = mg_json_get_str(hm->body, "$.query");
        char *json = db_query(query);
        mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
        json_free_serialized_string(json);
        return;
    }



    // --- TABLES ---
    if (mg_match(hm->uri, mg_str("#/table/*"), caps))
    {
        char *table_name = strtok(caps[1].buf, " ");
        if (mg_strcmp(hm->method, mg_str("POST")) == 0)
        {
            //char *json_column = mg_json_get_str(hm->body, "$");
            char *json = ctr_create_table(table_name, hm->body.buf);
            mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
            json_free_serialized_string(json);
            return;
        }
        if (mg_strcmp(hm->method, mg_str("DELETE")) == 0)
        {
            char *json = ctr_delete_table(table_name);
            mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
            json_free_serialized_string(json);
            return;
        }
    }

    //  --- CONFIGS ---
    if (mg_match(hm->uri, mg_str("#/configs"), NULL))
    {
        if (mg_strcmp(hm->method, mg_str("PUT")) == 0)
        {
            char *name = mg_json_get_str(hm->body, "$.name");
            char *value = mg_json_get_str(hm->body, "$.value");
            char *json = update_config(name, value);
            mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
            json_free_serialized_string(json);
            return;

        } else if (mg_strcmp(hm->method, mg_str("DELETE")) == 0) {
            char *name = mg_json_get_str(hm->body, "$.name");
            char *json = delete_config(name);
            mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
            json_free_serialized_string(json);
            return;

        } else if(mg_strcmp(hm->method, mg_str("POST")) == 0) {
            char *name = mg_json_get_str(hm->body, "$.name");
            char *value = mg_json_get_str(hm->body, "$.value");
            char *type = mg_json_get_str(hm->body, "$.type");
            char *json = add_config(name, value, type);
            mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
            json_free_serialized_string(json);
            return;

        }else {
            char *json = ctr_get_configs();
            mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
            json_free_serialized_string(json);
            return;
        }
    }


    // --- ADMINS ---
    if (mg_match(hm->uri, mg_str("#/admins"), NULL))
    {
        char *json = ctr_get_admins();
        mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
        json_free_serialized_string(json);
        return;
    }

    if (mg_match(hm->uri, mg_str("#/admin/*"), caps))
    {
        char *admin_id = caps[1].buf;
        if (mg_strcmp(hm->method, mg_str("POST")) == 0)
        {
            char *username = mg_json_get_str(hm->body, "$.username");
            char *password = mg_json_get_str(hm->body, "$.password");
            char *json = auth_add_admin(username, password);
            mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
            json_free_serialized_string(json);
            return;
        }
        if (mg_strcmp(hm->method, mg_str("DELETE")) == 0)
        {
            char *json = auth_delete_admin(admin_id);
            mg_http_reply(c, 200, MG_API_HEADERS, "%s\n", json);
            json_free_serialized_string(json);
            return;
        }
    }


}