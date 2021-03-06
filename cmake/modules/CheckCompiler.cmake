 ################################################################################
 #    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    #
 #                                                                              #
 #              This software is distributed under the terms of the             # 
 #         GNU Lesser General Public Licence version 3 (LGPL) version 3,        #  
 #                  copied verbatim in the file "LICENSE"                       #
 ################################################################################
MACRO ( Check_Compiler )


# Set a default build type for single-configuration
# CMake generators if no build type is set.
if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

# Check for the existence of fairsoft-config
# This program only exist in newer versions of fairsoft. If the file exist extract information about
# the compiler and compiler flags used to install fairsoft.
# Compare compiler and compiler flags used to compile fairsoft with the compiler and flags used now
# In case of differences print a warning
Find_Program(FAIRSOFT_CONFIG fairsoft-config PATHS $ENV{SIMPATH}/bin NO_DEFAULT_PATH)

If(FAIRSOFT_CONFIG)
  Message(STATUS "fairsoft-config found")
  Execute_Process(COMMAND ${FAIRSOFT_CONFIG} --cc
                  OUTPUT_VARIABLE FAIRSOFT_C_COMPILER)
  Execute_Process(COMMAND ${FAIRSOFT_CONFIG} --cxx
                  OUTPUT_VARIABLE FAIRSOFT_CXX_COMPILER)
  Execute_Process(COMMAND ${FAIRSOFT_CONFIG} --f77
                  OUTPUT_VARIABLE FAIRSOFT_Fortran_COMPILER)

  # Strip whitespaces, otherwise readlink and comparison don't work
  String(STRIP ${FAIRSOFT_C_COMPILER} FAIRSOFT_C_COMPILER)
  String(STRIP ${FAIRSOFT_CXX_COMPILER} FAIRSOFT_CXX_COMPILER)
  String(STRIP ${FAIRSOFT_Fortran_COMPILER} FAIRSOFT_Fortran_COMPILER)

  Get_Filename_Component(FAIRSOFT_C_COMPILER ${FAIRSOFT_C_COMPILER} REALPATH)
  Get_Filename_Component(FAIRSOFT_CXX_COMPILER ${FAIRSOFT_CXX_COMPILER} REALPATH)
  Get_Filename_Component(FAIRSOFT_Fortran_COMPILER ${FAIRSOFT_Fortran_COMPILER} REALPATH)
  
  Set(BREIT_C_COMPILER ${CMAKE_C_COMPILER})
  Set(BREIT_CXX_COMPILER ${CMAKE_CXX_COMPILER})
  Set(BREIT_Fortran_COMPILER ${CMAKE_Fortran_COMPILER})
  String(STRIP ${BREIT_C_COMPILER} BREIT_C_COMPILER)
  String(STRIP ${BREIT_CXX_COMPILER} BREIT_CXX_COMPILER)
  If(BREIT_Fortran_COMPILER)
    String(STRIP ${BREIT_Fortran_COMPILER} BREIT_Fortran_COMPILER)
    Get_Filename_Component(BREIT_Fortran_COMPILER ${BREIT_Fortran_COMPILER} REALPATH)
  EndIf()

  Get_Filename_Component(BREIT_C_COMPILER ${BREIT_C_COMPILER} REALPATH)
  Get_Filename_Component(BREIT_CXX_COMPILER ${BREIT_CXX_COMPILER} REALPATH)

  If(NOT (${FAIRSOFT_C_COMPILER} STREQUAL ${BREIT_C_COMPILER}) OR NOT (${FAIRSOFT_CXX_COMPILER} STREQUAL ${BREIT_CXX_COMPILER}))
    Message(STATUS "C compiler used for FairSoft installation:  ${FAIRSOFT_C_COMPILER}")
    Message(STATUS "C compiler used now:  ${BREIT_C_COMPILER}")
    Message(STATUS "CXX compiler used for FairSoft installation:  ${FAIRSOFT_CXX_COMPILER}")
    Message(STATUS "CXX compiler used now:  ${BREIT_CXX_COMPILER}")
    Message(STATUS "The compiler during the compilation of FairSoft is different from the current one.")
    If(USE_DIFFERENT_COMPILER)
      Message(STATUS "The error was silenced by the usage of -DUSE_DIFFERENT_COMPILER=TRUE")
    Else()
      Message(FATAL_ERROR "This is seen as an error. If you know that the setting is correct you can silence the error by using the CMake flag -DUSE_DIFFERENT_COMPILER=TRUE")
    EndIf()
  EndIf()

  If(BREIT_Fortran_COMPILER) 
    If(NOT (${FAIRSOFT_Fortran_COMPILER} STREQUAL ${BREIT_Fortran_COMPILER}))
      String(STRIP ${FAIRSOFT_Fortran_COMPILER} FAIRSOFT_Fortran_COMPILER)
      Message(STATUS "Fortran compiler used for FairSoft installation:  ${FAIRSOFT_Fortran_COMPILER}")
      Message(STATUS "Fortran compiler used now:  ${BREIT_Fortran_COMPILER}")
      Message(STATUS "The compiler during the compilation of FairSoft is different from the current one.")
      If(USE_DIFFERENT_COMPILER)
        Message(STATUS "The error was silenced by the usage of -DUSE_DIFFERENT_COMPILER=TRUE")
      Else()
        Message(FATAL_ERROR "This is seen as an error. If you know that the setting is correct you can silence the error by using the CMake flag -DUSE_DIFFERENT_COMPILER=TRUE")
      EndIf()
    EndIf()
  EndIf()

  Execute_Process(COMMAND ${FAIRSOFT_CONFIG} --cxxflags
                  OUTPUT_VARIABLE FAIRSOFT_CXX_FLAGS)
  String(STRIP ${FAIRSOFT_CXX_FLAGS} FAIRSOFT_CXX_FLAGS)
  Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FAIRSOFT_CXX_FLAGS}")
Else()
  Message(STATUS "fairsoft-config not found")
EndIf()


if (CMAKE_SYSTEM_NAME MATCHES Linux)
   MESSAGE("--- Found a Linux ssytem")
   if (CMAKE_COMPILER_IS_GNUCXX)
      MESSAGE("--- Found GNU compiler collection")
#      set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
#      set ( CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_MODULE_LINKER_FLAGS}")
      # we profile...
#      if(CMAKE_BUILD_TYPE_TOLOWER MATCHES profile)
#        set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
#        set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
#      endif(CMAKE_BUILD_TYPE_TOLOWER MATCHES profile)

   # Select flags.
   set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -Wshadow")
   set(CMAKE_CXX_FLAGS_NIGHTLY        "-O0 -g -Wshadow -Weffc++")
   set(CMAKE_CXX_FLAGS_TEST           "-O2 -g -Wshadow -Weffc++")
   set(CMAKE_CXX_FLAGS_RELEASE        "-O2 -Wshadow ")
#   set(CMAKE_CXX_FLAGS_DEBUG          "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
   set(CMAKE_CXX_FLAGS_DEBUG          "-g -Wshadow ")
   set(CMAKE_CXX_FLAGS_DEBUGFULL      "-g3 -fno-inline  -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-exceptions -fno-check-new -fno-common -fexceptions")
   set(CMAKE_CXX_FLAGS_PROFILE        "-g3 -fno-inline -ftest-coverage -fprofile-arcs -Wshadow -Weffc++ -Wall -Wextra -Wunused-variable")
   set(CMAKE_CXX_FLAGS_ARRAY_CHECK    "-g3 -fno-inline -ftest-coverage -fprofile-arcs -fstack-protector")
   set(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g")
   set(CMAKE_C_FLAGS_RELEASE          "-O2")
#   set(CMAKE_C_FLAGS_DEBUG            "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
   set(CMAKE_C_FLAGS_DEBUG            "-g")
   set(CMAKE_C_FLAGS_DEBUGFULL        "-g3 -fno-inline -Wno-long-long -std=iso9899:1990 -Wundef -Wcast-align -Werror-implicit-function-declaration -Wchar-subscripts -Wall -W -Wpointer-arith -Wwrite-strings -Wformat-security -Wmissing-format-attribute -fno-common -fexceptions")
   set(CMAKE_C_FLAGS_PROFILE          "-g3 -fno-inline -ftest-coverage -fprofile-arcs")
   set(CMAKE_C_FLAGS_ARRAY_CHECK      "-g3 -fno-inline -ftest-coverage -fprofile-arcs -fstack-protector")

#   set ( CMAKE_C_FLAGS     "${CMAKE_C_FLAGS} -Wno-long-long -std=iso9899:1990 -Wundef -Wcast-align -Werror-implicit-function-declaration -Wchar-subscripts -Wall -W -Wpointer-arith -Wwrite-strings -Wformat-security -Wmissing-format-attribute -fno-common")
#   set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-exceptions -fno-check-new -fno-common")
   endif (CMAKE_COMPILER_IS_GNUCXX)

   if (CMAKE_C_COMPILER MATCHES "icc")
      MESSAGE("--- Found Intel compiler collection")
#      set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
#      set ( CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_MODULE_LINKER_FLAGS}")

   # Select flags.
   set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g ")
   set(CMAKE_CXX_FLAGS_RELEASE        "-O2")
   set(CMAKE_CXX_FLAGS_DEBUG          "-O2 -g -0b0 -noalign")
   set(CMAKE_CXX_FLAGS_DEBUGFULL      "-g -Ob0 -noalign -W")
   set(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g")
   set(CMAKE_C_FLAGS_RELEASE          "-O2")
   set(CMAKE_C_FLAGS_DEBUG            "-O2 -g -Ob0 -noalign")
   set(CMAKE_C_FLAGS_DEBUGFULL        "-g -Ob0 -noalign -W")

#   set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -ansi -Wpointer-arith -fno-common")
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi -Wpointer-arith -fno-exceptions -fno-common")

    # Get the directory where to find libimf. The only way I found is to get the install dir from icpc and
    # from there one knows where the the libraries are installed
   Execute_process(COMMAND which icpc OUTPUT_VARIABLE _bla)
   String(REPLACE "bin" "compiler/lib" _intel_lib_dirs ${_bla})
   String(REPLACE "/icpc" "" _intel_lib_dirs ${_intel_lib_dirs})
   String(STRIP ${_intel_lib_dirs} _intel_lib_dirs)

   endif (CMAKE_C_COMPILER MATCHES "icc")
   if (CMAKE_C_COMPILER MATCHES "clang")
      MESSAGE("--- Clang compiler")

   # Select flags.

   set(CMAKE_C_FLAGS_PROFILE          "--coverage -g3")
   set(CMAKE_CXX_FLAGS_PROFILE          "--coverage -g3")

   endif (CMAKE_C_COMPILER MATCHES "clang")
endif (CMAKE_SYSTEM_NAME MATCHES Linux)


if (CMAKE_SYSTEM_NAME MATCHES Darwin)
   EXEC_PROGRAM("sw_vers -productVersion | cut -d . -f 1-2" OUTPUT_VARIABLE MAC_OS_VERSION)
   MESSAGE("--- Found a Mac OS X System ${MAC_OS_VERSION}")
   if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      MESSAGE("--- Found GNU compiler collection")

      STRING(COMPARE EQUAL "10.5" "${MAC_OS_VERSION}" MAC_OS_10_5)
      STRING(COMPARE EQUAL "10.6" "${MAC_OS_VERSION}" MAC_OS_10_6)
      STRING(COMPARE EQUAL "10.7" "${MAC_OS_VERSION}" MAC_OS_10_7)
      IF(MAC_OS_10_5 OR MAC_OS_10_6 OR MAC_OS_10_7)
        SET(CMAKE_CXX_FLAGS "-m64")
        SET(CMAKE_C_FLAGS "-m64")
        SET(CMAKE_Fortran_FLAGS "-m64")
      ENDIF(MAC_OS_10_5 OR MAC_OS_10_6 OR MAC_OS_10_7)

      SET(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -flat_namespace -single_module -undefined dynamic_lookup")
      SET(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -flat_namespace -single_module -undefined dynamic_lookup")
#      MESSAGE("C_FLAGS: ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS}")
#      MESSAGE("CXX_FLAGS: ${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS}")
      Execute_Process(COMMAND gfortran -print-file-name=libgfortran.dylib
                      OUTPUT_VARIABLE FORTRAN_LIBDIR
                     )
      Get_Filename_Component(FORTRAN_LIBDIR ${FORTRAN_LIBDIR}
                             PATH
                            )
      Message("Fortran libraries found in ${FORTRAN_LIBDIR}")
      SET(CMAKE_SHARED_LIBRARY_CREATE_Fortran_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_Fortran_FLAGS} -flat_namespace -single_module -undefined dynamic_lookup")

      # Select flags.
      set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -Wshadow ")
      set(CMAKE_CXX_FLAGS_NIGHTLY        "-O2 -g -Wshadow -Weffc++")
      set(CMAKE_CXX_FLAGS_RELEASE        "-O2 -Wshadow ")
      set(CMAKE_CXX_FLAGS_DEBUG          "-g -O2 -Wshadow  -fno-inline")
      set(CMAKE_CXX_FLAGS_DEBUGFULL      "-g3 -fno-inline -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-exceptions -fno-check-new -fno-common")
      set(CMAKE_CXX_FLAGS_PROFILE        "-g3 -fno-inline -ftest-coverage -fprofile-arcs")
      set(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g")
      set(CMAKE_C_FLAGS_RELEASE          "-O2")
      set(CMAKE_C_FLAGS_DEBUG            "-g -O2  -fno-inline")
      set(CMAKE_C_FLAGS_DEBUGFULL        "-g3 -fno-inline -Wno-long-long -std=iso9899:1990 -Wundef -Wcast-align -Werror-implicit-function-declaration -Wchar-subscripts -Wall -W -Wpointer-arith -Wwrite-strings -Wformat-security -Wmissing-format-attribute -fno-common")
      set(CMAKE_C_FLAGS_PROFILE          "-g3 -fno-inline -ftest-coverage -fprofile-arcs")

      
   else (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        Message("CXX Compiler: ${CMAKE_CXX_COMPILER}")
        Message("CXX Compiler ABI: ${CMAKE_CXX_COMPILER_ABI}")
        Message("CXX Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
        MESSAGE(FATAL_ERROR "This compiler is not known.")   
   endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")


endif (CMAKE_SYSTEM_NAME MATCHES Darwin)



#STRING(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UPPER)

SET (BLA CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE_UPPER})

MESSAGE("--- Build Type: ${CMAKE_BUILD_TYPE}")
MESSAGE("--- Compiler Flags: ${CMAKE_CXX_FLAGS} ${${BLA}}")

ENDMACRO ( Check_Compiler )
