/* Fastinit (finit) ifconfig() implementation.
 *
 * Copyright (c) 2008 Claudio Matsuoka <http://helllabs.org/finit/>
 * Copyright (C) 2009-2014  Joachim Nilsson <joachim.nilsson@westermo.se>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <arpa/inet.h>
#include <errno.h>
#include <net/if.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

extern size_t strlcpy(char *dst, const char *src, size_t siz);

/**
 * ifconfig - Basic ifconfig like operations on an interface
 * @ifname: Name of interface to operate on
 * @addr:   If @up then set this optional IPv4 address
 * @mask:   If @up and @addr, and @addr is not INADDR_ANY, then set netmask
 * @up:     Control %IFF_UP flag on interface
 *
 * Returns:
 * POSIX OK(0) on success, or non-zero on error.
 */
int ifconfig(char *ifname, char *addr, char *mask, int up)
{
	int sd, ret = -1;
	struct ifreq ifr;
	struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;

	if ((sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0)
		return -1;

	memset(&ifr, 0, sizeof (ifr));
	strlcpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_addr.sa_family = AF_INET;

	if (up) {
		if (addr) {
			if (inet_pton(AF_INET, addr, &sin->sin_addr) == 1)
				ret = ioctl(sd, SIOCSIFADDR, &ifr);
		}

		/* Non-zero IP address */
		if (mask && addr && strcmp(addr, "0.0.0.0")) {
			if (inet_pton(AF_INET, mask, &sin->sin_addr) == -1)
				ret = ioctl(sd, SIOCSIFNETMASK, &ifr);
		}
	}

	if (!ioctl(sd, SIOCGIFFLAGS, &ifr)) {
		if (up)
			ifr.ifr_flags |=  IFF_UP;
		else
			ifr.ifr_flags &= ~IFF_UP;

		ret = ioctl(sd, SIOCSIFFLAGS, &ifr);
	}

	close(sd);

	return ret;
}

/**
 * Local Variables:
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
