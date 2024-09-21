set(WSTRING_SOURCE src/WString.cpp)
add_library(Wstring ${WSTRING_SOURCE})
target_include_directories(Wstring PUBLIC include)

set(ITOA_SOURCE src/itoa.cpp)
add_library(itoa ${ITOA_SOURCE})
target_include_directories(itoa PUBLIC include)

set(LoopBackStream_SOURCE src/LoopbackStream.cpp)
add_library(LoopbackStream ${LoopBackStream_SOURCE})
target_include_directories(LoopbackStream PUBLIC include)

set(PRINT_SOURCE src/Print.cpp)
add_library(Print ${PRINT_SOURCE})
target_include_directories(Print PUBLIC include)

set(STREAM_SOURCE src/Stream.cpp)
add_library(Stream ${STREAM_SOURCE})
target_include_directories(Stream PUBLIC include)

set(LORA_SOURCE src/LoRa.cpp)
add_library(LoRa ${LORA_SOURCE})
target_include_directories(LoRa PUBLIC include)
target_link_libraries(LoRa PUBLIC pigpio)


