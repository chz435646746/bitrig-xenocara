# Copyright © 2012 Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice (including the next
# paragraph) shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

if HAVE_SHARED_GLAPI
SHARED_GLAPI_CFLAGS = -DGLX_SHARED_GLAPI
SHARED_GLAPI_LIB = $(top_builddir)/src/mapi/shared-glapi/libglapi.la
endif

SUBDIRS =

EXTRA_DIST = SConscript

if HAVE_XF86VIDMODE
EXTRA_DEFINES_XF86VIDMODE = -DXF86VIDMODE
endif

AM_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/include/GL/internal \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/loader \
	-I$(top_srcdir)/src/mapi \
	-I$(top_srcdir)/src/mapi/glapi \
	-I$(top_builddir)/src/mapi \
	-I$(top_builddir)/src/mapi/glapi \
	$(VISIBILITY_CFLAGS) \
	$(SHARED_GLAPI_CFLAGS) \
	$(EXTRA_DEFINES_XF86VIDMODE) \
	-D_REENTRANT \
	-DDEFAULT_DRIVER_DIR=\"$(DRI_DRIVER_SEARCH_DIR)\" \
	-DGL_LIB_NAME=\"lib@GL_LIB@.so\" \
	$(DEFINES) \
	$(LIBDRM_CFLAGS) \
	$(DRI2PROTO_CFLAGS) \
	$(GLPROTO_CFLAGS) \
	$(X11_INCLUDES)

lib_LTLIBRARIES = lib@GL_LIB@.la

noinst_LTLIBRARIES = libglx.la

libglx_la_SOURCES = \
	clientattrib.c \
	clientinfo.c \
	compsize.c \
	create_context.c \
	eval.c \
	glxclient.h \
	glxcmds.c \
	glxconfig.c \
	glxconfig.h \
	glxcurrent.c \
	glx_error.c \
	glx_error.h \
	glxext.c \
	glxextensions.c \
	glxextensions.h \
	glxhash.c \
	glxhash.h \
	glx_pbuffer.c \
	glx_query.c \
	indirect.c \
	indirect_glx.c \
	indirect_init.c \
	indirect_init.h \
	indirect_size.c \
	indirect_texture_compression.c \
	indirect_transpose_matrix.c \
	indirect_vertex_array.c \
	indirect_vertex_array.h \
	indirect_vertex_array_priv.h \
	indirect_vertex_program.c \
	indirect_window_pos.c \
	packrender.h \
	packsingle.h \
	pixel.c \
	pixelstore.c \
	query_renderer.c \
	render2.c \
	renderpix.c \
	single2.c \
	singlepix.c \
	vertarr.c

libglx_la_LIBADD = $(top_builddir)/src/loader/libloader.la

if HAVE_DRISW
libglx_la_SOURCES += \
	drisw_glx.c \
	drisw_priv.h
endif

if HAVE_DRICOMMON
libglx_la_SOURCES += \
	dri_common.c \
	dri_common.h \
	dri_common_query_renderer.c \
	xfont.c
endif

if HAVE_DRI2
libglx_la_SOURCES += \
	dri2.c \
	dri2_glx.c \
	dri2.h \
	dri2_priv.h \
	dri_glx.c \
	dri_sarea.h \
	XF86dri.c \
	xf86dri.h \
	xf86dristr.h
endif

if HAVE_DRI3
libglx_la_SOURCES += \
	dri3_glx.c \
	dri3_priv.h
endif

if HAVE_APPLEDRI
libglx_la_SOURCES += \
	applegl_glx.c

SUBDIRS += apple
libglx_la_LIBADD += $(builddir)/apple/libappleglx.la
endif

GL_LIBS = \
	libglx.la \
	$(top_builddir)/src/mapi/glapi/libglapi.la \
	$(SHARED_GLAPI_LIB) \
	$(GL_LIB_DEPS)

GL_LDFLAGS = \
	-no-undefined \
	-version-number 1:2 \
	$(BSYMBOLIC) \
	$(GC_SECTIONS) \
	$(LD_NO_UNDEFINED)

lib@GL_LIB@_la_SOURCES =
lib@GL_LIB@_la_LIBADD = $(GL_LIBS)
lib@GL_LIB@_la_LDFLAGS = $(GL_LDFLAGS)

SUBDIRS += . tests

include $(top_srcdir)/install-lib-links.mk
