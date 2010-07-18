project(kipmsg)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-kipmsg.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kipmsg.h )

find_package (KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)
include(CheckIncludeFiles)

add_subdirectory( src ) 
add_subdirectory( icons ) 
add_subdirectory( po ) 

########### next target ###############

#kde4_add_executable(kipmsg ${kipmsg_SRCS})

#target_link_libraries(kipmsg  ${KDE4_KDE3SUPPORT_LIBS} m )

#if(KDE4_ENABLE_FPIE)
#    macro_add_compile_flags(kipmsg ${KDE4_CXX_FPIE_FLAGS})
#    macro_add_link_flags(kipmsg ${KDE4_PIE_LDFLAGS})
#endif(KDE4_ENABLE_FPIE)


#TODO add message about install kipmsg as setuid 
MESSAGE(STATUS "Warning: kipmsg use setuid")
#install(TARGETS kipmsg PERMISSIONS SETUID OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############
#install( FILES Kppp.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})

