# Makefile.in generated by automake 1.6.3 from Makefile.am.
# KDE tags expanded automatically by am_edit - $Revision$ 
# Makefile.  Generated from Makefile.in by config.pl.

# Copyright 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002
# Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


SHELL = /bin/sh

srcdir = .
top_srcdir = .
VPATH = .
prefix = /usr
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include
pkgdatadir = $(datadir)/kipmsg
pkglibdir = $(libdir)/kipmsg
pkgincludedir = $(includedir)/kipmsg
top_builddir = .

ACLOCAL = ${SHELL} /root/src/ipmsg/kipmsg/admin/missing --run aclocal-1.6
AUTOCONF = $(SHELL) $(top_srcdir)/admin/cvs.sh configure || touch configure
AUTOMAKE = ${SHELL} /root/src/ipmsg/kipmsg/admin/missing --run automake-1.6
AUTOHEADER = ${SHELL} /root/src/ipmsg/kipmsg/admin/missing --run autoheader

am__cd = CDPATH="$${ZSH_VERSION+.}$(PATH_SEPARATOR)" && cd
INSTALL = /usr/bin/install -c -p
INSTALL_PROGRAM = ${INSTALL} $(INSTALL_STRIP_FLAG)
INSTALL_DATA = ${INSTALL} -m 644
install_sh_DATA = $(install_sh) -c -m 644
install_sh_PROGRAM = $(install_sh) -c
install_sh_SCRIPT = $(install_sh) -c
INSTALL_SCRIPT = ${INSTALL}
INSTALL_HEADER = $(INSTALL_DATA)
transform = s,x,x,
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
build_alias = 
build_triplet = i686-pc-linux-gnu
host_alias = 
host_triplet = i686-pc-linux-gnu
target_alias = 
target_triplet = i686-pc-linux-gnu

EXEEXT = 
OBJEXT = o
PATH_SEPARATOR = :
AMTAR = ${SHELL} /root/src/ipmsg/kipmsg/admin/missing --run tar
AR = ar
ARTSCCONFIG = /usr/bin/artsc-config
AS = @AS@
AUTODIRS = 
AWK = gawk
CC = gcc
CONF_FILES =  $(top_srcdir)/./admin/configure.in.min $(top_srcdir)/configure.in.in
CPP = gcc -E
CXX = g++
CXXCPP = g++ -E
DCOPIDL = /usr/bin/dcopidl
DCOPIDL2CPP = /usr/bin/dcopidl2cpp
DCOPIDLNG = /usr/bin/dcopidlng
DCOP_DEPENDENCIES = $(DCOPIDL) $(DCOPIDLNG)
DEPDIR = .deps
DLLTOOL = @DLLTOOL@
DOXYGEN = @DOXYGEN@
DOXYGEN_PROJECT_NAME = @DOXYGEN_PROJECT_NAME@
DOXYGEN_PROJECT_NUMBER = @DOXYGEN_PROJECT_NUMBER@
DPMSINC = @DPMSINC@
DPMSLIB = @DPMSLIB@
ECHO = echo
EGREP = grep -E
F77 = g77
FRAMEWORK_COREAUDIO = 
GCJ = @GCJ@
GCJFLAGS = @GCJFLAGS@
GLINC = @GLINC@
GLLIB = @GLLIB@
GMSGFMT = /usr/bin/msgfmt
INSTALL_STRIP_PROGRAM = ${SHELL} $(install_sh) -c -s
JAR = @JAR@
JAVAC = @JAVAC@
JAVAH = @JAVAH@
JVMLIBS = @JVMLIBS@
KCFG_DEPENDENCIES = $(KCONFIG_COMPILER)
KCONFIG_COMPILER = /usr/bin/kconfig_compiler
KDECONFIG = /usr/bin/kde-config
KDE_EXTRA_RPATH = 
KDE_HAS_DOXYGEN = @KDE_HAS_DOXYGEN@
KDE_HAVE_DOT = @KDE_HAVE_DOT@
KDE_INCLUDES = -I/usr/include/kde
KDE_LDFLAGS = -L/usr/lib
KDE_MT_LDFLAGS = 
KDE_MT_LIBS = -lpthread
KDE_NO_UNDEFINED = -Wl,--no-undefined -Wl,--allow-shlib-undefined
KDE_PLUGIN = -avoid-version -module -no-undefined $(KDE_NO_UNDEFINED) $(KDE_RPATH) $(KDE_MT_LDFLAGS)
KDE_RPATH = -R $(kde_libraries) -R $(qt_libraries) -R $(x_libraries)
KDE_USE_CLOSURE_FALSE = 
KDE_USE_CLOSURE_TRUE = #
KDE_USE_FINAL_FALSE = 
KDE_USE_FINAL_TRUE = #
KDE_USE_NMCHECK_FALSE = 
KDE_USE_NMCHECK_TRUE = #
KDE_XSL_STYLESHEET = /usr/share/apps/ksgmltools2/customization/kde-chunk.xsl
LEX = @LEX@
LEXLIB = @LEXLIB@
LEX_OUTPUT_ROOT = @LEX_OUTPUT_ROOT@
LIBBZ2 = @LIBBZ2@
LIBCOMPAT = 
LIBCRYPT = -lcrypt
LIBDL = -ldl
LIBGEN = @LIBGEN@
LIBJPEG = -ljpeg
LIBOBJS = 
LIBPNG = -lpng -lz -lm
LIBPTHREAD = -lpthread
LIBPYTHON = @LIBPYTHON@
LIBQIMGIO = @LIBQIMGIO@
LIBRESOLV = -lresolv
LIBSHADOW = @LIBSHADOW@
LIBSM = -lSM -lICE
LIBSOCKET = 
LIBSSL = @LIBSSL@
LIBTIFF = @LIBTIFF@
LIBTOOL = $(SHELL) $(top_builddir)/libtool --silent
LIBUCB = 
LIBUTIL = -lutil
LIBZ = -lz
LIB_JASPER = @LIB_JASPER@
LIB_KAB = -lkab
LIB_KABC = -lkabc
LIB_KDECORE = -lkdecore
LIB_KDEPIM = -lkdepim
LIB_KDEPRINT = -lkdeprint
LIB_KDEUI = -lkdeui
LIB_KFILE = -lkio
LIB_KFM = 
LIB_KHTML = -lkhtml
LIB_KIO = -lkio
LIB_KPARTS = -lkparts
LIB_KSPELL = -lkspell
LIB_KSYCOCA = -lkio
LIB_KUTILS = -lkutils
LIB_POLL = 
LIB_QPE = 
LIB_QT = -lqt-mt  $(LIBZ) $(LIBPNG) -lXext $(LIB_X11) $(LIBSM) -lpthread
LIB_QTOPIA = @LIB_QTOPIA@
LIB_SMB = -lsmb
LIB_X11 = -lX11 $(LIBSOCKET)
LIB_XEXT = -lXext
LIB_XRENDER = 
LN_S = ln -s
MCOPIDL = /usr/bin/mcopidl
MEINPROC = /usr/bin/meinproc
MOC = /usr/lib/qt3/bin/moc
MSGFMT = /usr/bin/msgfmt
NOOPT_CFLAGS = -O0
NOOPT_CXXFLAGS = -O0
OBJC = @OBJC@
OBJCFLAGS = @OBJCFLAGS@
OBJDUMP = @OBJDUMP@
PACKAGE = kipmsg
PAMLIBS = @PAMLIBS@
PASSWDLIBS = @PASSWDLIBS@
PERL = /usr/bin/perl
PYTHONINC = @PYTHONINC@
PYTHONLIB = @PYTHONLIB@
PYTHONMODDIR = @PYTHONMODDIR@
QTDOCDIR = @QTDOCDIR@
QTE_NORTTI = 
QTOPIA_INCLUDES = @QTOPIA_INCLUDES@
QT_INCLUDES = -I/usr/lib/qt3/include
QT_LDFLAGS = -L/usr/lib/qt3/lib
RANLIB = ranlib
RC = @RC@
SETUIDFLAGS = @SETUIDFLAGS@
SSL_INCLUDES = @SSL_INCLUDES@
SSL_LDFLAGS = @SSL_LDFLAGS@
STRIP = strip
TOPSUBDIRS =  doc icons po src
UIC = /usr/lib/qt3/bin/uic -L $(kde_widgetdir) -nounload
UIC_TR = tr2i18n
USER_INCLUDES = 
USER_LDFLAGS = 
USE_EXCEPTIONS = -fexceptions
USE_RTTI = 
USE_THREADS = 
VERSION = 0.1.1
WOVERLOADED_VIRTUAL = 
XGETTEXT = /usr/bin/xgettext
XMLLINT = /usr/bin/xmllint
XPMINC = @XPMINC@
XPMLIB = @XPMLIB@
X_EXTRA_LIBS = 
X_INCLUDES = -I/usr/X11R6/include
X_LDFLAGS = -L/usr/X11R6/lib
X_PRE_LIBS = 
X_RPATH = -R $(x_libraries)
all_includes = -I/usr/include/kde -I/usr/lib/qt3/include -I/usr/X11R6/include 
all_libraries = -L/usr/X11R6/lib -L/usr/lib/qt3/lib -L/usr/lib 
am__include = include
am__quote = 
install_sh = /root/src/ipmsg/kipmsg/admin/install-sh
jni_includes = @jni_includes@
kde_appsdir = ${datadir}/applnk
kde_bindir = ${exec_prefix}/bin
kde_confdir = ${datadir}/config
kde_datadir = ${datadir}/apps
kde_htmldir = ${datadir}/doc/HTML
kde_icondir = ${datadir}/icons
kde_includes = /usr/include/kde
kde_kcfgdir = ${datadir}/config.kcfg
kde_libraries = /usr/lib
kde_libs_htmldir = /usr/share/doc/HTML
kde_libs_prefix = /usr
kde_locale = ${datadir}/locale
kde_mimedir = ${datadir}/mimelnk
kde_moduledir = ${libdir}/kde3
kde_qtver = 3
kde_servicesdir = ${datadir}/services
kde_servicetypesdir = ${datadir}/servicetypes
kde_sounddir = ${datadir}/sounds
kde_styledir = ${libdir}/kde3/plugins/styles
kde_templatesdir = ${datadir}/templates
kde_wallpaperdir = ${datadir}/wallpapers
kde_widgetdir = /usr/lib/kde3/plugins/designer
kdeinitdir = $(kde_moduledir)
qt_includes = /usr/lib/qt3/include
qt_libraries = /usr/lib/qt3/lib
x_includes = /usr/X11R6/include
x_libraries = /usr/X11R6/lib
xdg_appsdir = ${datadir}/applications/kde
xdg_directorydir = ${datadir}/desktop-directories
xdg_menudir = ${sysconfdir}/xdg/menus
SUBDIRS = $(TOPSUBDIRS)

MAINTAINERCLEANFILES = subdirs configure.in acinclude.m4 configure.files 

EXTRA_DIST = admin COPYING configure.in.in
subdir = .
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
mkinstalldirs = $(SHELL) $(top_srcdir)/admin/mkinstalldirs
CONFIG_HEADER = config.h
CONFIG_CLEAN_FILES =
DIST_SOURCES =

#>- RECURSIVE_TARGETS = info-recursive dvi-recursive install-info-recursive \
#>- 	uninstall-info-recursive all-recursive install-data-recursive \
#>- 	install-exec-recursive installdirs-recursive install-recursive \
#>- 	uninstall-recursive check-recursive installcheck-recursive
#>+ 4
RECURSIVE_TARGETS = info-recursive dvi-recursive install-info-recursive \
	uninstall-info-recursive all-recursive install-data-recursive \
	install-exec-recursive installdirs-recursive install-recursive \
	uninstall-recursive check-recursive installcheck-recursive nmcheck-recursive bcheck-recursive
DIST_COMMON = README AUTHORS COPYING ChangeLog INSTALL Makefile.am \
	Makefile.in NEWS TODO acinclude.m4 aclocal.m4 admin/ChangeLog \
	admin/compile admin/config.guess admin/config.sub admin/depcomp \
	admin/install-sh admin/ltmain.sh admin/missing \
	admin/mkinstalldirs admin/ylwrap config.h.in configure \
	configure.in
DIST_SUBDIRS = $(SUBDIRS)
#>- all: config.h
#>+ 1
all: docs-am  config.h
	$(MAKE) $(AM_MAKEFLAGS) all-recursive

.SUFFIXES:

am__CONFIG_DISTCLEAN_FILES = config.status config.cache config.log \
 configure.lineno
$(srcdir)/Makefile.in:  Makefile.am  $(top_srcdir)/configure.in $(ACLOCAL_M4)
#>- 	cd $(top_srcdir) && \
#>- 	  $(AUTOMAKE) --gnu  Makefile
#>+ 3
	cd $(top_srcdir) && \
	  $(AUTOMAKE) --gnu  Makefile
	cd $(top_srcdir) && perl admin/am_edit -padmin Makefile.in
Makefile:  $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) && $(SHELL) ./config.status $@ $(am__depfiles_maybe)

$(top_builddir)/config.status: $(srcdir)/configure $(CONFIG_STATUS_DEPENDENCIES)
	$(SHELL) ./config.status --recheck
$(srcdir)/configure:  $(srcdir)/configure.in $(ACLOCAL_M4) $(CONFIGURE_DEPENDENCIES)
#>- 	cd $(srcdir) && $(AUTOCONF)
#>+ 2
	cd $(srcdir) && rm -f configure
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common configure

$(ACLOCAL_M4):  configure.in acinclude.m4
	cd $(srcdir) && $(ACLOCAL) $(ACLOCAL_AMFLAGS)

config.h: stamp-h1
	@if test ! -f $@; then \
	  rm -f stamp-h1; \
	  $(MAKE) stamp-h1; \
	else :; fi

stamp-h1: $(srcdir)/config.h.in $(top_builddir)/config.status
	@rm -f stamp-h1
	cd $(top_builddir) && $(SHELL) ./config.status config.h

$(srcdir)/config.h.in:  $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOHEADER)
	touch $(srcdir)/config.h.in

distclean-hdr:
	-rm -f config.h stamp-h1

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:
	-rm -f libtool
uninstall-info-am:

# This directory's subdirectories are mostly independent; you can cd
# into them and run `make' without going through this Makefile.
# To change the values of `make' variables: instead of editing Makefiles,
# (1) if the variable is set in `config.status', edit `config.status'
#     (which will cause the Makefiles to be regenerated when you run `make');
# (2) otherwise, pass the desired values on the `make' command line.
$(RECURSIVE_TARGETS):
	@set fnord $$MAKEFLAGS; amf=$$2; \
	dot_seen=no; \
	target=`echo $@ | sed s/-recursive//`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo "Making $$target in $$subdir"; \
	  if test "$$subdir" = "."; then \
	    dot_seen=yes; \
	    local_target="$$target-am"; \
	  else \
	    local_target="$$target"; \
	  fi; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target) \
	   || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done; \
	if test "$$dot_seen" = "no"; then \
	  $(MAKE) $(AM_MAKEFLAGS) "$$target-am" || exit 1; \
	fi; test -z "$$fail"

mostlyclean-recursive clean-recursive distclean-recursive \
maintainer-clean-recursive:
	@set fnord $$MAKEFLAGS; amf=$$2; \
	dot_seen=no; \
	case "$@" in \
	  distclean-* | maintainer-clean-*) list='$(DIST_SUBDIRS)' ;; \
	  *) list='$(SUBDIRS)' ;; \
	esac; \
	rev=''; for subdir in $$list; do \
	  if test "$$subdir" = "."; then :; else \
	    rev="$$subdir $$rev"; \
	  fi; \
	done; \
	rev="$$rev ."; \
	target=`echo $@ | sed s/-recursive//`; \
	for subdir in $$rev; do \
	  echo "Making $$target in $$subdir"; \
	  if test "$$subdir" = "."; then \
	    local_target="$$target-am"; \
	  else \
	    local_target="$$target"; \
	  fi; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target) \
	   || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done && test -z "$$fail"
tags-recursive:
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  test "$$subdir" = . || (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) tags); \
	done

ETAGS = etags
ETAGSFLAGS =

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP) $(TAGS_FILES)
	list='$(SOURCES) $(HEADERS) $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	mkid -fID $$unique

TAGS: tags-recursive $(HEADERS) $(SOURCES) config.h.in $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  if test "$$subdir" = .; then :; else \
	    test -f $$subdir/TAGS && tags="$$tags -i $$here/$$subdir/TAGS"; \
	  fi; \
	done; \
	list='$(SOURCES) $(HEADERS) config.h.in $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$tags$$unique" \
	  || $(ETAGS) $(ETAGSFLAGS) $(AM_ETAGSFLAGS) $(ETAGS_ARGS) \
	     $$tags $$unique

GTAGS:
	here=`$(am__cd) $(top_builddir) && pwd` \
	  && cd $(top_srcdir) \
	  && gtags -i $(GTAGS_ARGS) $$here

distclean-tags:
	-rm -f TAGS ID GTAGS GRTAGS GSYMS GPATH
#>- DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)
#>+ 1
DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST) $(KDE_DIST)

top_distdir = .
distdir = $(PACKAGE)-$(VERSION)

am__remove_distdir = \
  { test ! -d $(distdir) \
    || { find $(distdir) -type d ! -perm -200 -exec chmod u+w {} ';' \
         && rm -fr $(distdir); }; }

GZIP_ENV = --best
distcleancheck_listfiles = find . -type f -print

distdir: $(DISTFILES)
	$(am__remove_distdir)
	mkdir $(distdir)
	$(mkinstalldirs) $(distdir)/admin
	@list='$(DISTFILES)'; for file in $$list; do \
	  if test -f $$file || test -d $$file; then d=.; else d=$(srcdir); fi; \
	  dir=`echo "$$file" | sed -e 's,/[^/]*$$,,'`; \
	  if test "$$dir" != "$$file" && test "$$dir" != "."; then \
	    dir="/$$dir"; \
	    $(mkinstalldirs) "$(distdir)$$dir"; \
	  else \
	    dir=''; \
	  fi; \
	  if test -d $$d/$$file; then \
	    if test -d $(srcdir)/$$file && test $$d != $(srcdir); then \
	      cp -pR $(srcdir)/$$file $(distdir)$$dir || exit 1; \
	    fi; \
	    cp -pR $$d/$$file $(distdir)$$dir || exit 1; \
	  else \
	    test -f $(distdir)/$$file \
	    || cp -p $$d/$$file $(distdir)/$$file \
	    || exit 1; \
	  fi; \
	done
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  if test "$$subdir" = .; then :; else \
	    test -d $(distdir)/$$subdir \
	    || mkdir $(distdir)/$$subdir \
	    || exit 1; \
	    (cd $$subdir && \
	      $(MAKE) $(AM_MAKEFLAGS) \
	        top_distdir="$(top_distdir)" \
	        distdir=../$(distdir)/$$subdir \
	        distdir) \
	      || exit 1; \
	  fi; \
	done
	$(MAKE) $(AM_MAKEFLAGS) \
	  top_distdir="${top_distdir}" distdir="$(distdir)" \
	  dist-hook
	-find $(distdir) -type d ! -perm -777 -exec chmod a+rwx {} \; -o \
	  ! -type d ! -perm -444 -links 1 -exec chmod a+r {} \; -o \
	  ! -type d ! -perm -400 -exec chmod a+r {} \; -o \
	  ! -type d ! -perm -444 -exec $(SHELL) $(install_sh) -c -m a+r {} {} \; \
	|| chmod -R a+r $(distdir)
dist-gzip: distdir
	$(AMTAR) chof - $(distdir) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).tar.gz
	$(am__remove_distdir)

dist dist-all: distdir
	$(AMTAR) chof - $(distdir) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).tar.gz
	$(am__remove_distdir)

# This target untars the dist file and tries a VPATH configuration.  Then
# it guarantees that the distribution is self-contained by making another
# tarfile.
distcheck: dist
	$(am__remove_distdir)
	GZIP=$(GZIP_ENV) gunzip -c $(distdir).tar.gz | $(AMTAR) xf -
	chmod -R a-w $(distdir); chmod a+w $(distdir)
	mkdir $(distdir)/=build
	mkdir $(distdir)/=inst
	chmod a-w $(distdir)
	dc_install_base=`$(am__cd) $(distdir)/=inst && pwd` \
	  && cd $(distdir)/=build \
	  && ../configure --srcdir=.. --prefix=$$dc_install_base \
	    $(DISTCHECK_CONFIGURE_FLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) dvi \
	  && $(MAKE) $(AM_MAKEFLAGS) check \
	  && $(MAKE) $(AM_MAKEFLAGS) install \
	  && $(MAKE) $(AM_MAKEFLAGS) installcheck \
	  && $(MAKE) $(AM_MAKEFLAGS) uninstall \
	  && (test `find $$dc_install_base -type f -print | wc -l` -le 1 \
	      || { echo "ERROR: files left after uninstall:" ; \
	           find $$dc_install_base -type f -print ; \
	           exit 1; } >&2 ) \
	  && $(MAKE) $(AM_MAKEFLAGS) dist-gzip \
	  && rm -f $(distdir).tar.gz \
	  && $(MAKE) $(AM_MAKEFLAGS) distcleancheck
	$(am__remove_distdir)
	@echo "$(distdir).tar.gz is ready for distribution" | \
	  sed 'h;s/./=/g;p;x;p;x'
distcleancheck: distclean
	if test '$(srcdir)' = . ; then \
	  echo "ERROR: distcleancheck can only run from a VPATH build" ; \
	  exit 1 ; \
	fi
	test `$(distcleancheck_listfiles) | wc -l` -eq 0 \
	  || { echo "ERROR: files left after distclean:" ; \
	       $(distcleancheck_listfiles) ; \
	       exit 1; } >&2
check-am: all-am
check: check-recursive
all-am: Makefile config.h
installdirs: installdirs-recursive
installdirs-am:

install: install-recursive
install-exec: install-exec-recursive
install-data: install-data-recursive
uninstall: uninstall-recursive

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am

installcheck: installcheck-recursive
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	  INSTALL_STRIP_FLAG=-s \
	  `test -z '$(STRIP)' || \
	    echo "INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'"` install
mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
	-test -z "$(MAINTAINERCLEANFILES)" || rm -f $(MAINTAINERCLEANFILES)
#>- clean: clean-recursive
#>+ 1
clean: kde-rpo-clean  clean-recursive

#>- clean-am: clean-generic clean-libtool mostlyclean-am
#>+ 1
clean-am: clean-bcheck  clean-generic clean-libtool mostlyclean-am

distclean: distclean-recursive
	-rm -f $(am__CONFIG_DISTCLEAN_FILES)
distclean-am: clean-am distclean-generic distclean-hdr distclean-libtool \
	distclean-tags

dvi: dvi-recursive

dvi-am:

info: info-recursive

info-am:

install-data-am:

install-exec-am:

install-info: install-info-recursive

install-man:

installcheck-am:

maintainer-clean: maintainer-clean-recursive
	-rm -f $(am__CONFIG_DISTCLEAN_FILES)
	-rm -rf autom4te.cache
maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-recursive

mostlyclean-am: mostlyclean-generic mostlyclean-libtool

uninstall-am: uninstall-info-am

uninstall-info: uninstall-info-recursive

.PHONY: $(RECURSIVE_TARGETS) GTAGS all all-am check check-am clean \
	clean-generic clean-libtool clean-recursive dist dist-all \
	dist-gzip distcheck distclean distclean-generic distclean-hdr \
	distclean-libtool distclean-recursive distclean-tags \
	distcleancheck distdir dvi dvi-am dvi-recursive info info-am \
	info-recursive install install-am install-data install-data-am \
	install-data-recursive install-exec install-exec-am \
	install-exec-recursive install-info install-info-am \
	install-info-recursive install-man install-recursive \
	install-strip installcheck installcheck-am installdirs \
	installdirs-am installdirs-recursive maintainer-clean \
	maintainer-clean-generic maintainer-clean-recursive mostlyclean \
	mostlyclean-generic mostlyclean-libtool mostlyclean-recursive \
	tags tags-recursive uninstall uninstall-am uninstall-info-am \
	uninstall-info-recursive uninstall-recursive


$(top_srcdir)/configure.in: configure.in.in $(top_srcdir)/subdirs
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common configure.in ;

$(top_srcdir)/subdirs:
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common subdirs

$(top_srcdir)/acinclude.m4: $(top_srcdir)/admin/acinclude.m4.in $(top_srcdir)/admin/libtool.m4.in
	@cd $(top_srcdir) && cat admin/acinclude.m4.in admin/libtool.m4.in > acinclude.m4

package-messages:
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common package-messages
	$(MAKE) -C po merge

dist-hook:
	cd $(top_distdir) && perl admin/am_edit -padmin
	cd $(top_distdir) && $(MAKE) -f admin/Makefile.common subdirs
# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:




#>+ 2
KDE_DIST=stamp-h.in subdirs kipmsg-vl.spec kipmsg.kdevelop.pcs configure.files kipmsg.kdevses Makefile.cvs Doxyfile kipmsg.kdevelop 

#>+ 2
docs-am:

#>+ 6
force-reedit:
		cd $(top_srcdir) && \
	  $(AUTOMAKE) --gnu  Makefile
	cd $(top_srcdir) && perl admin/am_edit -padmin Makefile.in


#>+ 21
clean-bcheck: 
	rm -f *.bchecktest.cc *.bchecktest.cc.class a.out

bcheck: bcheck-recursive

bcheck-am:
	@for i in ; do \
	    if test $(srcdir)/$$i -nt $$i.bchecktest.cc; then \
	        echo "int main() {return 0;}" > $$i.bchecktest.cc ; \
	        echo "#include \"$$i\"" >> $$i.bchecktest.cc ; \
	        echo "$$i"; \
	        if ! $(CXX) $(DEFS) -I. -I$(srcdir) -I$(top_builddir) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(KDE_CXXFLAGS)  --dump-class-hierarchy -c $$i.bchecktest.cc; then \
	            rm -f $$i.bchecktest.cc; exit 1; \
	        fi ; \
	        echo "" >> $$i.bchecktest.cc.class; \
	        perl $(top_srcdir)/admin/bcheck.pl $$i.bchecktest.cc.class || { rm -f $$i.bchecktest.cc; exit 1; }; \
	        rm -f a.out; \
	    fi ; \
	done


#>+ 3
final:
	$(MAKE) all-am

#>+ 3
final-install:
	$(MAKE) install-am

#>+ 3
no-final:
	$(MAKE) all-am

#>+ 3
no-final-install:
	$(MAKE) install-am

#>+ 3
cvs-clean:
	$(MAKE) admindir=$(top_srcdir)/admin -f $(top_srcdir)/admin/Makefile.common cvs-clean

#>+ 3
kde-rpo-clean:
	-rm -f *.rpo

#>+ 3
nmcheck: 
nmcheck-am: nmcheck
