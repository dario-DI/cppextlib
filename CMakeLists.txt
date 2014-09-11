cmake_minimum_required(VERSION 2.8)

project(cppExtLibray)

find_package(Boost REQUIRED)

SET(TRDPARTY_DIR "../3rdParty/include")
SET(INC_DIR "../)

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/include
  ${TRDPARTY_DIR}/include
  ${INC_DIR}

  ../
)

# lib directories
LINK_DIRECTORIES(
  ${PROJECT_BINARY_DIR}/lib
  ${TRDPARTY_DIR}/lib
)


add_subdirectory(cex)

add_subdirectory(src)

add_subdirectory(test)

