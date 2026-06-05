# cmake_minimum_required(VERSION 2.8) #定义最低版本号
project(chat) #定义当前工程名字

#配置编译选项
set(CMAKE_BUILD_TYPE "Debug") #设置debug模式，如果没有这一行将不能调试设断点
#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

#配置最终的可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#配置头文件路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db)
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)

#加载子目录
add_subdirectory(src)