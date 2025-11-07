#include "uCanvas_api.h"
#include "uCanvas2D_ST7789_Port.h"
#include "uCanvas2D_EK79007Port.h"
#include "uCanvasRenderEngine.h"
#include "uCanvas_HID.h"
#include "uCanvas2D_Acceleration.h"
#include "esp_timer.h"
#include "driver/i2s.h"
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "uCanvas_Graph2D.h"
#include "uCanvas_Synth.h"

#define I2S_NUM         (0)
#define I2S_BCK_IO      (26)
#define I2S_WS_IO       (24)
#define I2S_DO_IO       (33)
#define I2S_MCK_IO      (48)
#define SAMPLE_RATE     (44100)
#define BITS_PER_SAMPLE I2S_BITS_PER_SAMPLE_16BIT
#define PI              (3.14159265f)
#define AMPLITUDE       (34000)
#define BUFFER_SIZE     (512)
#define CANVAS_HEIGHT   600
#define CANVAS_WIDTH    1024
#define MAX_VOICES      (5)
#define ADSR_STEP       (0.01f)
#define GRAPH_POINTS    (180)
#define WAVEFORM_BUFFER (512)
#define ADSR_GRAPH_POINTS (100)

typedef enum {
    WAVE_SINE = 0,
    WAVE_SQUARE,
    WAVE_SAWTOOTH
} wave_type_t;

typedef struct {
    float attack_time;
    float decay_time;
    float sustain_level;
    float release_time;
} adsr_t;

typedef struct {
    float frequency;
    float phase;
    uint32_t sample_count;
    int is_active;
    uint32_t release_sample_count;
    int is_releasing;
} voice_t;

typedef struct {
    wave_type_t wave_type;
    adsr_t adsr;
    voice_t voices[MAX_VOICES];
    int active_voice_count;
} synth_state_t;

static synth_state_t synth;
static uCanvas_Input_HID_Device_t KeyBoardDevice;
static uCanvas2D_Instance_t uCanvas_Instance_1;

// Waveform visualization buffer
static int16_t waveform_buffer[WAVEFORM_BUFFER];
static volatile int waveform_write_idx = 0;
static SemaphoreHandle_t waveform_mutex;

static uCanvas_universal_obj_t* wave_label = NULL;
static uCanvas_universal_obj_t* adsr_display = NULL;
static uCanvas_universal_obj_t* voices_display = NULL;
static uCanvas_Graph2D_Instance_t WaveformGraph;
static uCanvas_Graph2D_Instance_t ADSRGraph;

static const struct {
    char key;
    float freq;
    const char* note;
} key_to_freq[] = {
    {'Q', 261.63f, "C4"},  {'W', 293.66f, "D4"},  {'E', 329.63f, "E4"},  {'R', 349.23f, "F4"},
    {'T', 392.00f, "G4"},  {'Y', 440.00f, "A4"},  {'U', 493.88f, "B4"},  {'I', 523.25f, "C5"},
    {'A', 277.18f, "C#4"}, {'S', 311.13f, "D#4"}, {'D', 369.99f, "F#4"}, {'F', 415.30f, "G#4"},
    {'G', 466.16f, "A#4"},
    {'\0', 0, NULL}
};

static float sine_wave(float phase) {
    return sinf(2.0f * PI * phase);
}

static float square_wave(float phase) {
    return (phase < 0.5f) ? 1.0f : -1.0f;
}

static float sawtooth_wave(float phase) {
    return 2.0f * (phase - floorf(phase + 0.5f));
}

static float generate_sample(float phase, wave_type_t type) {
    switch (type) {
        case WAVE_SINE:
            return sine_wave(phase);
        case WAVE_SQUARE:
            return square_wave(phase);
        case WAVE_SAWTOOTH:
            return sawtooth_wave(phase);
        default:
            return 0.0f;
    }
}

static float calculate_envelope(voice_t *v, adsr_t *a) {
    if (!v->is_active && !v->is_releasing) return 0.0f;
    
    if (v->is_releasing) {
        float release_samples = a->release_time * SAMPLE_RATE;
        float progress = (float)v->release_sample_count / release_samples;
        
        if (progress >= 1.0f) {
            v->is_releasing = 0;
            v->is_active = 0;
            return 0.0f;
        }
        
        return a->sustain_level * (1.0f - progress);
    }
    
    float total_attack = a->attack_time * SAMPLE_RATE;
    float total_decay = total_attack + (a->decay_time * SAMPLE_RATE);
    float sample_pos = (float)v->sample_count;
    
    if (sample_pos < total_attack) {
        return sample_pos / total_attack;
    } else if (sample_pos < total_decay) {
        float decay_progress = (sample_pos - total_attack) / (a->decay_time * SAMPLE_RATE);
        return 1.0f - (decay_progress * (1.0f - a->sustain_level));
    } else {
        return a->sustain_level;
    }
}

static void update_adsr_graph(void) {
    // Calculate ADSR envelope shape for visualization
    float total_time = synth.adsr.attack_time + synth.adsr.decay_time + 0.2f + synth.adsr.release_time;
    float time_per_point = total_time / ADSR_GRAPH_POINTS;
    
    for (int i = 0; i < ADSR_GRAPH_POINTS; i++) {
        float current_time = i * time_per_point;
        float envelope_value = 0.0f;
        
        if (current_time < synth.adsr.attack_time) {
            // Attack phase
            envelope_value = current_time / synth.adsr.attack_time;
        } else if (current_time < synth.adsr.attack_time + synth.adsr.decay_time) {
            // Decay phase
            float decay_progress = (current_time - synth.adsr.attack_time) / synth.adsr.decay_time;
            envelope_value = 1.0f - (decay_progress * (1.0f - synth.adsr.sustain_level));
        } else if (current_time < synth.adsr.attack_time + synth.adsr.decay_time + 0.2f) {
            // Sustain phase (0.2s hold)
            envelope_value = synth.adsr.sustain_level;
        } else {
            // Release phase
            float release_progress = (current_time - synth.adsr.attack_time - synth.adsr.decay_time - 0.2f) / synth.adsr.release_time;
            if (release_progress >= 1.0f) {
                envelope_value = 0.0f;
            } else {
                envelope_value = synth.adsr.sustain_level * (1.0f - release_progress);
            }
        }
        
        int scaled_value = (int)(envelope_value * ADSRGraph.data_y_max);
        uCanvasGui_Graph2D_Push_DataPoints_Y(&ADSRGraph, scaled_value);
    }
    
    uCanvasGUI_Graph2D_Update(&ADSRGraph);
}

static void synth_set_wave(wave_type_t type) {
    synth.wave_type = type;
}

static void synth_set_adsr(float attack, float decay, float sustain, float release) {
    synth.adsr.attack_time = fmaxf(0.001f, attack);
    synth.adsr.decay_time = fmaxf(0.001f, decay);
    synth.adsr.sustain_level = fmaxf(0.0f, fminf(1.0f, sustain));
    synth.adsr.release_time = fmaxf(0.001f, release);
    update_adsr_graph();
}

static void synth_note_on(float frequency) {
    for (int i = 0; i < MAX_VOICES; i++) {
        if (!synth.voices[i].is_active && !synth.voices[i].is_releasing) {
            synth.voices[i].frequency = frequency;
            synth.voices[i].sample_count = 0;
            synth.voices[i].phase = 0.0f;
            synth.voices[i].is_active = 1;
            synth.voices[i].is_releasing = 0;
            synth.active_voice_count++;
            return;
        }
    }
}

static void synth_note_off(float frequency) {
    for (int i = 0; i < MAX_VOICES; i++) {
        if (synth.voices[i].is_active && synth.voices[i].frequency == frequency) {
            synth.voices[i].is_active = 0;
            synth.voices[i].is_releasing = 1;
            synth.voices[i].release_sample_count = 0;
            synth.active_voice_count--;
            return;
        }
    }
}

static void update_ui_wave_type(void) {
    char buf[64];
    const char* wave_names[] = {"SINE", "SQUARE", "SAWTOOTH"};
    sprintf(buf, "Wave: %s", wave_names[synth.wave_type]);
    uCanvas_Set_Text(wave_label, buf);
}

static void update_adsr_display(void) {
    char buf[128];
    sprintf(buf, "A:%.0fms D:%.0fms S:%.2f R:%.0fms",
        synth.adsr.attack_time * 1000,
        synth.adsr.decay_time * 1000,
        synth.adsr.sustain_level,
        synth.adsr.release_time * 1000);
    uCanvas_Set_Text(adsr_display, buf);
}

static void update_voices_display(void) {
    char buf[64];
    sprintf(buf, "Voices: %d / %d", synth.active_voice_count, MAX_VOICES);
    uCanvas_Set_Text(voices_display, buf);
}

static void i2s_init(void) {
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = BITS_PER_SAMPLE,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .dma_buf_count = 4,
        .dma_buf_len = 256,
        .use_apll = true,
        .intr_alloc_flags = 0
    };
    
    i2s_pin_config_t pin_config = {
        .mck_io_num = I2S_MCK_IO,
        .bck_io_num = I2S_BCK_IO,
        .ws_io_num = I2S_WS_IO,
        .data_out_num = I2S_DO_IO,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
    
    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM, &pin_config);
    i2s_zero_dma_buffer(I2S_NUM);
}

static void audio_task(void *arg) {
    int16_t *buffer = malloc(BUFFER_SIZE * sizeof(int16_t) * 2);
    if (!buffer) vTaskDelete(NULL);
    
    int capture_counter = 0;
    const int capture_interval = 4;
    
    while (1) {
        for (int i = 0; i < BUFFER_SIZE; i++) {
            float mixed_sample = 0.0f;
            
            for (int v = 0; v < MAX_VOICES; v++) {
                voice_t *voice = &synth.voices[v];
                
                if (voice->is_active || voice->is_releasing) {
                    float phase_increment = voice->frequency / SAMPLE_RATE;
                    float envelope = calculate_envelope(voice, &synth.adsr);
                    float sample = generate_sample(voice->phase, synth.wave_type) * envelope;
                    
                    mixed_sample += sample;
                    
                    voice->phase += phase_increment;
                    if (voice->phase >= 1.0f) voice->phase -= 1.0f;
                    
                    if (voice->is_active) {
                        voice->sample_count++;
                    } else if (voice->is_releasing) {
                        voice->release_sample_count++;
                    }
                }
            }
            
            mixed_sample /= MAX_VOICES;
            int16_t sample_int = (int16_t)(mixed_sample * AMPLITUDE);
            
            buffer[2 * i]     = sample_int;
            buffer[2 * i + 1] = sample_int;
            
            // Capture samples for waveform display
            if (capture_counter++ >= capture_interval) {
                capture_counter = 0;
                if (xSemaphoreTake(waveform_mutex, 0) == pdTRUE) {
                    waveform_buffer[waveform_write_idx] = sample_int;
                    waveform_write_idx = (waveform_write_idx + 1) % WAVEFORM_BUFFER;
                    xSemaphoreGive(waveform_mutex);
                }
            }
        }
        
        size_t bytes_written;
        i2s_write(I2S_NUM, buffer, BUFFER_SIZE * 4, &bytes_written, portMAX_DELAY);
    }
}

static void waveform_display_task(void *arg) {
    vTaskDelay(pdMS_TO_TICKS(500));
    
    while (1) {
        int scaled = 0;
        if (xSemaphoreTake(waveform_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            int read_idx = waveform_write_idx;
            for (int i = 0; i < GRAPH_POINTS && i < WAVEFORM_BUFFER; i++) {
                int idx = (read_idx - GRAPH_POINTS + i + WAVEFORM_BUFFER) % WAVEFORM_BUFFER;
                int16_t sample = waveform_buffer[idx];
                
                // Scale sample from -32768..32767 to graph range
                scaled = ((int32_t)sample + 32768) * WaveformGraph.data_y_max / 65536;
                uCanvasGui_Graph2D_Push_DataPoints_Y(&WaveformGraph, scaled);
            }
            xSemaphoreGive(waveform_mutex);
        }
        
        char title[64];
        const char* wave_names[] = {"SINE", "SQUARE", "SAWTOOTH"};
        sprintf(title, "Waveform - %s", wave_names[synth.wave_type]);
        uCanvasGUI_Graph2D_Update(&WaveformGraph);
        uCanvasGUI_Graph2D_Set_Title_Text(&WaveformGraph, title);
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}

static void MyKeyboardListener(uCanvasInputDeviceType_t device_type, key_event_t* key_event, void* user_data) {
    key_event_t* event = (key_event_t*)key_event;
    
    if (event->state == KEY_STATE_PRESSED) {
        char key = event->key_char;
        
        // Wave selection
        if (key == '1') {
            synth_set_wave(WAVE_SINE);
            update_ui_wave_type();
        } else if (key == '2') {
            synth_set_wave(WAVE_SQUARE);
            update_ui_wave_type();
        } else if (key == '3') {
            synth_set_wave(WAVE_SAWTOOTH);
            update_ui_wave_type();
        }
        // ADSR Control
        else if (key == 'Z') {
            synth_set_adsr(synth.adsr.attack_time + ADSR_STEP, synth.adsr.decay_time, synth.adsr.sustain_level, synth.adsr.release_time);
            update_adsr_display();
        } else if (key == 'X') {
            synth_set_adsr(fmaxf(0.001f, synth.adsr.attack_time - ADSR_STEP), synth.adsr.decay_time, synth.adsr.sustain_level, synth.adsr.release_time);
            update_adsr_display();
        }
        else if (key == 'C') {
            synth_set_adsr(synth.adsr.attack_time, synth.adsr.decay_time + ADSR_STEP, synth.adsr.sustain_level, synth.adsr.release_time);
            update_adsr_display();
        } else if (key == 'V') {
            synth_set_adsr(synth.adsr.attack_time, fmaxf(0.001f, synth.adsr.decay_time - ADSR_STEP), synth.adsr.sustain_level, synth.adsr.release_time);
            update_adsr_display();
        }
        else if (key == 'B') {
            synth_set_adsr(synth.adsr.attack_time, synth.adsr.decay_time, synth.adsr.sustain_level + 0.05f, synth.adsr.release_time);
            update_adsr_display();
        } else if (key == 'N') {
            synth_set_adsr(synth.adsr.attack_time, synth.adsr.decay_time, fmaxf(0.0f, synth.adsr.sustain_level - 0.05f), synth.adsr.release_time);
            update_adsr_display();
        }
        else if (key == 'M') {
            synth_set_adsr(synth.adsr.attack_time, synth.adsr.decay_time, synth.adsr.sustain_level, synth.adsr.release_time + ADSR_STEP);
            update_adsr_display();
        } else if (key == ',') {
            synth_set_adsr(synth.adsr.attack_time, synth.adsr.decay_time, synth.adsr.sustain_level, fmaxf(0.001f, synth.adsr.release_time - ADSR_STEP));
            update_adsr_display();
        }
        // Play notes
        else {
            for (int i = 0; key_to_freq[i].key != '\0'; i++) {
                if (key == key_to_freq[i].key || key == tolower(key_to_freq[i].key)) {
                    synth_note_on(key_to_freq[i].freq);
                    update_voices_display();
                    break;
                }
            }
        }
    }
    else if (event->state == KEY_STATE_RELEASED) {
        char key = event->key_char;
        for (int i = 0; key_to_freq[i].key != '\0'; i++) {
            if (key == key_to_freq[i].key || key == tolower(key_to_freq[i].key)) {
                synth_note_off(key_to_freq[i].freq);
                update_voices_display();
                break;
            }
        }
    }
}

void fps_monitor(void) {
    char buf[32] = {0};
    uCanvas_universal_obj_t* fps = New_uCanvas_2DTextbox(buf, CANVAS_WIDTH - 100, 10);
    uCanvas_Set_Color(fps, 10, 147, 255);
    uCanvas_Set_Textbox_Alignment(fps, TEXT_LEFT_ALIGNED);
    uCanvas_Set_TextBox_FontType(fps, SFONT_12);
    
    while (1) {
        sprintf(buf, "FPS: %lld", uCanvas_Get_FPS(&uCanvas_Instance_1));
        uCanvas_Set_Text(fps, buf);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void uCanvas_Synth(void) {
    i2s_init();
    
    waveform_mutex = xSemaphoreCreateMutex();
    memset(waveform_buffer, 0, sizeof(waveform_buffer));
    
    synth.wave_type = WAVE_SINE;
    synth.active_voice_count = 0;
    synth_set_adsr(0.05f, 0.1f, 0.8f, 0.2f);
    
    for (int i = 0; i < MAX_VOICES; i++) {
        synth.voices[i].is_active = 0;
        synth.voices[i].is_releasing = 0;
        synth.voices[i].phase = 0.0f;
    }
    
    KeyBoardDevice.EventListenerCallback = MyKeyboardListener;
    uCanvas_Get_HID_Device(&KeyBoardDevice);
    KeyBoardDevice.uCanvas_Input_HID_Device_Init();
    
    Intialize_PPA();
    
    uCanvas_Scene_t* scene = New_uCanvas_Scene();
    uCanvas2D_Display_Panel_t* panel = uCanvas2D_Get_Panel_Driver_EK79007();
    panel->init(1);
    panel->set_backlight(3000);
    
    if (uCanvas_Attach_RenderBuffer(&uCanvas_Instance_1, 512, 600)) {
        uCanvas_Set_Panel_RefreshDelay(&uCanvas_Instance_1, 0);
        uCanvas_Attach_Panel(&uCanvas_Instance_1, panel);
        uCanvas_Set_ViewPort_Position(&uCanvas_Instance_1, 0, 0);
        uCanvas_Attach_Scene(&uCanvas_Instance_1, scene);
        uCanvas_Attach_Renderer(&uCanvas_Instance_1, 1);
        uCanvas_Set_Render_Mode(&uCanvas_Instance_1, AUTO_REFRESH);
    }
    
    uCanvas_set_active_scene(scene);
    
    // Initialize waveform graph
    WaveformGraph.data_x_min = 0;
    WaveformGraph.data_x_max = GRAPH_POINTS;
    WaveformGraph.data_y_min = 0;
    WaveformGraph.data_y_max = 300;
    
    if (uCanvasGUI_Create_Graph2D(&WaveformGraph, 30, 180, 400, 150, GRAPH_POINTS)) {
        uCanvasGUI_Graph2D_Set_BackGround_Color(&WaveformGraph,0,40,0);
        uCanvasGUI_Graph2D_Set_Border_Color(&WaveformGraph, 41, 255, 144);
        uCanvasGUI_Graph2D_Set_MainContent_Color(&WaveformGraph, 41, 255, 144);
        WaveformGraph.GraphType = GRAPH2D_LINE;
        
        // Initialize graph with center line
        for (int i = 0; i < GRAPH_POINTS; i++) {
            uCanvasGui_Graph2D_Push_DataPoints(&WaveformGraph, i, WaveformGraph.data_y_max / 2);
        }
        uCanvasGUI_Graph2D_Update(&WaveformGraph);
    }
    
    // Initialize ADSR envelope graph
    ADSRGraph.data_x_min = 0;
    ADSRGraph.data_x_max = ADSR_GRAPH_POINTS;
    ADSRGraph.data_y_min = 0;
    ADSRGraph.data_y_max = 100;
    
    if (uCanvasGUI_Create_Graph2D(&ADSRGraph, 30, 400, 400, 100, ADSR_GRAPH_POINTS)) {
        uCanvasGUI_Graph2D_Set_BackGround_Color(&ADSRGraph, 40, 0, 40);
        uCanvasGUI_Graph2D_Set_Border_Color(&ADSRGraph, 255, 0, 255);
        uCanvasGUI_Graph2D_Set_MainContent_Color(&ADSRGraph, 255, 100, 255);
        ADSRGraph.GraphType = GRAPH2D_LINE;
        uCanvasGUI_Graph2D_Set_Title_Text(&ADSRGraph, "ADSR Envelope");
        
        // Initialize with default ADSR curve
        for (int i = 0; i < ADSR_GRAPH_POINTS; i++) {
            uCanvasGui_Graph2D_Push_DataPoints(&ADSRGraph, i, 0);
        }
        update_adsr_graph();
    }

    // ADSR display
    adsr_display = New_uCanvas_2DTextbox("A:50ms D:100ms S:0.80 R:200ms", 30, 520);
    uCanvas_Set_Color(adsr_display, 255, 100, 255);
    uCanvas_Set_TextBox_FontType(adsr_display, SFONT_16);
    
    // Title
    uCanvas_universal_obj_t* title = New_uCanvas_2DTextbox("MicroCanvas\nPolyphonic Synthesizer", 30, 20);
    uCanvas_Set_Color(title, 255, 255, 51);
    uCanvas_Set_TextBox_FontType(title, SFONT_24);
    
    // Wave label
    wave_label = New_uCanvas_2DTextbox("Wave: SINE", 30, 80);
    uCanvas_Set_Color(wave_label, 0, 255, 0);
    uCanvas_Set_TextBox_FontType(wave_label, SFONT_20);
    
    
    
    // Voices display
    voices_display = New_uCanvas_2DTextbox("Voices: 0 / 4", 30, 120);
    uCanvas_Set_Color(voices_display,41, 255, 144);
    uCanvas_Set_TextBox_FontType(voices_display, SFONT_20);
    
    xTaskCreate(audio_task, "audio_task", 4096, NULL, 5, NULL);
    xTaskCreate(waveform_display_task, "waveform_task", 3072, NULL, 3, NULL);
    uCanvas_Add_Task((void (*))fps_monitor, NULL, 0);
    
    while (1) {
        uCanvas_Delay(10);
    }
}