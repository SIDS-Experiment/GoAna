 ################################################################################
 #    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    #
 #                                                                              #
 #              This software is distributed under the terms of the             # 
 #         GNU Lesser General Public Licence version 3 (LGPL) version 3,        #  
 #                  copied verbatim in the file "LICENSE"                       #
 ################################################################################

set(INCLUDE_DIRECTORIES
    ${CMAKE_SOURCE_DIR}/data
    ${CMAKE_SOURCE_DIR}/data/io
    ${CMAKE_SOURCE_DIR}/tools
    ${CMAKE_SOURCE_DIR}/analysis/options
    )
include_directories(${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})


If (Boost_FOUND)
  configure_file( ${CMAKE_SOURCE_DIR}/analysis/lambdas/run/scripts/FitLambdas.sh.in ${CMAKE_BINARY_DIR}/bin/FitLambdas.sh )
  EXEC_PROGRAM(/bin/chmod ARGS "u+x  ${CMAKE_BINARY_DIR}/bin/FitLambdas.sh")
EndIf (Boost_FOUND)





set(LINK_DIRECTORIES
    ${Boost_LIBRARY_DIRS}
    ${ROOT_LIBRARY_DIR}
    )

link_directories(${LINK_DIRECTORIES})

#Set(LIBRARY_NAME Comparelib)
Set(DEPENDENCIES_LIST
    SIDSData
    SIDSTools
    go_analysis_options
    Tree
    Hist
    RIO
    Cint
    Core
    Gpad
    RooFitCore 
    RooFit 
    Foam
)
#GENERATE_LIBRARY()

# generate exec
If (Boost_FOUND)
    Set(Exe_Names
    runFitLambda
    )

set(Exe_Source
    run/runFitLambda.cxx
    )

  List(LENGTH Exe_Names _length)
  Math(EXPR _length ${_length}-1)

  ForEach(_file RANGE 0 ${_length})
    List(GET Exe_Names ${_file} _name)
    List(GET Exe_Source ${_file} _src)
    Set(EXE_NAME ${_name})
    Set(SRCS ${_src})
    Set(DEPENDENCIES ${DEPENDENCIES_LIST})
    GENERATE_EXECUTABLE()
  EndForEach(_file RANGE 0 ${_length})
EndIf (Boost_FOUND)



