#include "vector2.h"

double get_magnitude(vector2 **v)
{
    return sqrt((double)(((*v)->x*(*v)->x)+((*v)->y*(*v)->y)));
}

void add_vector2_point2(vector2 **v, point2 **p)
{
    ((*p)->x )+=((*v)->x);
    ((*p)->y )+=((*v)->y);
}

void set_vector_with_angle_magnitude(vector2 **vector, double angle, double magnitude)
{
    (*vector)->x = magnitude * cos(angle);
    (*vector)->y = magnitude * sin(angle);
}