cmake_minimum_required(VERSION 3.21)
project(coti-ledger-app)

set(CMAKE_C_STANDARD 11)

set(APP_DOMAIN ./app/src/)
set(BOLOS_SDK_INCLUDE ../nanos-sdk/include ../nanos-sdk/lib_2uf/include ../nanos-sdk/lib_ux/include ../nanos-sdk/lib_u2f/include ../nanos-sdk/lib_cxng/include)

include_directories(${APP_DOMAIN})
include_directories(${BOLOS_SDK_INCLUDE})

add_compile_definitions(OS_IO_SEPROXYHAL HAVE_BAGL HAVE_PRINTF HAVE_SPRINTF HAVE_UX_FLOW HAVE_USB_APDU HAVE_IO_USB HAVE_L4_USBLIB IO_USB_MAX_ENDPOINTS=6 IO_HID_EP_LENGTH=64 HAVE_USB_APDU)
add_compile_definitions(HAVE_ECC HAVE_SHA3 HAVE_U2F HAVE_IO_U2F HAVE_ECC_WEIERSTRASS HAVE_SECP256K1_CURVE HAVE_HASH HAVE_ECDSA U2F_PROXY_MAGIC=\"COTI\" USB_SEGMENT_SIZE=64 BLE_SEGMENT_SIZE=32)
add_compile_definitions(HAVE_WEBUSB WEBUSB_URL_SIZE_B=0 WEBUSB_URL="" PRINTF=screen_printf IO_SEPROXYHAL_BUFFER_SIZE_B=300 APPVERSION)

file(GLOB_RECURSE APP_SRC "./app/src/**")
file(GLOB_RECURSE BOLOS_SDK_SRC "../nanos-sdk/**")
add_executable(coti-ledger-app ${BOLOS_SDK_SRC} ${APP_SRC})

