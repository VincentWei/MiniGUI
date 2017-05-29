/* 
** $Id: errorlog.c 10660 2008-08-14 09:30:39Z weiym $
**
** errorlog.c: Error routines for programs that can run as a daemon.
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** All rights reserved by Feynman Software.
** 
** Some code come from APUE by Richard Stevens.
**
** Current maintainer: Wei Yongming.
** Create date: 2000.12.31
*/

#include	<stdarg.h>		/* ANSI C header file */
#include	<errno.h>		/* for definition of errno */

#include	"common.h"

#ifdef _MGRM_PROCESSES

#include	<syslog.h>

#ifdef _DEBUG
#include	"ourhdr.h"

static void	log_doit(int, int, const char *, va_list ap);

static int debug = 0;	/* caller must define and set this:
						   nonzero if interactive, zero if daemon */

/* Initialize syslog(), if running as daemon. */

void
log_open(const char *ident, int option, int facility)
{
	if (debug == 0)
		openlog(ident, option, facility);
}

/* Nonfatal error related to a system call.
 * Print a message with the system's errno value and return. */

void
log_ret(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	log_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	return;
}

/* Fatal error related to a system call.
 * Print a message and terminate. */

void
log_sys(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	log_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(2);
}

/* Nonfatal error unrelated to a system call.
 * Print a message and return. */

void
log_msg(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	log_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
	return;
}

/* Fatal error unrelated to a system call.
 * Print a message and terminate. */

void
log_quit(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	log_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(2);
}

/* Print a message and return to caller.
 * Caller specifies "errnoflag" and "priority". */

static void
log_doit(int errnoflag, int priority, const char *fmt, va_list ap)
{
	int		errno_save;
	char	buf[MAXLINE];

	errno_save = errno;		/* value caller might want printed */
	vsprintf(buf, fmt, ap);
	if (errnoflag)
		sprintf(buf+strlen(buf), ": %s", strerror(errno_save));
	strcat(buf, "\n");
	if (debug) {
		fflush(stdout);
		fputs(buf, stderr);
		fflush(stderr);
	} else
		syslog(priority, buf);
	return;
}

#endif /* _DEBUG */
#endif 

