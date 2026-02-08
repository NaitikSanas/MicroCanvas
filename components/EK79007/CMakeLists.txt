set(srcs "esp_lcd_ek79007.c" "ek79007_setup.c")
idf_build_get_property(idf_target IDF_TARGET)

message(STATUS "IDF target = ${idf_target}")

if(idf_target STREQUAL "esp32p4")
idf_component_register(SRCS "${COMPONENT_SRCS}" "esp_lcd_ek79007.c" "ek79007_setup.c"
                        INCLUDE_DIRS "." 
                       PRIV_REQUIRES driver   nvs_flash esp_lcd) 
endif()