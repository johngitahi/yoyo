#include "vault.h"

void initYoyoVault()
{
    if (access(get_yoyo_path(), F_OK) == 0)
    {
        fprintf(stderr, MSG_YOYO_ALREADY_INIT);
        exit(10);
    }

    char master_password[256];
    char rep_master_password[256];
    char currentTimeStr[30];
    struct tm *tm_info;

    time_t now = time(NULL);
    tm_info = localtime(&now);
    strftime(currentTimeStr, sizeof(currentTimeStr), "%m/%d/%Y %H:%M:%S", tm_info);

    strncpy(master_password, getpass("Enter master password: "), sizeof master_password - 1);
    master_password[sizeof master_password - 1] = '\0';
    strncpy(rep_master_password, getpass("Repeat the same password you entered: "),
            sizeof rep_master_password - 1);
    rep_master_password[sizeof rep_master_password - 1] = '\0';

    if (strcmp(master_password, rep_master_password) != 0)
    {
        fprintf(stderr, "Passwords do not match.\n");
        exit(3);
    }

    unsigned char salt[SALT_BYTES];
    randombytes_buf(salt, sizeof salt);

    unsigned char master_key[KEY_BYTES];
    if (crypto_pwhash(master_key, sizeof master_key, master_password, strlen(master_password), salt,
                      crypto_pwhash_OPSLIMIT_MODERATE, crypto_pwhash_MEMLIMIT_MODERATE,
                      crypto_pwhash_ALG_DEFAULT) != 0)
    {
        printf("Key derivation failed (out of memory?)\n");
        exit(2);
    }

    json_t *vault_json = json_object();
    json_object_set_new(vault_json, "version", json_string(YOYO_VER));
    json_object_set_new(vault_json, "lastModified", json_string(currentTimeStr));
    json_object_set_new(vault_json, "entries", json_array());

    char *json_str = json_dumps(vault_json, JSON_INDENT(2));
    json_decref(vault_json);

    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    randombytes_buf(nonce, sizeof nonce);

    size_t json_len = strlen(json_str);
    unsigned char *ciphertext = malloc(json_len + crypto_secretbox_MACBYTES);
    if (!ciphertext)
        exit(5);

    crypto_secretbox_easy(ciphertext, (unsigned char *)json_str, json_len, nonce, master_key);

    FILE *fp = fopen(get_yoyo_path(), "wb");
	if (!fp) {
		perror("fopen");
		exit(6);
	}

    fwrite(salt, 1, SALT_BYTES, fp);
    fwrite(nonce, 1, crypto_secretbox_NONCEBYTES, fp);
    fwrite(ciphertext, 1, json_len + crypto_secretbox_MACBYTES, fp);
    fclose(fp);

    sodium_memzero(master_key, sizeof(master_key));
    sodium_memzero(master_password, sizeof(master_password));
    sodium_memzero(rep_master_password, sizeof(rep_master_password));
    sodium_memzero(ciphertext, json_len + crypto_secretbox_MACBYTES);
    free(ciphertext);
}

json_t *readYoyoVault(char *masterPassword)
{
    FILE *fp = fopen(get_yoyo_path(), "rb");
    if (!fp)
    {
        perror("fopen");
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    unsigned char *file_buffer = malloc(file_size);
    if (!file_buffer)
    {
        fprintf(stderr, "Unable to create memory room to read Vault.\n");
        exit(13);
    }

    fread(file_buffer, 1, file_size, fp);
    fclose(fp);

    unsigned char *salt = file_buffer;
    unsigned char *nonce = file_buffer + SALT_BYTES;
    unsigned char *ciphertext = file_buffer + SALT_BYTES + crypto_secretbox_NONCEBYTES;
    size_t ciphertext_len = file_size - SALT_BYTES - crypto_secretbox_NONCEBYTES;

    unsigned char master_key[KEY_BYTES];
    if (crypto_pwhash(master_key, sizeof master_key, masterPassword, strlen(masterPassword), salt,
                      crypto_pwhash_OPSLIMIT_MODERATE, crypto_pwhash_MEMLIMIT_MODERATE,
                      crypto_pwhash_ALG_DEFAULT) != 0)
    {
        fprintf(stderr, "You entered a wrong password.\n");
        exit(12);
    }

    size_t decrypted_len = ciphertext_len - crypto_secretbox_MACBYTES;
    unsigned char *decrypted = malloc(decrypted_len + 1);
    if (crypto_secretbox_open_easy(decrypted, ciphertext, ciphertext_len, nonce, master_key) != 0)
    {
        fprintf(stderr, "Decryption failed\n");
        free(ciphertext);
        free(decrypted);
        exit(13);
    }
    decrypted[decrypted_len] = '\0';

    json_error_t error;
    json_t *vaultRepJSON = json_loads((const char *)decrypted, 0, &error);
    if (!vaultRepJSON)
    {
        fprintf(stderr, "Failed to read the vault.(maybe a corrupted Yoyo Vault)\n");
        exit(14);
    }

    return vaultRepJSON;

    sodium_memzero(master_key, sizeof master_key);
    sodium_memzero(masterPassword, sizeof masterPassword);
    free(file_buffer);
    free(decrypted);
}

void addToYoyo(json_t *yoyoVault, const char *service, const char *uid, const char *servicePassword,
               char *masterPassword)
{
    json_t *entries = json_object_get(yoyoVault, "entries");
    if (!json_is_array(entries))
    {
        json_decref(yoyoVault);
        fprintf(stderr, "Yoyo Vault Corrupted\n");
        exit(14);
    }

    json_t *entry = json_object();
    json_object_set_new(entry, "service", json_string(service));
    json_object_set_new(entry, "uid", json_string(uid));
    json_object_set_new(entry, "password", json_string(servicePassword));

    json_array_append_new(entries, entry);

    char currentTimeStr[30];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(currentTimeStr, sizeof currentTimeStr, "%m/%d/%Y %H:%M:%S", tm_info);

    json_object_set_new(yoyoVault, "lastModified", json_string(currentTimeStr));

    char *json_str = json_dumps(yoyoVault, JSON_INDENT(2));
    size_t json_len = strlen(json_str);

    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    randombytes_buf(nonce, sizeof nonce);

    unsigned char *ciphertext = malloc(json_len + crypto_secretbox_MACBYTES);
    if (!ciphertext)
    {
        free(json_str);
        exit(15);
    }

    FILE *fp = fopen(get_yoyo_path(), "rb");
    if (!fp)
    {
        perror("open vault error");
        exit(13);
    }
    unsigned char salt[SALT_BYTES];
    fread(salt, 1, SALT_BYTES, fp);
    fclose(fp);

    unsigned char master_key[KEY_BYTES];
    if (crypto_pwhash(master_key, sizeof master_key, masterPassword, strlen(masterPassword), salt,
                      crypto_pwhash_OPSLIMIT_MODERATE, crypto_pwhash_MEMLIMIT_MODERATE,
                      crypto_pwhash_ALG_DEFAULT) != 0)
    {
        printf("Key derivation failed (out of memory)\n");
        exit(2);
    }

    if (crypto_secretbox_easy(ciphertext, (unsigned char *)json_str, json_len, nonce, master_key) !=
        0)
    {
        fprintf(stderr, "Encryption failed\n");
        free(json_str);
        free(ciphertext);
        exit(16);
    }

    fp = fopen(get_yoyo_path(), "wb");
    fwrite(salt, 1, SALT_BYTES, fp);
    fwrite(nonce, 1, crypto_secretbox_NONCEBYTES, fp);
    fwrite(ciphertext, 1, json_len + crypto_secretbox_MACBYTES, fp);
    fclose(fp);

    sodium_memzero(master_key, sizeof master_key);
    sodium_memzero(json_str, json_len);
    free(json_str);
    sodium_memzero(ciphertext, json_len + crypto_secretbox_MACBYTES);
    free(ciphertext);
}

void getFromYoyo(const char *service_name, char *masterPassword)
{
    if (access(get_yoyo_path(), F_OK) != 0)
    {
        fprintf(stderr, "Vault not initialized. Run `yoyo init` first.\n");
        exit(11);
    }
	
    json_t *yoyoVaultJSON = readYoyoVault(masterPassword);

    json_t *entries = json_object_get(yoyoVaultJSON, "entries");
    if (!json_is_array(entries))
    {
        json_decref(yoyoVaultJSON);
        fprintf(stderr, "Yoyo Vault Corrupted: 'entries' is not an array\n");
        exit(14);
    }

    size_t index;
    json_t *entry;
    json_array_foreach(entries, index, entry)
    {
        json_t *service = json_object_get(entry, "service");
        json_t *password = json_object_get(entry, "password");

		if (!json_is_string(service))
			continue;
		if (strcmp(json_string_value(service), service_name))
			continue;
		if (!json_is_string(password))
			continue;

		char *pw_copy = strdup(json_string_value(password));
		copy_to_clipboard(pw_copy, TIMEDOWN);
		free(pw_copy);
		json_decref(yoyoVaultJSON);
		break;
    }
}

void showAllInYoyo(char *masterPassword)
{
    json_t *yoyoVaultJSON = readYoyoVault(masterPassword);

    json_t *entries = json_object_get(yoyoVaultJSON, "entries");
    if (!json_is_array(entries))
    {
        fprintf(stderr, "Yoyo Vault Corrupted: 'entries' is not an array\n");
        json_decref(yoyoVaultJSON);
        exit(14);
    }

    size_t index;
    json_t *entry;

    json_array_foreach(entries, index, entry)
    {
        json_t *service = json_object_get(entry, "service");
        if (json_is_string(service))
        {
            printf("%s\n", json_string_value(service));
        }
    }

    json_decref(yoyoVaultJSON);
}
