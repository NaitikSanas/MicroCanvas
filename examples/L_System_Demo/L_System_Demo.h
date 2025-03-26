
#include "uCanvas_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Structure to store position and angle for branching
typedef struct {
    float x, y;
    float angle;
} TurtleState;

typedef struct {
    char symbol;
    char replacement[20];
} LSystemRule;
TurtleState branch_endpoints[1000];

void L_System_Demo_Main();
void generate_lsystem(char *output, const char *axiom, int iterations, LSystemRule *rules, int rule_count);
void render_lsystem(const char *lsystem, float startX, float startY) ;