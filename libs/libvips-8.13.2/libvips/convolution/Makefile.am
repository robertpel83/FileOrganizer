noinst_LTLIBRARIES = libconvolution.la

libconvolution_la_SOURCES = \
	canny.c \
	sobel.c \
	convolution.c \
	pconvolution.h \
	correlation.c \
	correlation.h \
	conv.c \
	conva.c \
	convf.c \
	convi.c \
	convasep.c \
	convsep.c \
	compass.c \
	fastcor.c \
	spcor.c \
	sharpen.c \
	gaussblur.c 

AM_CPPFLAGS = -I${top_srcdir}/libvips/include @VIPS_CFLAGS@ @VIPS_INCLUDES@ 

EXTRA_DIST = meson.build
