#ifndef JWT_H
#define JWT_H

#define JWT_SECRET_KEY "2513ea28a89bc026fa7352cc2725441c35d44cf3cb90b737adb4ed7f1ef5f6b8c2721559984cb6136214a732b4d5df26807a3e1bdf325c056e13fcdc162dfc2625ee49cacae295624128a6c4cc5e55d1ade870343c829f7d3aae132ccf1b1038bdd0d27a829090d6e3ee4deb97398b55dfc61bb0129a5aa1dccb52be4ef7a418547f40aa39453b8955c5684052ff7840b2fdc092739692a7735e1772d8a257e11bf9639716e4d1ef7cb825a6d25e97c5852fb2993ca38ab171a7443582d05ce0995dc07e31fd9a6b3f4ca2d4fa13b07b1e3196800265f34db2aa72ed02716b5842163d0f607f1bb066972549adbd8d377467aac9c8df4a421d858e10c6903afe"
#define JWT_DEFAULT_HEADER "{\"alg\":\"HS256\",\"typ\":\"JWT\"}"
#define JWT_EXPIRATION_TIME 3600

#define JWT_WRONG_SIGNATURE 1
#define JWT_TOKEN_EXPIRED 2

char *jwt_sign(const char *header, const char *payload, const char *secret);
int jwt_verify(const char *jwt, int is_admin);

#endif