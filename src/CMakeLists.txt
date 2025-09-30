file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${kuel_SOURCE_DIR}/include/kuel/*.hpp")
# set(HEADER_LIST "${Kuel_SOURCE_DIR}/include/kuel/lib.hpp")

add_library(kuel_library lexer.cpp match_tree.cpp lexer_enum_conv.cpp parser.cpp code_generator.cpp ${HEADER_LIST})

target_include_directories(kuel_library PUBLIC ../include)

target_compile_features(kuel_library PUBLIC cxx_std_11)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})

