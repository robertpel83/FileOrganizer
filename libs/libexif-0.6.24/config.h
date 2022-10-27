


/* MSVC hacks */
#ifdef _MSC_VER
#define ssize_t int64_t
//#define _CRT_SECURE_NO_WARNINGS
#if _MSC_VER < 1900
#define snprintf(buffer, count, format, ...) _snprintf_s(buffer, count, count, format, __VA_ARGS__)
#endif
#endif

/* Disable gettext */
#undef ENABLE_NLS
#undef HAVE_DCGETTEXT
#undef HAVE_GETTEXT
#undef HAVE_ICONV
#undef ICONV_CONST
#define __WATCOMC__

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
#undef ENABLE_NLS

   /* Define to 1 if you have the Mac OS X function CFLocaleCopyCurrent in the
	  CoreFoundation framework. */
#undef HAVE_CFLOCALECOPYCURRENT

	  /* Define to 1 if you have the Mac OS X function CFPreferencesCopyAppValue in
		 the CoreFoundation framework. */
#undef HAVE_CFPREFERENCESCOPYAPPVALUE

		 /* Define if the GNU dcgettext() function is already present or preinstalled.
			*/
#undef HAVE_DCGETTEXT


			/* Define to 1 if you have the <dlfcn.h> header file. */
#undef HAVE_DLFCN_H

/* Define if the GNU gettext() function is already present or preinstalled. */
#undef HAVE_GETTEXT

/* Define if you have the iconv() function and it works. */
#undef HAVE_ICONV


/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1


/* Define to 1 if you have the `localtime_r' function. */
#undef HAVE_LOCALTIME_R

/* Define to 1 if you have localtime_s() */
#define HAVE_LOCALTIME_S 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#undef HAVE_UNISTD_H

/* Define as const if the declaration of iconv() needs const. */
#undef ICONV_CONST




/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1



/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
#undef _FILE_OFFSET_BITS

/* Define for large files, on AIX-style hosts. */
#undef _LARGE_FILES



#define strdup _strdup
#define strncasecmp _strnicmp
//#define snprintf _snprintf

#include <basetsd.h>
typedef SSIZE_T ssize_t;

/* Define to 1 if translation of program messages to the user's native language is requested. */
#define GETTEXT_PACKAGE "libexif-12"
#define LOCALEDIR ""

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "libexif"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "libexif-devel@lists.sourceforge.net"

/* Define to the full name of this package. */
#define PACKAGE_NAME "EXIF library"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "EXIF library 0.6.24"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libexif"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.6.24"

/* Version number of package */
#define VERSION "0.6.24"

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif