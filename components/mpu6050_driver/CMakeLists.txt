set(COMPONENT_ADD_INCLUDEDIRS ".")
set(COMPONENT_SRCS  "mpu60x0_i2c.c" 
                    "mpu60x0.c"
                    "mpu6050_read.c"
)
# Fix cmake build
idf_component_register(SRCS "${COMPONENT_SRCS}"
                       PRIV_REQUIRES driver
                       INCLUDE_DIRS "${COMPONENT_ADD_INCLUDEDIRS}")
