cmake_minimum_required(VERSION 2.8) #定义最低版本号
project(main) #定义当前工程名字

set(CMAKE_BUILD_TYPE "Debug") #设置debug模式，如果没有这一行将不能调试设断点
#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

#配置头文件路径
# include_directories()
#配置库文件路径
# link_directories()

#aux_source_directory(. SRC_LIST) #定义变量，存储当前目录下的所有源文件

set(SRC_LIST 
    ./muduo_server.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(server ${SRC_LIST})
target_link_libraries (server muduo_net muduo_base pthread) # 执行可执行文件需要依赖的库
