

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    if (!jwt) return JWT_WRONG_SIGNATURE;

    // strtok mutates its input — work on a private copy so we don't
    // trample the caller's buffer (which may be const / shared).
    char *jwt_copy = strdup(jwt);
    if (!jwt_copy) return JWT_WRONG_SIGNATURE;

    char *base64_header = strtok(jwt_copy, ".");
    char *base64_payload = base64_header ? strtok(NULL, ".") : NULL;
    char *received_signature = base64_payload ? strtok(NULL, ".") : NULL;

    if (!base64_header || !base64_payload || !received_signature) {
        free(jwt_copy);
        return JWT_WRONG_SIGNATURE;
    }

    char *header_payload = malloc(strlen(base64_header) + strlen(base64_payload) + 2);
    sprintf(header_payload, "%s.%s", base64_header, base64_payload);

    uint8_t hmac[32];
    mg_hmac_sha256(hmac, (uint8_t*)JWT_SECRET_KEY, strlen(JWT_SECRET_KEY),
                   (uint8_t*)header_payload, strlen(header_payload));

    char *expected_signature = base64url_encode(hmac, sizeof(hmac));
    int sig_mismatch = strcmp(received_signature, expected_signature);

    free(header_payload);
    free(expected_signature);

    if (sig_mismatch) {
        free(jwt_copy);
        return JWT_WRONG_SIGNATURE;
    }

    char *decoded_payload = base64url_decode(base64_payload);
    JSON_Value *json_payload = json_parse_string(decoded_payload);
    free(decoded_payload);
    free(jwt_copy);

    if (!json_payload) return JWT_WRONG_SIGNATURE;

    JSON_Object *payload_object = json_value_get_object(json_payload);
    double exp = json_object_get_number(payload_object, "exp");
    long current_time = (long)time(NULL);

    if (exp < current_time) {
        json_value_free(json_payload);
        return JWT_TOKEN_EXPIRED;
    }

    if (is_admin) {
        const char *role = json_object_get_string(payload_object, "role");
        if (!role || strcmp(role, "ADMIN") != 0) {
            json_value_free(json_payload);
            return JWT_WRONG_SIGNATURE;
        }
    }

    json_value_free(json_payload);
    return 0;
}