

#include <stdio.h>
#include "jwt.h"
#include "utils.h"
#include "../libs/mongoose.h"
#include "../libs/log.h"

// Sign JWT using Mongoose HMAC
char *jwt_sign(const char *header, const char *payload, const char *secret) {
    // Combine header and payload

    
    char *base64_header = base64_encode(header, strlen(header));
    char *base64_payload = base64_encode(payload, strlen(payload));

    char *header_payload = malloc(strlen(base64_header) + strlen(base64_payload) + 2);
    sprintf(header_payload, "%s.%s", base64_header, base64_payload);

    free(base64_header);
    free(base64_payload);

    // Compute HMAC-SHA256 signature using Mongoose
    uint8_t hmac[32];
    mg_hmac_sha256(hmac, secret, strlen(secret), header_payload, strlen(header_payload));

    char *signature = base64_encode(hmac, sizeof(hmac));

    char *jwt = malloc(strlen(header_payload) + strlen(signature) + 2);
    sprintf(jwt, "%s.%s", header_payload, signature);

    free(header_payload);
    free(signature);

    return jwt;
}

// Verify JWT using Mongoose HMAC
int jwt_verify(const char *jwt, const char *secret) {
    char *header_payload = strtok(jwt, ".");
    char *received_signature = strtok(NULL, ".");
    
    log_debug("jwt header_payload: %s",header_payload);
    log_debug("jwt received_signature: %s",received_signature);

    // Recompute the signature
    uint8_t hmac[32];
    mg_hmac_sha256(hmac, secret, strlen(secret), header_payload, strlen(header_payload));

    log_debug("hmac: %s",hmac);

    char *expected_signature = base64_encode(hmac, sizeof(hmac));
    int valid = memcmp(received_signature, expected_signature) == 0;

    free(expected_signature);
    free(received_signature);
    
    log_debug("is valid: %d",valid);

    return valid;
}