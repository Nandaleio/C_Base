

#include <stdio.h>
#include "../includes/jwt.h"
#include "../includes/utils.h"
#include "../libs/mongoose.h"
#include "../libs/log.h"

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
int jwt_verify(const char *jwt, const char *secret, int is_admin) {
    char *base64_header = strtok(jwt, ".");
    char *base64_payload = strtok(NULL, ".");
    char *received_signature = strtok(NULL, ".");
    
    char *header_payload = malloc(strlen(base64_header) + strlen(base64_payload) + 2);
    sprintf(header_payload, "%s.%s", base64_header, base64_payload);
    
    uint8_t hmac[32];
    mg_hmac_sha256(hmac, secret, strlen(secret), header_payload, strlen(header_payload));

    char *expected_signature = base64url_encode(hmac, sizeof(hmac));
    int valid = strcmp(received_signature, expected_signature);

    if(is_admin) {
        char decoded_payload[strlen(base64_payload)];
        mg_base64_decode(base64_payload, strlen(base64_payload), decoded_payload, strlen(base64_payload));
        valid &= (strstr(decoded_payload, "ADMIN") != NULL);
    }

    free(expected_signature);
    free(received_signature);

    return valid;
}