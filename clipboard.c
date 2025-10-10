#include "vault.h"

const char *get_home_dir()
{
#if defined(_WIN32)
    return getenv("USERPROFILE");
#else
    return getenv("HOME");
#endif
}

const char *get_yoyo_path()
{
    static char fullpath[512];
    const char *home = get_home_dir();
    if (!home)
        return NULL;
    snprintf(fullpath, sizeof(fullpath), "%s%s%s", home, PATH_SEP, YOYO_VAULT);
    return fullpath;
}

void copy_to_clipboard(const char *text, int timeDown)
{
#if defined(__linux)
    {
        FILE *pipe = popen("xclip -selection clipboard", "w");
        if (pipe)
        {
            fputs(text, pipe);
            pclose(pipe);
        }
        printf("Password copied. Will clear in %d seconds.\n", timeDown);

        if (fork() == 0)
        {
            sleep(timeDown);
            FILE *clr = popen("xclip -selection clipboard", "w");
            if (clr)
            {
                fputs("", clr);
                pclose(clr);
            }
            printf("Clipboard cleared.\n");
            _exit(0);
        }
    }

#elif defined(__APPLE__)
    {
        FILE *PIPE = popen("pbcopy", "w");
        if (pipe)
        {
            fputs(text, pipe);
            pclose(pipe);
        }
        printf("Password copied. Will clear in %d seconds.\n", timeDown);

        if (fork() == 0)
        {
            sleep(timeDown);
            FILE *clr = popen("pbcopy", "w");
            if (clr)
            {
                fputs("", clr);
                pclose(clr);
            }
            printf("Clipboard cleared.\n");
            _exit(0);
        }
    }

#elif define(_WIN32)
    {
        if (OpenClipboard(NULL))
        {
            EmptyClipboard();
            HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, strlen(text) + 1);
            memcpy(GlobalLock(hMem), text, strlen(text) + 1);
            GlobalUnlock(hMem);
            SetClipboardData(CF_TEXT, hMem);
            CloseClipboard();
        }
        printf("Password copied. Will clear in %d seconds.\n", timeOut);

    }

#else
#error "unsupported platform"

#endif
}
