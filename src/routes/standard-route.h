#ifndef STANDARD_ROUTES_H
#define STANDARD_ROUTES_H

#include "../libs/mongoose.h"

#define MG_API_HEADERS "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: *\r\n"

void standard_routes(struct mg_connection *c, int ev, void *ev_data, struct mg_http_message *hm);

#endif