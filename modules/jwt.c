

#include "jwt.h"
#include "utils.h"
#include "../libs/mongoose.h"

// Sign JWT using Mongoose HMAC
char *jwt_sign(const char *header, const char *payload, const char *secret) {
    // Combine header and payload
    char *header_payload = malloc(strlen(header) + strlen(payload) + 2);
    sprintf(header_payload, "%s.%s", header, payload);

    // Compute HMAC-SHA256 signature using Mongoose
    uint8_t hmac[32];
    mg_hmac_sha256(secret, strlen(secret), header_payload, strlen(header_payload), hmac);

    // Base64 encode the signature
    char *signature = base64_encode(hmac, sizeof(hmac));

    // Combine header.payload.signature
    char *jwt = malloc(strlen(header_payload) + strlen(signature) + 2);
    sprintf(jwt, "%s.%s", header_payload, signature);

    free(header_payload);
    free(signature);

    return jwt;
}

// Verify JWT using Mongoose HMAC
int jwt_verify(const char *jwt, const char *secret) {
    char *jwt_copy = strdup(jwt);
    char *header_payload = strtok(jwt_copy, ".");
    char *received_signature = strtok(NULL, ".");

    // Recompute the signature
    uint8_t hmac[32];
    mg_hmac_sha256(secret, strlen(secret), header_payload, strlen(header_payload), hmac);

    char *expected_signature = base64_encode(hmac, sizeof(hmac));
    int valid = strcmp(received_signature, expected_signature) == 0;

    free(jwt_copy);
    free(expected_signature);

    return valid;
}