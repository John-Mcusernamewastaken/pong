#ifndef VECTOR2
    #define VECTOR2
    #include <math.h>
	
	typedef struct vector2 {
		double x;
		double y;
	}vector2;

	typedef struct point2 {
		double x;
		double y;
	}point2;
	
    typedef struct vector2 vector2;
    typedef struct point2 point2;
    double get_magnitude(vector2 **v);
    void add_vector2_point2(vector2 **v, point2 **p);
    void set_vector_with_angle_magnitude(vector2 **vector, double angle, double magnitude);
#endif