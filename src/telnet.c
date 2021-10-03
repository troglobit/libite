/* Poor mans telnet expect in C.
 *
 * Copyright (c) 2010-2021  Joachim Wiberg <troglobit@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * @file telnet.c
 * @author Joachim Wiberg
 * @date 2010-2021
 * @copyright ISC License
 */

#include <errno.h>
#include <poll.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "lite.h"

#define  EXPECT_DEBUG        0
#define  EXPECT_POLL_TIMEOUT 3000	/* 3 sec timeout waiting for server. */

#if EXPECT_DEBUG
# define TELL(fmt, args...) fprintf (stderr, fmt "\n", ##args)
#else
# define TELL(fmt, args...)
#endif

/** @private for internal use only */
struct sdbuf {
	int   sd;
	char *buf;
};

/**
 * Open telnet connection to addr:port and connect.
 * @param addr  Integer encoded IPv4 address in network byte order
 * @param port  Internet port number in network byte order
 *
 * @returns An @ref sdbuf_t socket buffer context.
 */
sdbuf_t *telnet_open(int addr, short port)
{
	struct sockaddr_in *sin;
	struct sockaddr s;
	sdbuf_t *ctx;
	int saved;
	int retries = 10;

	ctx = (sdbuf_t *)malloc(sizeof(sdbuf_t));
	if (!ctx) {
//		perror("Failed allocating expect ctxect");
		return NULL;
	}

	ctx->buf = (char *)malloc(BUFSIZ);
	if (!ctx->buf) {
//		perror("Failed allocating expect buffer");
		free(ctx);
		return NULL;
	}

	ctx->sd = socket(PF_INET, SOCK_STREAM, 0);
	if (-1 == ctx->sd) {
		saved = errno;
//		perror ("Failed creating socket");
		free(ctx->buf);
		free(ctx);

		errno = saved;
		return NULL;
	}

 retry:
	sin = (struct sockaddr_in *)&s;
	sin->sin_family = AF_INET;
	sin->sin_port = port;
	sin->sin_addr.s_addr = addr;
	if (-1 == connect(ctx->sd, &s, sizeof(s))) {
		if (ETIMEDOUT == errno && retries-- > 0) {
			sleep(1);
			goto retry;
		}

		saved = errno;
//		perror ("Cannot connect to telnet daemon");
		telnet_close(ctx);
		errno = saved;

		return NULL;
	}

	return ctx;
}

/**
 * Close a telnet session previously opened with telnet_open()
 * @param ctx  An @ref sdbuf_t socket buffer context
 *
 * @returns Always returns POSIX OK(0).
 */
int telnet_close(sdbuf_t *ctx)
{
	free(ctx->buf);
	close(ctx->sd);
	free(ctx);

	return 0;
}

static int wait_substr(sdbuf_t *ctx, char *str)
{
	struct pollfd pfd = { ctx->sd, POLLIN, 0 };
	ssize_t len, tot = 0;
	int err;

	while (1) {
		err = poll(&pfd, 1, EXPECT_POLL_TIMEOUT);
		if (err <= 0) {
			if (0 == err)
				errno = ETIMEDOUT;

			if (EINTR == errno)
				continue;

//			perror("No prompt");
			return 1;
		}

		len = read(ctx->sd, &ctx->buf[tot], BUFSIZ);
		if (len <= 0) {
			errno = ENOMSG;
			return 1;
		}

		tot += len;
		ctx->buf[tot] = 0;
		TELL("Got line: '%s' is it '%s'?", ctx->buf, str);

		if (strstr(ctx->buf, str))
			return 0;
	}
}

/**
 * Poor man's telnet expect
 * @param ctx     Telnet session context from telnet_open()
 * @param script  @c NULL terminated list of expect and response strings
 * @param output  Optional output from session
 *
 * Issues @p script sequence on telnet session specified in @p ctx, with
 * optional @p output from session.
 *
 * The @p script consists of strings of expect and response pairs.  For
 * example, expect "ogin: " followed by response "root\n", expect
 * "assword: "with response "secret\n", concluded by @c NULL.
 *
 * @returns POSIX OK(0) or non-zero on error.
 */
int telnet_expect(sdbuf_t *ctx, char *script[], FILE *output)
{
	struct pollfd fd = { ctx->sd, POLLIN, 0 };
	ssize_t len;
	int i, first = 1, cont = 1, result = 0;

	for (i = 0; script[i]; i++) {
		/* when expecting the empty string, just wipe any available input */
		if (!strlen(script[i])) {
			result = poll(&fd, 1, 0);
			if (result > 0)
				lseek(ctx->sd, 0, SEEK_END);
		} else {
			if (wait_substr(ctx, script[i])) {
				result = ENOMSG;
				return 1;
			}
		}

		i++;

		if (strlen(script[i])) {
			TELL("Sending line: '%s'", script[i]);
			if (-1 == write(ctx->sd, script[i], strlen(script[i]))) {
				errno = ENOSPC;
				return 1;
			}
		}
	}

	while (cont) {
		char *ptr = ctx->buf;
		char *tmp;

		result = poll(&fd, 1, EXPECT_POLL_TIMEOUT);
		if (result <= 0) {
			if (0 == result)
				errno = ETIMEDOUT;

			if (EINTR == errno)
				continue;

//			perror ("No result");
			result = errno;
			break;
		}

		/*
		 * On VERY long outputs we need to send some love back to the
		 * server, otherwise it terminates the connection. --Jocke
		 */
		if (-1 == write(ctx->sd, "\n", 1))
			goto retry;

		len = read(ctx->sd, ctx->buf, BUFSIZ);
		if (len == -1) {
		retry:
			if (EINTR == errno)
				continue;

			result = errno;
			break;
		}
		ctx->buf[len] = 0;

		TELL("Read line: '%s'", ctx->buf);

		/* Skip first line -- only an echo of the last command */
		if (first) {
			first = 0;
			ptr = strchr(ctx->buf, '\n');
			if (ptr)
				ptr++;
			else
				ptr = ctx->buf;
		}

		TELL("Comparing '%s' with '%s'", ptr, script[i - 2]);

		/* Skip last line -- only the returning prompt. */
		tmp = strstr(ptr, script[i - 2]);
		if (tmp) {
			TELL("OK, last line.  Good bye!");
			result = 0;
			cont = 0;
			*tmp = 0;

			tmp = strrchr(ptr, '\n');
			if (tmp)
				*++tmp = 0;
		}

		if (output && *ptr) {
			if (EOF == fputs(ptr, output)) {
				/* Only happens when /tmp is full :( */
				result = ENOSPC;
				/* We must complete read-out of descriptor */
				output = NULL;
			}
		}
	}

	return result;
}

/**
 * Very simple expect-like implementation for telnet.
 * @param addr    Integer encoded IPv4 address, in network byte order, use htonl().
 * @param port    Internet port to connect to, in network byte order, use htons().
 * @param script  Expect like script of paired 'expect', 'response' strings.
 * @param output  A FILE pointer to a tempfile() for output from the last command.
 *
 * This is a very simple expect-like implementation for querying and
 * operating daemons remotely over telnet.
 *
 * The @p script is a query-response type of list of strings leading up
 * to a final command, which output is then written to the given @p
 * output file.
 *
 * @returns POSIX OK(0), or non-zero with @a errno set on error.
 */
int telnet_session(int addr, short port, char *script[], FILE *output)
{
	sdbuf_t *sdbuf;
	int result = 0;

	sdbuf = telnet_open(addr, port);
	if (!sdbuf)
		return errno;

	if (telnet_expect(sdbuf, script, output))
		result = errno;

	telnet_close(sdbuf);

	return errno = result;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
