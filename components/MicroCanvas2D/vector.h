#ifndef VECTOR_H
#define VECTOR_H
#include "stdio.h"
#include "math.h"

typedef struct _2Dvec
{
    int x;
    int y;
}Vector2D_t;

Vector2D_t vector_init(int x, int y);
Vector2D_t add_vec(Vector2D_t vec1,Vector2D_t vec2);
Vector2D_t sub_vec(Vector2D_t vec1,Vector2D_t vec2);
void acc_vecx(Vector2D_t* vec, int x);
void acc_vecy(Vector2D_t* vec, int y);

#endif