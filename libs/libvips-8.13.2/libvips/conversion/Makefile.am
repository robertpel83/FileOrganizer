noinst_LTLIBRARIES = libconversion.la

libconversion_la_SOURCES = \
	switch.c \
	transpose3d.c \
	composite.cpp \
	smartcrop.c \
	conversion.c \
	pconversion.h \
	tilecache.c \
	gamma.c \
	sequential.c \
	flatten.c \
	premultiply.c \
	unpremultiply.c \
	byteswap.c \
	cache.c \
	copy.c \
	embed.c \
	flip.c \
	insert.c \
	join.c \
	arrayjoin.c \
	extract.c \
	replicate.c \
	cast.c \
	bandjoin.c \
	bandrank.c \
	recomb.c \
	bandmean.c \
	bandfold.c \
	bandunfold.c \
	bandbool.c \
	bandary.h \
	bandary.c \
	rot.c \
	rot45.c \
	autorot.c \
	ifthenelse.c \
	falsecolour.c \
	msb.c \
	grid.c \
	scale.c \
	wrap.c \
	subsample.c \
	zoom.c

# gcc annoyingly warns about clang pragmas, and does not support suppressing
# the warning with a gcc pragma
AM_CPPFLAGS = \
	-I${top_srcdir}/libvips/include \
	@VIPS_CFLAGS@ @VIPS_INCLUDES@ \
	-Wno-unknown-pragmas

EXTRA_DIST = meson.build

