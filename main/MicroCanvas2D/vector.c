#include "vector.h"

Vector2D_t add_vec(Vector2D_t vec1,Vector2D_t vec2){
    Vector2D_t res;
    res.x = vec1.x + vec2.x;
    res.y = vec1.y + vec2.y;
    return res;
}
Vector2D_t sub_vec(Vector2D_t vec1,Vector2D_t vec2){
    Vector2D_t res;
    res.x = vec1.x - vec2.x;
    res.y = vec1.y - vec2.y;
    return res;
}


void acc_vecx(Vector2D_t* vec, int x){
    vec->x += x;
}
void acc_vecy(Vector2D_t* vec, int y){
    vec->y += y;
}

Vector2D_t vector_init(int x, int y){
    Vector2D_t vec;
    vec.x = x;
    vec.y = y;
    return vec;
}