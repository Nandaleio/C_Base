#ifndef ADMIN_ROUTES_H
#define ADMIN_ROUTES_H

#include "../libs/mongoose.h"

#define MG_API_HEADERS "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: *\r\n"

void admin_routes(struct mg_connection *c, int ev, void *ev_data, struct mg_http_message *hm);

#endif
