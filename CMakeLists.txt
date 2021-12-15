cmake_minimum_required(VERSION 3.21)
project(coti-ledger-app)

set(CMAKE_C_STANDARD 11)

set(APP_DOMAIN ./app/src/)
set(BOLOS_SDK ../nanos-sdk/include ../nanos-sdk/lib_2uf/include ../nanos-sdk/lib_ux/include)

include_directories(${APP_DOMAIN})
include_directories(${BOLOS_SDK})

add_compile_definitions(HAVE_UX_FLOW OS_IO_SEPROXYHAL)
#set(Headers ../nanos-sdk/include/*.h)

file(GLOB_RECURSE APP_SRC "./app/src/**" )
file(GLOB_RECURSE BOLOS_SDK_SRC "../nanos-sdk/**")
add_executable(coti-ledger-app ${BOLOS_SDK_SRC} ${APP_SRC})

