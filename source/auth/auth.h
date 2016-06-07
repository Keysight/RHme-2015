#ifndef AUTH_H
#define AUTH_H

#define USER_ADMIN 1
#define USER_USER 0
#define USER_PRIVILEGED 2

void generate_challenge(void);
void verify_response(void);

#endif
