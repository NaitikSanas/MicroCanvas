set(COMPONENT_ADD_INCLUDEDIRS "inc")
set(COMPONENT_SRCS  "src/fonts.c" 
                    "src/ssd1306.c"
)
# Fix cmake build
idf_component_register(SRCS "${COMPONENT_SRCS}"
                       PRIV_REQUIRES driver
                       INCLUDE_DIRS "${COMPONENT_ADD_INCLUDEDIRS}")
