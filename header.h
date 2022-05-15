#ifndef TETPONG_H
    #define TETPONG_H
    void setup_sdl(SDL_Window **window, SDL_Surface **surface);
    void move_paddle(int direction, point2 **paddleCoords);
    void ai_move(double ballVelocityX, point2 **ballCoords, point2 **paddleCoords, int* cmdStore);
    void render_frame(SDL_Window **window, SDL_Surface **surface,point2 **paddleLeft, point2 **paddleRight, point2 **ball);
    int transform_value(int value, int oldSize, int newSize);
    void ball_bounce_paddle(vector2 **ballVelocity);
    void ball_bounce_edge(double *yVelocity);
    bool ball_colliding_paddle(point2 **ball, point2 **paddle);
    void score_point(vector2 **ballVelocity, point2 **ballCoords, int *scoring);
	void draw_paddle(SDL_Surface **surface, point2 **paddle, Uint32 paddleColor);
#endif
