#ifndef YOYO_VAULT_H
#define YOYO_VAULT_H

#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#if defined(_WIN32)
#include <windows.h>
#include <process.h>
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

#include <sodium.h>
#include <jansson.h>

#define YOYO_USAGE \
	"Usage: yoyo [OPTIONS] COMMAND [ARGS...]\n" \
	"\n" \
	"A simple local password manager.\n" \
	"\n" \
	"Options:\n" \
	"  help       Show this help message and exit\n" \
	"  version    Show version information\n" \
	"\n" \
	"Commands:\n" \
	"  init                           Initialize a new vault and set a master password\n" \
	"  add <service> <uid> <password> Add a new password to the vault\n" \
	"  get <service>                  Get a saved password for a specific service\n" \
	"  list                           List all the passwords in the Yoyo vault\n" \
	"  gen                            Generate a strong password(showed on stdout)\n" \
	"  status                         Show status of yoyo on your local"

#define MSG_YOYO_ALREADY_INIT \
	"Your Yoyo vault is already set up.\n" \
	"Use `yoyo add` to store a password, or `yoyo list` to view saved passwords.\n"

#define YOYO_VER "v0.0"
#define YOYO_VAULT ".yoyo"
#define TIMEDOWN 60


#define KEY_BYTES 32        // 256-bit encryption key

#define SALT_BYTES crypto_pwhash_SALTBYTES


void initYoyoVault();
json_t *readYoyoVault(char* masterPassword);
void addToYoyo(json_t *yoyoVault, const char *service, const char *uid, const char *servicePassword, char *masterPassword);
void getFromYoyo(const char *service,char *masterPassword);
void showAllInYoyo(char *masterPassword);

const char *get_yoyo_path();
void copy_to_clipboard(const char *text, int timeDown);
const char *get_home_dir();

void generatePassphrase(const char *wordlist_path, size_t words);

#endif
