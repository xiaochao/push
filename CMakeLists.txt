cmake_minimum_required(VERSION 2.6)
project(push)

include_directories(
	/usr/include/mysql
	/home/xiaochao/project/system/push/contrib/include
	/usr/local/include
)

link_directories(
	/usr/lib/mysql
	/usr/local/lib
	/home/xiaochao/project/system/push/contrib/lib/json
)

set(executable_output_path ${PROJECT_SOURCE_DIR}/build/bin)

link_libraries(json pthread mysqlclient event)
add_subdirectory(src)
