include(${CMAKE_CURRENT_LIST_DIR}/ClazySources.cmake)
set(CLAZY_SRCS ${CLAZY_SRCS} ${CMAKE_CURRENT_LIST_DIR}/src/checks/level2/oldstyleconnect.cpp) # inconditional, we don't have the regexp test yet

set(LLVM_LINK_COMPONENTS support)

include_directories(/data/sources/llvm/tools/clang/tools/extra/clang-tidy/)
include_directories(${CMAKE_CURRENT_LIST_DIR}/src/)

add_clang_library(clangTidyClazyModule
  ${CMAKE_CURRENT_LIST_DIR}/src/ClangTidyClazyCompat.cpp
  ${CLAZY_SRCS}

  LINK_LIBS
  clangAST
  clangASTMatchers
  clangBasic
  clangLex
  clangTidy
  clangTidyUtils
  clangRewrite
  clangRewriteFrontend
  )
