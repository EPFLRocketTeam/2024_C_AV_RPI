set(LORA_SOURCES src/LoRa.cpp
                src/itoa.cpp
                src/LoopbackStream.cpp
                src/Print.cpp
                src/Stream.cpp
                src/WString.cpp)
add_library(LoRa4Raspi ${LORA_SOURCES})
target_include_directories(LoRa4Raspi PUBLIC include)
target_link_libraries(LoRa4Raspi PUBLIC pigpio)


