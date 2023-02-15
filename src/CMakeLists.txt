aux_source_directory(. DIR_LIB_SRCS)

add_definitions("-Wall -std=c++11 -O3 -fPIC")
# 生成链接库
add_library (nlpanalyzer SHARED ${DIR_LIB_SRCS})
