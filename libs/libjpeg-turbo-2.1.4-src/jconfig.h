/* Version ID for the JPEG library.
 * Might be useful for tests like "#if JPEG_LIB_VERSION >= 60".
 */
#define JPEG_LIB_VERSION  @JPEG_LIB_VERSION@

/* libjpeg-turbo version */
#define LIBJPEG_TURBO_VERSION  @VERSION@

/* libjpeg-turbo version in integer form */
#define LIBJPEG_TURBO_VERSION_NUMBER  @LIBJPEG_TURBO_VERSION_NUMBER@

/* Support arithmetic encoding *///cmakedefine
#define C_ARITH_CODING_SUPPORTED 1

/* Support arithmetic decoding *///cmakedefine
#define D_ARITH_CODING_SUPPORTED 1

/* Support in-memory source/destination managers *///cmakedefine
#define MEM_SRCDST_SUPPORTED 1

/* Use accelerated SIMD routines. *///cmakedefine
#define WITH_SIMD 1

/*
 * Define BITS_IN_JSAMPLE as either
 *   8   for 8-bit sample values (the usual setting)
 *   12  for 12-bit sample values
 * Only 8 and 12 are legal data precisions for lossy JPEG according to the
 * JPEG standard, and the IJG code does not support anything else!
 * We do not support run-time selection of data precision, sorry.
 */

#define BITS_IN_JSAMPLE  @BITS_IN_JSAMPLE@      /* use 8 or 12 */

/* Define if your (broken) compiler shifts signed values as if they were
   unsigned. */
#define RIGHT_SHIFT_IS_UNSIGNED 1//cmakedefine


/*
 * These symbols indicate the properties of your machine or compiler.
 * #define the symbol if yes, #undef it if no.
 */

/* Define "boolean" as unsigned char, not int, on Windows systems.
 */
#ifdef _WIN32
#ifndef __RPCNDR_H__            /* don't conflict if rpcndr.h already read */
typedef unsigned char boolean;
#endif
#define HAVE_BOOLEAN            /* prevent jmorecfg.h from redefining it */
#endif


/*
 * The following options affect code selection within the JPEG library,
 * but they don't need to be visible to applications using the library.
 * To minimize application namespace pollution, the symbols won't be
 * defined unless JPEG_INTERNALS has been defined.
 */

#ifdef JPEG_INTERNALS

/* Define this if your compiler implements ">>" on signed values as a logical
 * (unsigned) shift; leave it undefined if ">>" is a signed (arithmetic) shift,
 * which is the normal and rational definition.
 */
#undef RIGHT_SHIFT_IS_UNSIGNED


#endif /* JPEG_INTERNALS */


/*
 * The remaining options do not affect the JPEG library proper,
 * but only the sample applications cjpeg/djpeg (see cjpeg.c, djpeg.c).
 * Other applications can ignore these.
 */

#ifdef JPEG_CJPEG_DJPEG

/* These defines indicate which image (non-JPEG) file formats are allowed. */

#define BMP_SUPPORTED           /* BMP image file format */
#define GIF_SUPPORTED           /* GIF image file format */
#define PPM_SUPPORTED           /* PBMPLUS PPM/PGM image file format */
#define TARGA_SUPPORTED         /* Targa image file format */

/* Define this if you want to name both input and output files on the command
 * line, rather than using stdout and optionally stdin.  You MUST do this if
 * your system can't cope with binary I/O to stdin/stdout.  See comments at
 * head of cjpeg.c or djpeg.c.
 */
#undef TWO_FILE_COMMANDLINE

/* By default, we open image files with fopen(..., "rb") or fopen(..., "wb").
 * This is necessary on systems that distinguish text files from binary files,
 * and is harmless on most systems that don't.  If you have one of the rare
 * systems that complains about the "b" spec, define this symbol.
 */
#undef DONT_USE_B_MODE

/* Define this if you want percent-done progress reports from cjpeg/djpeg.
 */
#undef PROGRESS_REPORT


#endif /* JPEG_CJPEG_DJPEG */
