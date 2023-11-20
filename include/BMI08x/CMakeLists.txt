add_library(BMI08x bmi08xa.c bmi08x.h bmi08a.c bmi08g.c)

# Include directories for BMI
target_include_directories(BMI08x
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

#target_include_directories(BMI08x PRIVATE ../I2C_Common)

target_link_libraries(BMI08x PRIVATE I2C_Common)