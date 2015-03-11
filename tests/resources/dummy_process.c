#define _GNU_SOURCE

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error_en(en, msg) \
    do { errno = en; handle_error(msg); } while (0)

#define do_safe(cmd, ...) \
    do { const int ret = cmd(__VA_ARGS__); if (ret) handle_error(#cmd); } while (0)

int main(void)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGTERM);
    sigaddset(&set, SIGINT);
    do_safe(sigprocmask, SIG_BLOCK, &set, NULL);
    for (;;)
    {
        int sig;
        do_safe(sigwait, &set, &sig);
        const char *signame = strsignal(sig);
        if (!signame)
            signame = "Unknown signal";
        printf("%d: %s\n", sig, signame);
        if (sigismember(&set, sig))
            return 0;
        else
            return 1;
    }
    return 0;
}
