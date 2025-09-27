#include "common.h"

int main(void)
{
    FILE *fp = fopen("/home/gth/.yoyo", "rb");

    if (!fp)
    {
        perror("fopen");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    unsigned char *file_buffer = malloc(file_size);
    if (!file_buffer)
        exit(2);
    fread(file_buffer, 1, file_size, fp);
    fclose(fp);

    unsigned char *salt = file_buffer;               // FIRST salt bytes
    unsigned char *nonce = file_buffer + SALT_BYTES; // next noncebytes
    unsigned char *ciphertext = file_buffer + SALT_BYTES + crypto_secretbox_NONCEBYTES;
    size_t ciphertext_len = file_size - SALT_BYTES - crypto_secretbox_NONCEBYTES;

    char *password = getpass("Enter your master password: ");

    unsigned char master_key[KEY_BYTES];
    if (crypto_pwhash(master_key, sizeof master_key, password, strlen(password), salt,
                      crypto_pwhash_OPSLIMIT_MODERATE, crypto_pwhash_MEMLIMIT_MODERATE,
                      crypto_pwhash_ALG_DEFAULT) != 0)
    {
        fprintf(stderr, "key deriv failed\n");
        exit(4);
    }

    size_t decrypted_len = ciphertext_len - crypto_secretbox_MACBYTES;
    unsigned char *decrypted = malloc(decrypted_len + 1);

    if (crypto_secretbox_open_easy(decrypted, ciphertext, ciphertext_len, nonce, master_key) != 0)
    {
        fprintf(stderr, "Decryption failed\n");
        free(ciphertext);
        free(decrypted);
        exit(6);
    }
    decrypted[decrypted_len] = '\0';
    printf("%s\n", decrypted);

    sodium_memzero(master_key, sizeof master_key);
    sodium_memzero(password, sizeof password);
    free(file_buffer);
    free(decrypted);
    return (0);
}
