 ################################################################################
 #    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    #
 #                                                                              #
 #              This software is distributed under the terms of the             # 
 #         GNU Lesser General Public Licence version 3 (LGPL) version 3,        #  
 #                  copied verbatim in the file "LICENSE"                       #
 ################################################################################
# Create a library called "libFairTestDetector" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

add_subdirectory(logger)

set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}/tools
)

Set(SYSTEM_INCLUDE_DIRECTORIES
    ${SYSTEM_INCLUDE_DIRECTORIES}
)


include_directories(${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${Boost_LIBRARY_DIRS}
) 

link_directories(${LINK_DIRECTORIES})

Set(SRCS 
)

  Set(SRCS
    #SIDSLog.cxx
    SIDSProgOptions.cxx
  )

#Set(LINKDEF SIDSToolsLinkDef.h)
Set(LIBRARY_NAME SIDSTools)

Set(DEPENDENCIES boost_system boost_program_options sids_logger)

GENERATE_LIBRARY()
