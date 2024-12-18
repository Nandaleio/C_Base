

#include <stdio.h>
#include "jwt.h"
#include "utils.h"
#include "../libs/mongoose.h"
#include "../libs/log.h"
#include "../libs/parson.h"

// Sign JWT using Mongoose HMAC
char *jwt_sign(const char *header, const char *payload, const char *secret) {
    // Combine header and payload

    char *base64_header = base64url_encode(header, strlen(header));
    char *base64_payload = base64url_encode(payload, strlen(payload));

    char *header_payload = malloc(strlen(base64_header) + strlen(base64_payload) + 2);
    sprintf(header_payload, "%s.%s", base64_header, base64_payload);

    free(base64_header);
    free(base64_payload);

    // Compute HMAC-SHA256 signature using Mongoose
    uint8_t hmac[32];
    mg_hmac_sha256(hmac, secret, strlen(secret), header_payload, strlen(header_payload));

    char *signature = base64url_encode(hmac, sizeof(hmac));

    char *jwt = malloc(strlen(header_payload) + strlen(signature) + 2);
    sprintf(jwt, "%s.%s", header_payload, signature);

    free(header_payload);
    free(signature);
    
    return jwt;
}

// Verify JWT using Mongoose HMAC
int jwt_verify(const char *jwt, int is_admin) {
    char *base64_header = strtok(jwt, ".");
    char *base64_payload = strtok(NULL, ".");
    char *received_signature = strtok(NULL, ".");
    
    char *header_payload = malloc(strlen(base64_header) + strlen(base64_payload) + 2);
    sprintf(header_payload, "%s.%s", base64_header, base64_payload);
    
    uint8_t hmac[32];
    mg_hmac_sha256(hmac, JWT_SECRET_KEY, strlen(JWT_SECRET_KEY), header_payload, strlen(header_payload));

    char *expected_signature = base64url_encode(hmac, sizeof(hmac));
    int valid = strcmp(received_signature, expected_signature);

    if(valid) return JWT_WRONG_SIGNATURE;

    char *decoded_payload = base64url_decode(base64_payload);
    JSON_Value *json_payload = json_parse_string(decoded_payload);
    JSON_Object *payload_object = json_value_get_object(json_payload);
    double exp = json_object_get_number(payload_object, "exp");
    long current_time = (unsigned long)time(NULL);
    valid &= exp < current_time;

    if(valid) return JWT_TOKEN_EXPIRED;

    if(is_admin) {
        char *role = json_object_get_string(payload_object, "role");
        valid &= strcmp(role, "ADMIN");
    }

    json_value_free(json_payload);
    free(expected_signature);

    return valid;
}