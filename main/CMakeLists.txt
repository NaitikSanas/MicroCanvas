idf_component_register(SRCS "main.c" 
                            "app/app.c"
                            "../examples/simple_menu_demo/simple_menu_demo.c"
                            "../examples/design_game_map/design_game_map.c"
                            "../examples/fonts_support_testing/fonts_demo.c"
                            "../examples/slider_example/slider_example.c"

INCLUDE_DIRS "."    
            "app"   
            "../examples/simple_menu_demo" 
            "../examples/design_game_map" 
            "../examples/fonts_support_testing"  
            "../examples/slider_example" )