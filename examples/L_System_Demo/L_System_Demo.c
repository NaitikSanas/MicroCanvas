#include "L_System_Demo.h"

#define MAX_ITERATIONS 8
#define MAX_STRING_LENGTH 15000
#define ANGLE 30 // Angle in degrees
#define LENGTH 5 // Initial line length
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
uCanvas_Scene_t* scene;
int endpoint_count = 0;

void L_System_Demo_Main() {
    start_uCanvas_engine();
    scene = New_uCanvas_Scene();
    uCanvas_set_active_scene(scene);
    
    static char lsystem[MAX_STRING_LENGTH] = {0};
    LSystemRule rules[] = {
        {'F', "F[+F]F[-F]"},  // Simple wave-like branching
        {'+', "+15"},         // Small right turn
        {'-', "-15"}          // Small left turn
    };
    generate_lsystem(lsystem, "F", MAX_ITERATIONS, rules, sizeof(rules) / sizeof(rules[0]));
    render_lsystem(lsystem, 0, SCREEN_HEIGHT - 10); // Start at the center-bottom
    printf("done\r\n");
    while (1)
    {
        vTaskDelay(1);
    }
}

void uCanvas_ColorWipe(uCanvas_universal_obj_t* obj, color_t from_color, color_t to_color, uint32_t delay_us, uint16_t steps) {
    int16_t red_diff = to_color.red - from_color.red;
    int16_t green_diff = to_color.green - from_color.green;
    int16_t blue_diff = to_color.blue - from_color.blue;

    for (uint16_t step = 0; step < steps; step++) {
        color_t current_color;
        current_color.red = from_color.red + (red_diff * step) / steps;
        current_color.green = from_color.green + (green_diff * step) / steps;
        current_color.blue = from_color.blue + (blue_diff * step) / steps;
        uCanvas_Set_Color(obj, current_color.red, current_color.green, current_color.blue);
        esp_rom_delay_us(delay_us);
    }
    uCanvas_Set_Color(obj, to_color.red, to_color.green, to_color.blue);
}

// Generate L-System string with custom rules
void generate_lsystem(char *output, const char *axiom, int iterations, LSystemRule *rules, int rule_count) {
    strcpy(output, axiom);
    char *temp = malloc(MAX_STRING_LENGTH);
    if (!temp) {
        printf("[uCanvas]: ERROR: Memory allocation failed for L-System!\n");
        return;
    }
    
    for (int i = 0; i < iterations; i++) {
        int index = 0;
        for (int j = 0; output[j] != '\0'; j++) {
            int replaced = 0;
            for (int k = 0; k < rule_count; k++) {
                if (output[j] == rules[k].symbol) {
                    if (index + strlen(rules[k].replacement) >= MAX_STRING_LENGTH - 1) {
                        printf("[uCanvas]: ERROR: L-System string too large!\n");
                        free(temp);
                        return;
                    }
                    strcpy(&temp[index], rules[k].replacement);
                    index += strlen(rules[k].replacement);
                    replaced = 1;
                    break;
                }
            }
            if (!replaced) {
                if (index + 1 >= MAX_STRING_LENGTH - 1) {
                    printf("[uCanvas]: ERROR: L-System string too large!\n");
                    free(temp);
                    return;
                }
                temp[index++] = output[j];
            }
        }
        temp[index] = '\0';
        strcpy(output, temp);
    }
    free(temp);
}
static uCanvas_universal_obj_t* textbox1;
static uCanvas_universal_obj_t* textbox2;
static char text_buf[64];
static TurtleState stack[1000]; // Stack for branching

// Render the L-System using uCanvas
void render_lsystem(const char *lsystem, float startX, float startY) {
    int stackPos = 0;
    float x = startX, y = startY;
    float angle = -60; // Start facing upwards
    float length = LENGTH;
    endpoint_count = 0; // Reset branch endpoint counter
    
    textbox1 = New_uCanvas_2DTextbox("-", 240/2, 320-16);
    uCanvas_Set_Color(textbox1, 255, 255, 0);
    textbox1->font_properties.font_type = FONT_10M;

    textbox2 = New_uCanvas_2DTextbox("-", 240/2, 320-32);
    uCanvas_Set_Color(textbox2, 255, 255, 0);
    textbox2->font_properties.font_type = FONT_10M;
    
    uint16_t total_objects = 0;
    for (int i = 0; lsystem[i] != '\0'; i++) {
        char c = lsystem[i];
        if (c == 'F') {
            float newX = x + length * cos(angle * M_PI / 180.0);
            float newY = y + length * sin(angle * M_PI / 180.0);
            
            if (newX >= 0 && newX < SCREEN_WIDTH && newY >= 0 && newY < SCREEN_HEIGHT) {
                uCanvas_universal_obj_t* line = New_uCanvas_2DLine(x, y, newX, newY);
                if(line){
                    uCanvas_Set_Line_Coordinates(line, x, y, newX, newY);
                    uCanvas_Set_Color(line, 0, 180, 0);
                    uCanvas_ColorWipe(line,(color_t){150,150,0},(color_t){0,180,0},60000,1);
                    uCanvas_Delay(1);
                    x = newX;
                    y = newY;
                    sprintf(text_buf, "total obj: %d", total_objects);
                    uCanvas_Set_Text(textbox2, text_buf);
                    // sprintf(text_buf, "free mem: %d", esp_get_free_heap_size());
                    uCanvas_Set_Text(textbox1, text_buf);
                    total_objects++;
                }
                else {
                    // sprintf(text_buf, "free mem: %ld [OOM]", esp_get_free_heap_size());
                    uCanvas_Set_Text(textbox1, text_buf);
                }
            }

            // Check if this 'F' is at the tip (last segment before a ']')
            if (lsystem[i + 1] == ']' || lsystem[i + 1] == '\0') {
                if (endpoint_count < 300) {
                    branch_endpoints[endpoint_count++] = (TurtleState){x, y, angle};
                }
            }

        } else if (c == '+') {
            angle += ANGLE;
        } else if (c == '-') {
            angle -= ANGLE;
        } else if (c == '[') {
            stack[stackPos++] = (TurtleState){x, y, angle};
        } else if (c == ']') {
            TurtleState state = stack[--stackPos];
            x = state.x;
            y = state.y;
            angle = state.angle;
        }
    }

    char* buf = malloc(42);
    if(buf){
    for (int i = 0; i < 68; i++)
        printf("b %d\r\n",buf[i]);
    }
    // Draw flowers only at the **true** branch tips
    // for (int i = 0; i < endpoint_count; i++) {
    //     uCanvas_universal_obj_t* flower = New_uCanvas_2DCircle(branch_endpoints[i].x, branch_endpoints[i].y, 2);
    //     uCanvas_Set_Color(flower, 240,0, 0);
    //     uCanvas_Set_Fill(flower,FILL);
      
    //     // uCanvas_ColorWipe(flower,(color_t){0,0,0},(color_t){240,0,0},20000,1);
    //     sprintf(text_buf, "total obj: %d", total_objects);
    //     uCanvas_Set_Text(textbox2, text_buf);
    //     sprintf(text_buf, "free mem: %d", esp_get_free_heap_size());
    //     uCanvas_Set_Text(textbox1, text_buf);
    //     total_objects++;
    //     uCanvas_Delay(5);
       
    // } 

    uCanvas_Delete_Scene(scene);
    // printf()
    printf("free mem: %d", esp_get_free_heap_size());
    // uCanvas_Set_Text(textbox1, text_buf);
}


