#include "vault.h"

#define MAX_WORDS 10000
#define MAX_WORD_LEN 32

static char **wordlist = NULL;
static size_t word_count = 0;

void load_wordlist(const char *filename) {
    if (wordlist != NULL) return;

    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Failed to open wordlist");
        exit(1);
    }

    wordlist = malloc(MAX_WORDS * sizeof(char *));
    if (!wordlist) {
        perror("malloc failed");
        fclose(f);
        exit(1);
    }

    char line[128];
    while (fgets(line, sizeof(line), f)) {
        char *sep = strpbrk(line, " \t");
        if (!sep) continue;

        while (*sep == ' ' || *sep == '\t') sep++;

        line[strcspn(line, "\r\n")] = 0;
        sep[strcspn(sep, "\r\n")] = 0;

        if (*sep == '\0') continue;

        wordlist[word_count] = strdup(sep);
        if (!wordlist[word_count]) {
            perror("strdup failed");
            fclose(f);
            exit(1);
        }
        word_count++;

        if (word_count >= MAX_WORDS) break;
    }

    fclose(f);

    if (word_count == 0) {
        fprintf(stderr, "No words loaded from %s\n", filename);
        exit(1);
    }
}


void random_capitalize(char *word) {
    if (!word || !*word) return;
    unsigned int r;
    randombytes_buf(&r, sizeof(r));
    size_t len = strlen(word);
    int mode = r % 2;

    if (mode == 0)
        word[0] = toupper((unsigned char)word[0]);
    else
        word[len - 1] = toupper((unsigned char)word[len - 1]);
}

void generatePassphrase(const char *wordlist_path, size_t words) {
    if (words < 3) words = 4;
    load_wordlist(wordlist_path);

    printf("Generated passphrase: ");
    for (size_t i = 0; i < words; i++) {
        unsigned int r;
        randombytes_buf(&r, sizeof(r));
        size_t idx = r % word_count;

        char temp[MAX_WORD_LEN];
        snprintf(temp, sizeof(temp), "%s", wordlist[idx]);
        random_capitalize(temp);

        printf("%s", temp);
        if (i + 1 < words) printf("-");
    }
    printf("\n");
}
