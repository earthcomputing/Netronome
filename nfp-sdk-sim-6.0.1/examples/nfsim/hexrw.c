/*
 * Copyright (C) 2010,  Netronome Systems, Inc.  All rights reserved.
 *
 * Helper functions for read/dumping hex numbers
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>

struct read_word_state {
    char buff[1+16+3];
    int eob;
};

static void read_word(struct read_word_state *state, int c, int out_fd)
{
    int err;
    if (c == 0 || isspace(c)) {
        if (state->eob != 0) {
            uint32_t val;
            state->buff[state->eob] = 0;
            val = strtoull(state->buff, NULL, 0);
            err = write(out_fd, &val, sizeof(val));
            if (err == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
            state->eob = 0;
        }
    } else {
        state->buff[state->eob++] = c;
        if (state->eob == sizeof(state->buff)) {
            state->eob = 0;
        }
    }
}

int hexread(int in_fd, pid_t *pidp)
{
    pid_t pid;
    int pipe_fd[2], err;

    err = pipe(pipe_fd);
    if (err < 0)
        return -errno;

    pid = fork();
    if (pid < 0) {
        return -errno;
    }

    if (pid == 0) {
        struct read_word_state state = {};
        char c;

        /* Child */
        close(pipe_fd[0]);
        while (read(in_fd, &c, 1) == 1) {
            read_word(&state, c, pipe_fd[1]);
        }
        read_word(&state, 0, 1);
        exit(EXIT_FAILURE);
    }
    close(in_fd);
    close(pipe_fd[1]);

    *pidp = pid;
    return pipe_fd[0];
}

int hexdump(int out_fd, pid_t *pidp, int view_all, uint64_t offset,
            unsigned int chunk)
{
    pid_t pid;
    int pipe_fd[2], err;

    if (offset & (chunk - 1))
        return -EINVAL;

    err = pipe(pipe_fd);
    if (err == -1)
        return -errno;

    pid = fork();
    if (pid < 0) {
        return -errno;
    }

    if (pid == 0) {
        uint8_t last_line[16];
        uint8_t this_line[16];
        int i, err;
        int last_line_valid = 0;

        /* Child */
        dup2(pipe_fd[0], 0);
        if (out_fd != 1) {
            dup2(out_fd, 1);
            close(out_fd);
        }
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        i = (offset & 0xf);
        while (1) {
            err = read(0, &this_line[i], chunk);
            if (err == chunk)
                i += chunk;

            if (err == 0 && i == 0)
                break;

            if ((err != chunk) || (i == 16)) {
                int j;

                if (last_line_valid && (i == 16) &&
                    ( memcmp(last_line, this_line, 16) == 0)) {
                    if (last_line_valid == 1)
                        printf("*\n");
                    offset = (offset & ~0xf) + 16;
                    last_line_valid++;
                    i = 0;
                    continue;
                }

                printf("%010"PRIx64":", (uint64_t)(offset & ~0xf));
                printf(" ");
                for (j = 0; j < (offset & 0xf); j += chunk)
                    printf(" %*c", 2 * chunk, ' ');
                for (; j < i; j += chunk) {
                    int k;
                    printf(" ");
                    for (k = 0; k < chunk; k++) {
                        printf("%02x", this_line[j + chunk - (k + 1)]);
                    }
                }
                for (; j < 16; j += chunk)
                    printf(" %*c", 2 * chunk, ' ');
                printf("\n");

                if (!view_all) {
                    memcpy(last_line, this_line, 16);
                    last_line_valid = 1;
                }
                offset = (offset & ~0xf) + 16;
                i = 0;

                if (err != chunk)
                    break;
            }
        }
        exit((err != 0) ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    close(pipe_fd[0]);

    *pidp = pid;
    return pipe_fd[1];
}
