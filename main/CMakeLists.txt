idf_component_register(SRCS "main.c" 
                            "app/app.c"
                            "../examples/Game_Of_Life/Game_of_life.c"
                            "../examples/L_System_Demo/L_System_Demo.c"
                            "../examples/3D_Cube/3D_Cube.c"
                            "../examples/space_explorer_game/space_explorer_game.c"
                            "../examples/examples_init.c"             

INCLUDE_DIRS "."    
            "app"   
            "../examples/Game_Of_Life"
            "../examples/L_System_Demo"
            "../examples/3D_Cube/"

            "../examples/space_explorer_game"
            "../examples"

             )