
BISON_TARGET(parser ltlparser.y ${CMAKE_CURRENT_BINARY_DIR}/ltlparser.c DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/ltlparser.h)
FLEX_TARGET(lexer ltllexer.l ${CMAKE_CURRENT_BINARY_DIR}/ltllexer.c DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/ltllexer.h)
ADD_FLEX_BISON_DEPENDENCY(lexer parser)

include_directories(${JET_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

add_library(ltlf_translation SHARED ltlf2fol.cpp ltl_formula.c LTLInterp.cpp trans.c utility.cpp ../synthesis/VarPartition.cpp ${BISON_parser_OUTPUTS} ${FLEX_lexer_OUTPUTS})

add_executable(readFormula main.cpp)
add_executable(ltlf2fol translate.cpp)

target_link_libraries(readFormula ltlf_translation ${JET_LIBRARIES})
target_link_libraries(ltlf2fol ltlf_translation ${JET_LIBRARIES})

install(TARGETS ltlf2fol
  RUNTIME DESTINATION bin
  )
