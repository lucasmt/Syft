add_library(synthesis SHARED DFA.cpp DFAGameSolver.cpp SMTBDD.cpp StateMap.cpp SyftMgr.cpp SymbolicDFA.cpp VarPartition.cpp)
include_directories(${CUDD_INCLUDE_DIRS} ${JET_INCLUDE_DIRS} ${RSYNTH_INCLUDE_DIRS})

#add_executable(synth synStrategy.cpp)
add_executable(bdd main.cpp)

target_link_libraries(bdd synthesis ${CUDD_LIBRARIES} ${RSYNTH_LIBRARIES} ${JET_LIBRARIES})
#target_link_libraries(synth synthesis ${CUDD_LIBRARIES} ${JET_LIBRARIES} ${RSYNTH_LIBRARIES})

install(TARGETS bdd synthesis #synth
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )

INSTALL (
  DIRECTORY ${CMAKE_SOURCE_DIR}/src/synthesis
    DESTINATION include
    FILES_MATCHING PATTERN "*.h*")
