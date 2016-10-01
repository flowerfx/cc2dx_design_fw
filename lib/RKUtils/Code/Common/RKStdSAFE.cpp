//***********************************************************************************************************************************
//*
//* RKStdSAFE.h
//*
//* a multi platform alternative to standard unsafe functions, like sprintf, strcpy, etc ..
//*
//* [Christophe Le Bouil]
//*
//***********************************************************************************************************************************

#include <string.h>

#ifdef GAME_ANDROID
	extern "C"
	{
		char* ecvt(double, int, int*, int*) {
			return "<some float, formatting not implemented for android>";
		}
	}
#endif

	//-------------------------------------- 
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4996 )   // warning C4996: 'strncpy': This function or variable may be unsafe...
#endif

#ifdef __cplusplus
	extern "C" {
#endif

		char *strcpy_REALLY(char *destBuffer, const char *source);
		char *strncpy_REALLY(char *destBuffer, const char *source, size_t numBytesToCopy);
		char *strcat_REALLY(char *destBuffer, const char *source);

#ifdef __cplusplus
	}
#endif

	//---------------------------------------------------------------------------------------------------------------------------------
	char *strncpy_REALLY(char *destBuffer, const char *source, size_t numBytesToCopy)
		//---------------------------------------------------------------------------------------------------------------------------------
	{
		return strncpy(destBuffer, source, numBytesToCopy);

		//+ TODO : check if destBuffer is in the heap -> check
		// or check if destBuffer is in the "DATA" section -> other checks
	}


	//---------------------------------------------------------------------------------------------------------------------------------
	char *strcpy_REALLY(char *destBuffer, const char *source)
		//---------------------------------------------------------------------------------------------------------------------------------
	{
		return strcpy(destBuffer, source);
	}


	//---------------------------------------------------------------------------------------------------------------------------------
	char *strcat_REALLY(char *destBuffer, const char *source)
		//---------------------------------------------------------------------------------------------------------------------------------
	{
		return strcat(destBuffer, source);
	}






#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>  //isdigit
#include <math.h>

#include "RKAssert.h"
#include "RKStdSAFE.h"

	/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#ifndef __dj_include_values_h_
#define __dj_include_values_h_

#ifdef __cplusplus
	extern "C" {
#endif

#ifndef __dj_ENFORCE_ANSI_FREESTANDING

#ifndef __STRICT_ANSI__

#ifndef _POSIX_SOURCE

#define _IEEE       0
#define _DEXPLEN    11
#define _FEXPLEN    8
		//#define DMAXEXP     ((1 << _DEXPLEN - 1) - 1 + _IEEE)
#define DMAXEXP     ((1 << (_DEXPLEN - 1)) - 1 + _IEEE) // : 1023
		//#define FMAXEXP     ((1 << _FEXPLEN - 1) - 1 + _IEEE)
#define DMINEXP     (-DMAXEXP)
		//#define FMINEXP     (-FMAXEXP)

#endif /* !_POSIX_SOURCE */
#endif /* !__STRICT_ANSI__ */
#endif /* !__dj_ENFORCE_ANSI_FREESTANDING */

#ifndef __dj_ENFORCE_FUNCTION_CALLS
#endif /* !__dj_ENFORCE_FUNCTION_CALLS */

#ifdef __cplusplus
	}
#endif

#endif /* !__dj_include_values_h_ */

#ifndef OS_W10
	extern double	modf(double, double*);			//extern double	modf();
#endif
	

	namespace RKUtils
	{
	extern "C" int my_vsnprintf(char *buf, int nbytes, const char *fmt, va_list args);
	//----------------------------------------------------------------------------------------------------------------------
	int vsnprintf_SAFE(char *buf, int nbytes, const char *fmt, va_list args)
		//----------------------------------------------------------------------------------------------------------------------
	{
		// calls our own C function version of vsnprintf found in " string_snprintf.c "
		return my_vsnprintf(buf, nbytes, fmt, args);
	}


	//--------------------------------------------------------------------------------------------------------------------------------------
	int sprintf_SAFE(char *destBuffer, int sizeOfDestBuffer, const char *format, ...)
		//--------------------------------------------------------------------------------------------------------------------------------------
	{
		va_list	args;
		int	ret;

		va_start(args, format);
		ret = my_vsnprintf(destBuffer, sizeOfDestBuffer, format, args);
		va_end(args);

		RKWarning(ret >= 0, "sprintf_SAFE buffer size=%d : Was Truncated! : <%s>", sizeOfDestBuffer, destBuffer);

		return ret;
	}


	//--------------------------------------------------------------------------------------------------------------------------------------
	int sprintfcat_SAFE(char *destBuffer, int sizeofDestBuffer, const char *format, ...)
		//--------------------------------------------------------------------------------------------------------------------------------------
	{
		int destPos = strlen(destBuffer);

		va_list	args;
		int	ret;

		va_start(args, format);
		ret = my_vsnprintf(destBuffer + destPos, sizeofDestBuffer - destPos - 1, format, args);
		va_end(args);
		return ret;
	}


	//--------------------------------------------------------------------------------------------------------------------------------------
	char *strncpy_SAFE(char *destBuffer, int sizeofDestBuffer, const char *source, size_t numBytesToCopy)
		//--------------------------------------------------------------------------------------------------------------------------------------
	{
		int nbCopy = numBytesToCopy;
		if (nbCopy >= sizeofDestBuffer)
		{
			RKWarning(nbCopy <= sizeofDestBuffer, "strncpy_SAFE: nbCopy=%d sizeofDestBuffer=%d source=%s", nbCopy, sizeofDestBuffer, source);
			nbCopy = sizeofDestBuffer;
		}
		return strncpy_REALLY(destBuffer, source, nbCopy);
	}

	//--------------------------------------------------------------------------------------------------------------------------------------
	char *strcpy_SAFE(char *destBuffer, int sizeofDestBuffer, const char *source)
		//--------------------------------------------------------------------------------------------------------------------------------------
	{
		if (source == NULL)
		{
			//mm should MLAssert
			*destBuffer = 0;
			return destBuffer;
		}
		int nbCopy = strlen(source);
		if (!(nbCopy <= sizeofDestBuffer - 1))
		{
			RKWarning(nbCopy <= sizeofDestBuffer - 1, "strcpy_SAFE: nbCopy=%d sizeofDestBuffer=%d source=%s", nbCopy, sizeofDestBuffer, source);
			nbCopy = sizeofDestBuffer - 1;
			strncpy_REALLY(destBuffer, source, nbCopy);
			destBuffer[nbCopy] = 0;
			return destBuffer;
		}
		return strcpy_REALLY(destBuffer, source);
	}

	//--------------------------------------------------------------------------------------------------------------------------------------
	char *strcat_SAFE(char *destBuffer, int sizeofDestBuffer, const char *source)
		//--------------------------------------------------------------------------------------------------------------------------------------
	{
		int curSrcLen = strlen(destBuffer);
		int maxToCopy = sizeofDestBuffer - curSrcLen;
		int nbCopy = strlen(source);

		if (!(nbCopy <= maxToCopy - 1))
		{
			RKWarning(nbCopy <= maxToCopy - 1, "strcat_SAFE(): nbCopy=%d maxToCopy=%d source=%s", nbCopy, maxToCopy, source);
			nbCopy = maxToCopy;
			return strncpy_REALLY(destBuffer, source, nbCopy);
		}
		return strcpy_REALLY(destBuffer + curSrcLen, source);
	}



	//--------------------------------------------------------------------------------------------------------------------------------------
	//char *itoa_SAFE(int val, char *dest, int sizeOfDest, int radix)
	//--------------------------------------------------------------------------------------------------------------------------------------
	/*
	{
	#if _MSC_VER >= 1500	//VC 2008"
	//mmm should return the buffer !!#define itoa_SAFE(val, dest, sizeofDestBuffer, radix) _itoa_s(val, dest, sizeofDestBuffer, radix)
	//char *itoa_SAFE(int val, char *dest, int sizeOfDest, int radix);
	//#define itoa_SAFE(val, dest, sizeofDestBuffer, radix) _itoa_s(val, dest, sizeofDestBuffer, radix)
	int err = _itoa_s(val, dest, sizeOfDest, radix);
	return dest;

	#else	//VC6 ...
	//#define itoa_SAFE(val, dest, sizeofDestBuffer, radix) itoa(val, dest, radix)		//TODO !
	return itoa(val, dest, radix);		//TODO !
	#endif
	}
	*/











	//****************************************************************************************************************************************************************
	//*
	//*  from string_snprintf.c
	//*
	//****************************************************************************************************************************************************************
	int my_vsnprintf(char *buf, int nbytes, const char *fmt, va_list args);

	typedef struct string_str_forStdSafe StringForStdSafe; // String;
	struct string_str_forStdSafe {
		char	*s_buf;		/* current position in buffer */
		int	s_nleft;	/* number of bytes left */
	};


	typedef int	sn_write_TYPE(char *buf, int size, int num, StringForStdSafe *s);
	static int new_doprnt(const char *fmt, va_list args, sn_write_TYPE *func, void *arg);

	/*
	 *	A safe sprintf() - returns the length of buf or -1 if the buffer
	 *  was not big enough (always writes as much as it can).
	 */
	int snprintf(char *buf, int nbytes, const char *fmt, ...);
	int snprintf(char *buf, int nbytes, const char *fmt, ...)
	{
		va_list	args;
		int	ret;

		va_start(args, fmt);
		ret = my_vsnprintf(buf, nbytes, fmt, args);
		va_end(args);

		return ret;
	}

#if 0
	typedef struct string_str_forStdSafe StringForStdSafe;
	struct string_str_forStdSafe {
		char	*s_buf;		/* current position in buffer */
		int	s_nleft;	/* number of bytes left */
	};
#endif

	static int	sn_write(char *buf, int size, int num, StringForStdSafe *s);

	/*
	 *	A safe vsprintf() - returns the length of buf or -1 if the buffer
	 *  was not big enough (always writes as much as it can).
	 */
	int my_vsnprintf(char *buf, int nbytes, const char *fmt, va_list args)
	{
		StringForStdSafe	s;
		int	ret;

		s.s_buf = buf;
		s.s_nleft = nbytes - 1;
		ret = new_doprnt(fmt, args, sn_write, (void *)&s);
		*s.s_buf = '\0';

		return ret;
	}

	static int sn_write(char *buf, int size, int num, StringForStdSafe *s)
	{
		size *= num;
		if (size > s->s_nleft) {
			size = s->s_nleft;
			num = -1;
		}

		if (size > 0) {
			(void)memcpy(s->s_buf, buf, (unsigned long)size);
			s->s_buf += size;
			s->s_nleft -= size;
		}

		return num;
	}

	/*
	 *	New_doprnt() is a routine similar to _doprnt() except it takes
	 *  different arguments.  Should be fairly portable (excpet the floating
	 *  point stuff which needs frexp(), modf() and ecvt()).  Has been used
	 *  on BSD and USG systems, as well as on an atari.
	 *
	 *  Permission is granted to do whatever you want with this code as long as
	 *  you send me any improvements.
	 *				Mike Rendell
	 *
	 *  Bug fixes/reports to michael@garfield.mun.edu or ..!uunet!garfield!michael
	 *
	 *  Wish list:
	 *  - it would be nice to have a way to extend the format characters used -
	 *  V9 is rumered to have a way of doing this but I have no idea what the
	 *  interface is.
	 *  - cleanup the floating point stuff
	 *
	 * Revision log:
	 * 89/03/24  10:16:48  michael
	 *	- now assumes func is exactly like fwrite() in that it returns the
	 *	number of bytes written, or < 0 on error.  new_doprnt() returns
	 *	as soon as func().  the total number of bytes written (according
	 *	to the return value of func()) is returned.
	 *	- fixed problem with %g and exp calculation
	 *
	 * 89/03/23  20:04:12  michael
	 *	- now handles %efg formats.
	 *
	 * 89/01/22  17:16:19  michael
	 *	- now calls a fwrite() like function instead of a putchar like one.
	 *	- Fixed several compatability problems with formatting: %#08x now
	 *	works, % #d works, %03s where s starts with +, -, ' ', or 0x now works.
	 *
	 * 88/08/16  15:43:14  michael
	 *	- Fixed problem with %#[xo]
	 *
	 * 88/02/15  22:22:38  m68k
	 *	- Added check for null %s's - now prints (long) warning message
	 */

#include	<limits.h>
#include	<float.h>


#ifndef BITS
# define BITS(t)	(CHAR_BIT * sizeof(t))
#endif /* BITS */

#define FP  			/* if you want floating point stuff */

#define BUF_SIZE	128
#define FPBUF_SIZE	((DMAXEXP)+16)/* this must be >
	 *	MAX(DMAXEXP, log10(pow(2, DSIGNIF)))
	 *    + ceil(log10(DMAXEXP)) + 8 (I think).
	 * Since this is hard to express as a
	 * constant, just use a large buffer.
	 */

	/*
	 *	What kinda of machine we on?  Hopefully the C compiler will optimize
	 *  this out...
	 *
	 *	For shorts, we want sign extend for %d but not for %[oxu] - on 16 bit
	 *  machines it don't matter.  Assmumes C compiler has converted shorts to
	 *  ints before pushing them.
	 */
#define POP_INT(f, x, a) (((f) & FL_LONG) ?				\
				va_arg((a), unsigned long)		\
			    :						\
				(sizeof(int) < sizeof(long) ?		\
					((x) ?				\
						(long) va_arg((a), int)	\
					    :				\
						va_arg((a), unsigned))	\
				    :					\
					va_arg((a), unsigned)))

#define ABIGNUM		32000	/* big numer that will fit in a short */
#define LOG2_10		3.321928094887362347870319429	/* log base 2 of 10 */

#define	FL_HASH		0x01	/* `#' seen */
#define FL_PLUS		0x02	/* `+' seen */
#define FL_RIGHT	0x04	/* `-' seen */
#define FL_BLANK	0x008	/* ` ' seen */
#define FL_SHORT	0x010	/* `h' seen */
#define FL_LONG		0x020	/* `l' seen */
#define FL_ZERO		0x040	/* `0' seen */
#define FL_DOT		0x080	/* '.' seen */
#define FL_UPPER	0x100	/* format character was uppercase */
#define FL_NUMBER	0x200	/* a number was formated %[douxefg] */


	/*
	 *	Used to stuff a character into a buffer - when the buffer fills, it
	 *  will be flushed.  Nasty as it knows the names of local variables.
	 */
#define STUFFC(c)	do {						\
				if (--bufcnt < 0) {			\
					int	ret;			\
									\
					if ((ret = (*func)(buf, 1, 	\
						   BUF_SIZE, (StringForStdSafe*)arg)) < 0)	\
						return ret;		\
					nwritten += ret;		\
					bufcnt = BUF_SIZE - 1;		\
					bufp = buf;			\
								}					\
				*bufp++ = (c);				\
				} while (0)

#ifdef FP
	/*static double
	my_ceil(d)
	double	d;*/
	static double my_ceil(double d)
	{
		double		i;

		return d - modf(d, &i) + (d < 0 ? -1 : 1);
	}
#endif /* FP */


	/*
	 #define _IEEE_tst       0
	 #define _DEXPLEN_tst    11
	 #define DMAXEXP_tst     ((1 << _DEXPLEN_tst - 1) - 1 + _IEEE_tst)
	 #define DMAXEXP _tst2    ((1 << (_DEXPLEN_tst - 1)) - 1 + _IEEE_tst) // : 1023

	 char gBufTst[DMAXEXP_tst];		//1023
	 char gBufTst2[DMAXEXP_tst2];		//2046

	 void __fixTst(void)
	 {
	 }
	 */


#ifndef DMAXEXP  //mmm iphone, gcc 4.2 ...
#define _IEEE       0
#define _DEXPLEN    11
#define DMAXEXP     ((1 << (_DEXPLEN - 1)) - 1 + _IEEE) // : 1023
#endif

	static int new_doprnt(const char *fmt, va_list args, sn_write_TYPE *func, void *arg);

	//static int new_doprnt(const char *fmt, va_list args, int (*func)(), void *arg)
	static int new_doprnt(const char *fmt, va_list args, sn_write_TYPE *func, void *arg)
		/* func is an fwrite() like routine */
		/* arg is the "stream" arg for fwrite() */
	{
		char		c, *s, *p;
		int		tmp = 0;
		int		field, precision;
		int		len;
		int		flags;
		unsigned long	lnum;
		/* %#o produces the longest output */
		char		numbuf[(BITS(long) + 2) / 3 + 1];
		/* this stuff for dealing with the buffer */
		char		buf[BUF_SIZE];
		char		*bufp = buf;
		int		bufcnt = BUF_SIZE;	/* # free chars left in buf */
		int		nwritten = 0;
#ifdef FP
		//C++!! extern double	frexp();
		//C++!! extern char	*ecvt();      /// + warning C4996: 'ecvt': The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: _ecvt.

		double		fpnum;
		int		exp, decpt;
		char		style;
		char		fpbuf[(FPBUF_SIZE)];
#endif /* FP */

		if (!fmt)
			return 0;

		while ((c = *fmt++)) {
			if (c != '%') {
				STUFFC(c);
				continue;
			}
			/*
			 *	This will accept flags/fields in any order - not
			 *  just the order specified in printf(3), but this is
			 *  the way _doprnt() seems to work (on bsd and sysV).
			 *  The only resriction is that the format character must
			 *  come last :-).
			 */
			flags = field = precision = 0;
			for (; (c = *fmt++);) {
				switch (c) {
				case '#':
					flags |= FL_HASH;
					continue;

				case '+':
					flags |= FL_PLUS;
					continue;

				case '-':
					flags |= FL_RIGHT;
					continue;

				case ' ':
					flags |= FL_BLANK;
					continue;

				case '0':
					if (!(flags & FL_DOT))
						flags |= FL_ZERO;
					continue;

				case '.':
					flags |= FL_DOT;
					precision = 0;
					continue;

				case '*':
					tmp = va_arg(args, int);
					if (flags & FL_DOT)
						precision = tmp;
					else if ((field = tmp) < 0) {
						field = -field;
						flags |= FL_RIGHT;
					}
					continue;

				case 'l':
					flags |= FL_LONG;
					continue;

				case 'h':
					flags |= FL_SHORT;
					continue;
				}
				if (isdigit(c)) {
					tmp = c - '0';
					while (c = *fmt++, isdigit(c))
						tmp = tmp * 10 + c - '0';
					--fmt;
					if (tmp < 0)		/* overflow? */
						tmp = 0;
					if (flags & FL_DOT)
						precision = tmp;
					else
						field = tmp;
					continue;
				}
				break;
			}

			if (precision < 0)
				precision = 0;

			if (!c)		/* nasty format */
				break;

			if (isupper(c)) {
				flags |= FL_UPPER;
				c = tolower(c);
			}

			switch (c) {
			case 'p': /* pointer */
				flags &= ~(FL_LONG | FL_SHORT);
				if (sizeof(char *) > sizeof(int))
					flags |= FL_LONG; /* hope it fits.. */
				/* aaahhh... */
			case 'd':
			case 'i':
			case 'o':
			case 'u':
			case 'x':
				flags |= FL_NUMBER;
				s = &numbuf[sizeof(numbuf)];
				lnum = POP_INT(flags, c == 'd', args);
				switch (c) {
				case 'd':
				case 'i':
					if (0 > (long)lnum)
						lnum = -(long)lnum, tmp = 1;
					else
						tmp = 0;
					/* aaahhhh..... */

				case 'u':
					do {
						*--s = (char)(lnum % 10 + '0');					//warning C4244: '=' : conversion from 'unsigned long ' to 'char ', possible loss of data
						lnum /= 10;
					} while (lnum);

					if (c != 'u') {
						if (tmp)
							*--s = '-';
						else if (flags & FL_PLUS)
							*--s = '+';
						else if (flags & FL_BLANK)
							*--s = ' ';
					}
					break;

				case 'o':
					do {
						*--s = (char)((lnum & 0x7) + '0');				//warning C4244: '=' : conversion from 'unsigned long ' to 'char ', possible loss of data
						lnum >>= 3;
					} while (lnum);

					if ((flags & FL_HASH) && *s != '0')
						*--s = '0';
					break;

				case 'p':
				case 'x':
					p = (flags & FL_UPPER) ? (char*)"0123456789ABCDEF" :
						(char*)"0123456789abcdef"; // TODO: fix these ugly casts, most likely there is a bug in there
					do {
						*--s = p[lnum & 0xf];
						lnum >>= 4;
					} while (lnum);

					if (flags & FL_HASH) {
						*--s = (flags & FL_UPPER) ? 'X' : 'x';
						*--s = '0';
					}
				}
				len = &numbuf[sizeof(numbuf)] - s;
				if (flags & FL_DOT) {
					if (precision > len) {
						field = precision;
						flags |= FL_ZERO;
					}
					else
						precision = len; /* no loss */
				}
				break;

#ifdef FP
			case 'e':
			case 'g':
			case 'f':
				/*
				 *	This could proabably be done better,
				 *  but it seems to work.  Note that gcvt()
				 *  is not used, as you cannot tell it to
				 *  not strip the zeros.
				 */
				flags |= FL_NUMBER;
				if (!(flags & FL_DOT))
					precision = 6;	/* default */
				/*
				 *	Assumes doubles are pushed on
				 *  the stack.  If this is not so, then
				 *  FL_LONG/FL_SHORT should be checked.
				 */
				fpnum = va_arg(args, double);
				s = fpbuf;
				style = c;
				/*
				 *  This is the same as
				 *	exp = ceil(log10(fpnum))
				 *  but doesn't need -lm.  This is an
				 *  aproximation as exp is rounded up.
				 */
				(void)frexp(fpnum, &exp);
				exp = (int)(my_ceil(exp / LOG2_10));			//warning C4244: '=' : conversion from 'double ' to 'int ', possible loss of data

				if (exp < 0)
					exp = 0;

				p = ecvt(fpnum, precision + 1 + exp, &decpt, &tmp);
				if (c == 'g') {
					if (decpt < -4 || decpt > precision)
						style = 'e';
					else
						style = 'f';
					if (decpt > 0 &&
						(precision -= decpt) < 0)
						precision = 0;
				}
				if (tmp)
					*s++ = '-';
				else if (flags & FL_PLUS)
					*s++ = '+';
				else if (flags & FL_BLANK)
					*s++ = ' ';

				if (style == 'e')
					*s++ = *p++;
				else {
					if (decpt > 0) {
						/* Overflow check - should
						 * never have this problem.
						 */
						if (decpt >
							&fpbuf[sizeof(fpbuf)]
							- s - 8)
							decpt =
							&fpbuf[sizeof(fpbuf)]
							- s - 8;
						(void)memcpy(s, p, decpt);
						s += decpt;
						p += decpt;
					}
					else
						*s++ = '0';
				}

				/* print the fraction? */
				if (precision > 0) {
					*s++ = '.';
					/* Overflow check - should
					 * never have this problem.
					 */
					if (precision > &fpbuf[sizeof(fpbuf)]
						- s - 7)
						precision =
						&fpbuf[sizeof(fpbuf)]
						- s - 7;
					for (tmp = decpt;  tmp++ < 0 &&
						precision > 0; precision--)
						*s++ = '0';
					tmp = strlen(p);
					if (precision > tmp)
						precision = tmp;
					/* Overflow check - should
					 * never have this problem.
					 */
					if (precision > &fpbuf[sizeof(fpbuf)]
						- s - 7)
						precision =
						&fpbuf[sizeof(fpbuf)]
						- s - 7;
					(void)memcpy(s, p, precision);
					s += precision;
					/*
					 *	`g' format strips trailing
					 *  zeros after the decimal.
					 */
					if (c == 'g' && !(flags & FL_HASH)) {
						while (*--s == '0')
							;
						if (*s != '.')
							s++;
					}
				}
				else if (flags & FL_HASH)
					*s++ = '.';

				if (style == 'e') {
					*s++ = (flags & FL_UPPER) ? 'E' : 'e';
					if (--decpt >= 0)
						*s++ = '+';
					else {
						*s++ = '-';
						decpt = -decpt;
					}
					p = &numbuf[sizeof(numbuf)];
					for (tmp = 0; tmp < 2 || decpt; tmp++) {
						*--p = '0' + decpt % 10;
						decpt /= 10;
					}
					tmp = &numbuf[sizeof(numbuf)] - p;
					(void)memcpy(s, p, tmp);
					s += tmp;
				}

				len = s - fpbuf;
				s = fpbuf;
				precision = len;
				break;
#endif /* FP */

			case 's':
				if (!(s = va_arg(args, char *)))
					//s = "(null %s)";		//BAD : if printf, will try to decode this '%s' !
					s = (char*)"(null !s!)";
				len = strlen(s);
				break;

			case 'c':
				flags &= ~FL_DOT;
				numbuf[0] = va_arg(args, int);
				s = numbuf;
				len = 1;
				break;

			case '%':
			default:
				numbuf[0] = c;
				s = numbuf;
				len = 1;
				break;
			}

			/*
			 *	At this point s should point to a string that is
			 *  to be formatted, and len should be the length of the
			 *  string.
			 */
			if (!(flags & FL_DOT) || len < precision)
				precision = len;
			if (field > precision) {
				field -= precision;
				if (!(flags & FL_RIGHT)) {
					field = -field;
					/* skip past sign or 0x when padding with 0 */
					if ((flags & FL_ZERO) && (flags & FL_NUMBER)) {
						if (*s == '+' || *s == '-' || *s == ' ')
						{
							STUFFC(*s);
							s++;
							precision--;
						}
						else if (*s == '0') {
							STUFFC(*s);
							s++;
							if (--precision > 0 &&
								(*s | 0x20) == 'x')
							{
								STUFFC(*s);
								s++;
								precision--;
							}
						}
						c = '0';
					}
					else
						c = flags & FL_ZERO ? '0' : ' ';
					for (; field < 0; field++)
						STUFFC(c);
				}
				else
					c = ' ';
			}
			else
				field = 0;

			for (; precision-- > 0; s++)
				STUFFC(*s);
			for (; field > 0; --field)
				STUFFC(c);
		}
		if ((tmp = (*func)(buf, 1, BUF_SIZE - bufcnt, (StringForStdSafe*)arg)) < 0)
			return tmp;

		return nwritten + tmp;
	}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
}

