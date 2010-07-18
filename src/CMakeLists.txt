project(kipmsg)

find_package (KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)
include(CheckIncludeFiles)

include_directories( ${CMAKE_SOURCE_DIR} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

#check_include_files(sys/param.h HAVE_SYS_PARAM_H)
#check_include_files(net/if_ppp.h HAVE_NET_IF_PPP_H)
#check_include_files(linux/if_ppp.h HAVE_LINUX_IF_PPP_H)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-kipmsg.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kipmsg.h )

########### next target ###############

set(kipmsg_SRCS 
   about.cpp
   absenceconfig.cpp
   attachedfiledialog.cpp
   clickableurlconfig.cpp
   detailconfig.cpp
   downloadcompletedialog.cpp
   downloaderrordialog.cpp
   downloadmonitor.cpp
   encodingconfig.cpp
   hideconfigdialog.cpp
   idiomconfig.cpp
   kipmsgconfig.cpp
   kipmsgevent.cpp
   kipmsglogger.cpp
   kipmsgnotify.cpp
   kipmsgutils.cpp
   kipmsgtray.cpp
   logconfig.cpp
   main.cpp
   networkconfig.cpp
   newabsencename.cpp
   notifywidget.cpp
   openconfirm.cpp
   passworddialog.cpp
   priorityconfig.cpp
   recievedialog.cpp
   searchbox.cpp
   senddialog.cpp
   showconfig.cpp
   unlockpasswordconfig.cpp
   about.h
   absenceconfig.h
   attachedfiledialog.h
   clickableurlconfig.h
   detailconfig.h
   downloadcompletedialog.h
   downloaderrordialog.h
   downloadmonitor.h
   encodingconfig.h
   hideconfigdialog.h
   idiomconfig.h
   kipmsgconfig.h
   kipmsgevent.h
   kipmsglogger.h
   kipmsgnotify.h
   kipmsgutils.h
   kipmsgtray.h
   logconfig.h
   networkconfig.h
   newabsencename.h
   notifywidget.h
   openconfirm.h
   passworddialog.h
   priorityconfig.h
   recievedialog.h
   searchbox.h
   senddialog.h
   showconfig.h
   unlockpasswordconfig.h )

kde4_add_kcfg_files(kipmsg_SRCS kipmsgsettings.kcfgc)

kde4_add_ui_files(kipmsg_PART_SRCS
   aboutbase.ui
   absenceconfigbase.ui
   attachedfiledialogbase.ui
   clickableurlconfigbase.ui
   detailconfigbase.ui
   downloadcompletedialogbase.ui
   downloaderrordialogbase.ui
   downloadmonitorbase.ui
   encodingconfigbase.ui
   hideconfigdialogbase.ui
   idiomconfigbase.ui
   kipmsgconfigbase.ui
   kipmsgnotifybase.ui
#   kipmsgwidgetbase.ui
   logconfigbase.ui
   networkconfigbase.ui
   newabsencenamebase.ui
   notifywidgetbase.ui
   openconfirmbase.ui
   passworddialogbase.ui
   priorityconfigbase.ui
   recievedialogbase.ui
   searchboxbase.ui
   senddialogbase.ui
   showconfigbase.ui
   unlockpasswordconfigbase.ui
)


kde4_add_plugin(kipmsg ${kipmsg_PART_SRCS})

kde4_add_executable(kipmsg ${kipmsg_SRCS})

target_link_libraries(kipmsg ${KDE4_SOLID_LIBRARY} ${KDE_NOTIFYCONFIG_LIBS}
${KDE4_KIO_LIBS} ${KDE4_KDESKTOP_LIBS} ${KDE4_KHTML_LIBS} m ipmsg )
#${KDE4_KIO_LIBS} ${KDE4_KDESKTOP_LIBS} QtWebKit m ipmsg )

if(KDE4_ENABLE_FPIE)
    macro_add_compile_flags(kipmsg ${KDE4_CXX_FPIE_FLAGS})
    macro_add_link_flags(kipmsg ${KDE4_PIE_LDFLAGS})
endif(KDE4_ENABLE_FPIE)
macro_add_compile_flags(kipmsg -g)
macro_add_compile_flags(kipmsg -UNDEBUG)
add_definitions( -DDEBUG )
add_definitions( -DKDE_DEFAULT_DEBUG_AREA=1234 )


#TODO add message about install kipmsg as setuid 
MESSAGE(STATUS "Warning: kipmsg use setuid")
install(TARGETS kipmsg PERMISSIONS SETUID OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############
install( FILES kipmsg.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})

