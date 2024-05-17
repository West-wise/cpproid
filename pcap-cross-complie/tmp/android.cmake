cmake_minimum_required(VERSION 3.28)

set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_SYSTEM_VERSION 24)
set(CMAKE_ANDROID_NDK /home/kali/Android/sdk/ndk/25.1.8937393)
set(CMAKE_ANDROID_STL_TYPE c++_static)

set(PCAP_LIB_DIR "/home/kali/Android/pcap")

if(NOT DEFINED BIT)
    set(BIT 64)
endif()

if(BIT STREQUAL 64)
    set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)
    set(PCAP_LIB_DIR "${PCAP_LIB_DIR}/libpcap64")

elseif(BIT STREQUAL 32)
    set(PCAP_LIB_DIR "${PCAP_LIB_DIR}/libpcap32")
    set(CMAKE_ANDROID_ARCH_ABI armeabi-v7a)
    set(CMAKE_ANDROID_ARM_NEON TRUE)
else()
    message(FATAL_ERROR "Unsupported argument BIT: ${BIT}")
endif()