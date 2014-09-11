FILE(GLOB_RECURSE SRC_LIST "*.cpp")  

add_library(cppExtLib ${SRC_LIST})

target_link_libraries(cppExtLib lua51)