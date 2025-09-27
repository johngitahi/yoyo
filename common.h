#ifndef YOYO_COMMON_H
#define YOYO_COMMON_H

#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sodium.h>
#include <jansson.h>

#define YOYO_USAGE \
	"Usage: yoyo [OPTIONS] COMMAND [ARGS...]\n" \
	"\n" \
	"A simple local password manager.\n" \
	"\n" \
	"Options:\n" \
	"  -h, --help       Show this help message and exit\n" \
	"  -v, --version    Show version information\n" \
	"\n" \
	"Commands:\n" \
	"  init             Initialize a new vault and set a master password\n" \
	"  get <service>    Get a password for a specific service\n" \
	"  list             List all the passwords in the Yoyo vault\n"

#define MSG_YOYO_ALREADY_INIT \
	"Your Yoyo vault is already set up.\n" \
	"Use `yoyo add` to store a password, or `yoyo list` to view saved passwords.\n"

#define YOYO_VER "v0.0"
#define KEY_BYTES 32        // 256-bit encryption key

#define SALT_BYTES crypto_pwhash_SALTBYTES

void initYoyoVault();
json_t *readYoyoVault(char* masterPassword);
void addToYoyo(json_t *yoyoVault, const char *service, const char *uid, const char *servicePassword, char *masterPassword);


#endif
