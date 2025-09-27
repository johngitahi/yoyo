/*
 * yoyo - A simple local password manager
 *
 * written by gĩtahi(opl.to/gth)
 *
 * if you see this, remember to live because you have one life only
 */

#include "common.h"

int main(int argc, char *argv[argc + 1])
{
    if (argc < 2)
    {
        fputs(YOYO_USAGE, stderr);
        return (1);
    }

    if (sodium_init() < 0)
    {
        printf("Libsodium initialization failed\n");
        return (1);
    }

    if (strcmp(argv[1], "init") == 0)
    {
        initYoyoVault();
    }
    else if (strcmp(argv[1], "add") == 0)
    {
        if (argc < 5 || strcmp(argv[2], "-h") == 0)
        {
            fprintf(stderr, "Usage: yoyo add <service> <username/email> <password>\n");
            exit(16);
        }

        if (access("/home/gth/.yoyo", F_OK) != 0)
        {
            fprintf(stderr, "Vault not initialized. Run `yoyo init` first.\n");
            exit(11);
        }

        char *masterPassword = getpass("Enter your master password: ");
        const char *service = argv[2];
        const char *uid = argv[3];
        const char *servicePassword = argv[4];

        json_t *yoyoVaultJSON = readYoyoVault(masterPassword);

        // add to yoyo
        addToYoyo(yoyoVaultJSON, service, uid, servicePassword, masterPassword);

        json_decref(yoyoVaultJSON);

        // continue
    }
    else if (strcmp(argv[1], "get") == 0)
    {
        // getPassword
        // getFromYoyo();
    }
    else if (strcmp(argv[1], "list") == 0)
    {
        // listAllPasswords
        // showAllInYoyo();
    }
    else if (strcmp(argv[1], "-h") == 0 || (strcmp(argv[1], "--help") == 0))
    {
        fputs(YOYO_USAGE, stdout);
        return (0);
    }
    else if (strcmp(argv[1], "-v") == 0 || (strcmp(argv[1], "--version") == 0))
    {
        fputs(YOYO_VER, stdout);
        return (0);
    }
    else if (strcmp(argv[1], "status") == 0)
    {
        // fputs("Your Yoyo vault lives at %s", yoyo_location);
    }

    return (0);
}
