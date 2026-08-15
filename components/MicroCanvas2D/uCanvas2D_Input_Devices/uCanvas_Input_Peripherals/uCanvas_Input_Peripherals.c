#include "uCanvas_Input_Peripherals.h"
#include "uCanvas_api.h"

void uCanvas_PB_Input_ISR(void* arg){

}

void uCanvas_Init_PushButton(gpio_num_t gpio_num){
    gpio_set_direction(gpio_num,GPIO_MODE_INPUT);
    gpio_set_pull_mode(gpio_num,GPIO_PULLUP_ONLY);  
    gpio_set_intr_type(gpio_num,GPIO_INTR_DISABLE);
//     gpio_install_isr_service(0);
//     gpio_isr_handler_add(gpio_num, uCanvas_PB_Input_ISR, (void*) gpio_num);
}

uint8_t uCanvas_Get_PushbuttonState(gpio_num_t gpio_num){
    return gpio_get_level(gpio_num);
}

uint8_t uCanvas_Get_PushbuttonState_WTR(gpio_num_t gpio_num){
    if(uCanvas_Get_PushbuttonState(gpio_num)==0){
        while (!uCanvas_Get_PushbuttonState(gpio_num))
        {
            uCanvas_Delay(1);
        }  
        return 0;
    }
    else return uCanvas_Get_PushbuttonState(gpio_num);
}

void uCanvas_rotary_encoder_init(rotary_encoder_t* obj,gpio_num_t pinA, gpio_num_t pinB, gpio_num_t pinSW){
    if(pinA != -1){
        obj->pin_A = pinA;
        uCanvas_Init_PushButton(pinA);
    }

    if(pinB != -1){
        obj->pin_B = pinB;
        uCanvas_Init_PushButton(pinB);
    }

    if(pinSW != -1){
        obj->pin_SW = pinSW;
        uCanvas_Init_PushButton(pinSW);
    }
    obj->state = ENCODER_IDLE;
    obj->current_state = ENCODER_IDLE;
    obj->last_state =ENCODER_IDLE;
}

// void uCanvas_rotary_encoder_read(rotary_encoder_t* obj){
//     uint8_t A = uCanvas_Get_PushbuttonState(obj->pin_A);
//     uint8_t B = uCanvas_Get_PushbuttonState(obj->pin_B);
//     obj->current_state = (A << 1) | B;    
//     int direction = 0;
//     // Check valid rotary encoder transitions (Gray code sequence)
//     if (obj->last_state == 0b00 && obj->current_state == 0b01) direction = ENCODER_CW;  // CW
//     if (obj->last_state == 0b01 && obj->current_state == 0b11) direction = ENCODER_CW;  
//     if (obj->last_state == 0b11 && obj->current_state == 0b10) direction = ENCODER_CW;  
//     if (obj->last_state == 0b10 && obj->current_state == 0b00) direction = ENCODER_CW;  
    
//     if (obj->last_state == 0b00 && obj->current_state == 0b10) direction = ENCODER_CCW; // CCW
//     if (obj->last_state == 0b10 && obj->current_state == 0b11) direction = ENCODER_CCW; 
//     if (obj->last_state == 0b11 && obj->current_state == 0b01) direction = ENCODER_CCW; 
//     if (obj->last_state == 0b01 && obj->current_state == 0b00) direction = ENCODER_CCW; 
//     // Update last state
//     obj->last_state = obj->current_state;
//     obj->state = direction; // +1 for CW, -1 for CCW, 0 for no change
//     obj->sw_state = uCanvas_Get_PushbuttonState(obj->pin_SW);
// }

const int8_t encoder_lookup[4][4] = {
    // current_state: 00  01  10  11
    /* 00 */         { 0,  ENCODER_CW,  ENCODER_CCW,  0 },
    /* 01 */         { ENCODER_CCW, 0,  0,           ENCODER_CW },
    /* 10 */         { ENCODER_CW,  0,  0,           ENCODER_CCW },
    /* 11 */         { 0,  ENCODER_CCW,  ENCODER_CW,  0 }
};

void uCanvas_rotary_encoder_read(rotary_encoder_t* obj) {
    // Read raw A and B values
    uint8_t A = uCanvas_Get_PushbuttonState(obj->pin_A);
    uint8_t B = uCanvas_Get_PushbuttonState(obj->pin_B);
    
    // Pack A and B into a 2-bit state
    obj->current_state = (A << 1) | B;

    // Determine direction based on state transition
    int direction = encoder_lookup[obj->last_state][obj->current_state];

    // Update states
    obj->last_state = obj->current_state;
    obj->state = direction; // +1 = CW, -1 = CCW, 0 = No movement or invalid

    // Read pushbutton (switch) state
    obj->sw_state = uCanvas_Get_PushbuttonState(obj->pin_SW);
}

encoder_state_t uCanvas_rotary_encoder_get_state(rotary_encoder_t* obj){
    return obj->state;
}
