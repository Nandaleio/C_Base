#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H

// USERS :
char *auth_login(char* username, char *password);
char *auth_add_user(char *username, char *password);

// ADMINS :
char *auth_add_admin(char *username, char *password);
char *auth_admin_login(char *username, char *password);

#endif