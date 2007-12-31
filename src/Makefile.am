# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = kipmsgutils.h priorityconfig.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kipmsg.pot

KDE_ICON = AUTO

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kipmsg

# the application source, library search path, and link libraries
kipmsg_SOURCES = main.cpp kipmsgwidget.cpp kipmsgwidgetbase.ui \
							recievedialogbase.ui recievedialog.h recievedialog.cpp \
							senddialogbase.ui senddialog.cpp senddialog.h \
							kipmsgconfigbase.ui kipmsgconfig.cpp kipmsgconfig.h \
							detailconfigbase.ui detailconfig.cpp detailconfig.h \
							logconfigbase.ui logconfig.cpp logconfig.h \
							clickableurlconfigbase.ui clickableurlconfig.cpp clickableurlconfig.h \
						 	unlockpasswordconfigbase.ui unlockpasswordconfig.cpp unlockpasswordconfig.h \
							showconfigbase.ui showconfig.cpp showconfig.h \
							searchboxbase.ui searchbox.cpp searchbox.h \
							aboutbase.ui about.cpp about.h \
							downloadmonitorbase.ui downloadmonitor.cpp downloadmonitor.h \
							absenceconfigbase.ui absenceconfig.cpp absenceconfig.h \
							encodingconfigbase.ui encodingconfig.h encodingconfig.cpp \
							openconfirmbase.ui openconfirm.cpp openconfirm.h \
							passworddialog.cpp passworddialog.h passworddialogbase.ui \
							attachedfiledialog.cpp attachedfiledialog.h attachedfiledialogbase.ui \
							downloaderrordialog.cpp downloaderrordialog.h downloaderrordialogbase.ui \
							downloadcompletedialog.cpp downloadcompletedialog.h downloadcompletedialogbase.ui \
							newabsencenamebase.ui newabsencename.h newabsencename.cpp \
							notifywidgetbase.ui notifywidget.cpp notifywidget.h \
					 		kipmsgnotifybase.ui kipmsgnotify.cpp kipmsgnotify.h \
							networkconfigbase.ui networkconfig.cpp networkconfig.h \
							priorityconfigbase.ui priorityconfig.cpp priorityconfig.h \
							idiomconfigbase.ui idiomconfig.cpp idiomconfig.h \
							hideconfigdialogbase.ui hideconfigdialog.cpp hideconfigdialog.h \
							kipmsgsettings.kcfgc \
							kipmsglogger.cpp kipmsglogger.h kipmsgevent.cpp kipmsgevent.h kipmsgutils.cpp


kipmsg_LDFLAGS = $(KDE_RPATH) $(all_libraries)
kipmsg_LDADD = $(LIB_KDEUI) $(LIB_KIO) $(LIB_KHTML) $(KDE_MT_LIBS) -lipmsg -lcrypto -lssl -lpthread
kde_kcfg_DATA = kipmsgsettings.kcfg

# this is where the desktop file will go 
shelldesktopdir   = $(kde_appsdir)/Utilities
shelldesktop_DATA = kipmsg.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/kipmsg
shellrc_DATA = kipmsgui.rc

