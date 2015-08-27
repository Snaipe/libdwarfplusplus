ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -DLOCALEDIR='"$(localedir)"'

lib_LTLIBRARIES = libdwarf++.la

WARNINGS = -Wall -Wextra \
	-Wno-unused-result

libdwarf___la_CXXFLAGS = \
	$(WARNINGS) \
	-std=c++14 \
	-I$(top_srcdir)/src/ \
	-I$(top_srcdir)/include/ \
	$(COVERAGE_CFLAGS)

libdwarf___la_LDFLAGS = $(COVERAGE_LDFLAGS) -version-info 1:0:0
libdwarf___la_LIBADD = -ldwarf -lelf

EXTRA_DIST = LICENSE

subdirincludedir = $(includedir)/libdwarf++/
subdirinclude_HEADERS = \
    include/libdwarf++/xvector.hh \
    include/libdwarf++/die.hh \
    include/libdwarf++/exception.hh \
    include/libdwarf++/cu.hh \
    include/libdwarf++/cdwarf \
    include/libdwarf++/tag.hh \
    include/libdwarf++/exprloc.hh \
    include/libdwarf++/dwarf.hxx \
    include/libdwarf++/dwarf.hh

libdwarf___la_SOURCES = \
    src/cu.cc \
    src/exprloc.cc \
    src/exception.cc \
    src/die.cc \
    src/tag.cc \
    src/dwarf.cc
