#define _USE_MATH_DEFINES //some SDL thing

#include <stdbool.h>
#include <stdlib.h>
#include <SDL.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <WinUser.h>
#include <pthread.h>
#include <time.h>
#include "vector2.h"
#include "header.h"


//SDL
#define WINDOW_TITLE        		"Pong: 0 - 0"
#define SDL_FAILURE_CODE    		-1
#define SURFACE_FLAGS       		0
#define SURFACE_WIDTH       		100
#define SURFACE_HEIGHT      		100
#define SURFACE_DEPTH       		1
#define SURFACE_RMASK       		0xff000000
#define SURFACE_GMASK       		0x00ff0000
#define SURFACE_BMASK       		0x0000ff00
#define SURFACE_AMASK       		0x000000ff
#define WINDOW_POS_X        		SDL_WINDOWPOS_CENTERED
#define WINDOW_POS_Y        		SDL_WINDOWPOS_CENTERED
#define WINDOW_WIDTH        		500
#define WINDOW_HEIGHT       		500
#define WINDOW_FLAGS        		0

//colours
#define COLOR_WHITE 				0xFFFFFF
#define COLOR_BLACK 				0x000000
#define COLOR_RED   				0xEB3434
#define COLOR_BLUE  				0x348FEB


//general properties
#define FRAMERATE               	144
#define FRAME_DELAY             	1000/FRAMERATE 	//time between display updates
#define LOGICAL_RESOLUTION_X    	10000
#define LOGICAL_RESOLUTION_Y    	5000
#define LOGIC_DELAY					1000/100 		//time between game state updates

//paddle properties
#define PADDLE_EDGE_GAP     		500
#define PADDLE_SPEED        		25
#define PADDLE_WIDTH        		200
#define PADDLE_HEIGHT       		1000
#define PADDLE_LEFT_COLOR   		COLOR_RED
#define PADDLE_RIGHT_COLOR  		COLOR_BLUE
#define PADDLE_LEFT_IS_AI   		false
#define PADDLE_RIGHT_IS_AI  		true

//ball properties
#define BALL_COLOR                  COLOR_WHITE
#define BALL_WIDTH                 	100
#define BALL_HEIGHT                 100
#define POINTS_PER_GOAL             1
#define BALL_INITIAL_COORDS_X       LOGICAL_RESOLUTION_X/2;
#define BALL_INITIAL_COORDS_Y       LOGICAL_RESOLUTION_Y/2;
#define BALL_INITIAL_VELOCITY_X     50
#define BALL_INITIAL_VELOCITY_Y     0
#define BOUNCE_MINARC 				40
#define BOUNCE_MAXARC 				140

point2 *paddleLeft;
int paddleLeftCmd[1];  //user or AI commands to paddle [up/down]
int leftPoints;

point2 *ballCoords;
vector2 *ballVelocity;

point2 *paddleRight;
int paddleRightCmd[1]; //user or AI commands to paddle [up/down]
int rightPoints;

bool game_continue;
SDL_Window *window; 
SDL_Surface *surface;

void error(const char * format, ...)
{
	va_list args;
	time_t t;
	struct tm *localTime;
	char timeStr[64];
	FILE* errorout = fopen("error.log", "a");
	
	va_start(args, format); //get varargs
	
	//get time as string
	time(&t);
	localTime = localtime(&t);
	strftime(timeStr, 64, "%Y-%m-%d %H:%M:%S", localTime);
	
	//print error
	 fprintf(errorout, "%s: ", timeStr);
	vfprintf(errorout, format, args);
	 fprintf(errorout, "\n");
	
	//free resources
	fclose(errorout);
	va_end(args);
}

void render()
{
	while(game_continue){
		render_frame(&window, &surface, &paddleLeft, &paddleRight, &ballCoords);
		Sleep(FRAME_DELAY);
	}
}

void do_ai()
{
	while(game_continue)
	{
		if(PADDLE_LEFT_IS_AI)
			ai_move(ballVelocity->x,&ballCoords, &paddleLeft, &paddleLeftCmd[0]);
		if(PADDLE_RIGHT_IS_AI)
			ai_move(ballVelocity->x,&ballCoords, &paddleRight, &paddleRightCmd[0]);
		Sleep(LOGIC_DELAY);
	}
}

void get_player_input()
{
	while(game_continue)
	{
		//get player input & move player paddles
		if(!PADDLE_LEFT_IS_AI)
			if(GetAsyncKeyState(0x53)& 0x8000) 
				paddleLeftCmd[0] = +1;
			else if(GetAsyncKeyState(0x57)& 0x8000) 
				paddleLeftCmd[0] = -1;
			else
				paddleLeftCmd[0] = 0;
		if(!PADDLE_RIGHT_IS_AI)
			if(GetAsyncKeyState(0x4B)& 0x8000) 
				paddleRightCmd[0] = -1;
			else if(GetAsyncKeyState(0x49)& 0x8000)
				paddleRightCmd[0] = +1;
			else
				paddleRightCmd[0] = 0;
		Sleep(LOGIC_DELAY);
	}
}

void game_logic()
{
	while(game_continue)
	{
		add_vector2_point2(&ballVelocity, &ballCoords); //move ball
		//move paddles
		move_paddle(paddleLeftCmd[0], &paddleLeft);
		move_paddle(paddleRightCmd[0], &paddleRight);
		//check collision with

		//sides
		if(ballCoords->x<=0) //left
			score_point(&ballVelocity, &ballCoords, &leftPoints);
		else if(ballCoords->x>=LOGICAL_RESOLUTION_X) //right
			score_point(&ballVelocity, &ballCoords, &rightPoints);
		
		else if(ballCoords->y<=0) 
			ball_bounce_edge(&(ballVelocity->y)); //top
		else if(ballCoords->y>=LOGICAL_RESOLUTION_Y)
			ball_bounce_edge(&(ballVelocity->y)); //bottom

		//paddles
		if(ball_colliding_paddle(&ballCoords,&paddleLeft)) 
			ball_bounce_paddle(&ballVelocity);
		else if(ball_colliding_paddle(&ballCoords,&paddleRight))
			ball_bounce_paddle(&ballVelocity);
		
		Sleep(LOGIC_DELAY);
	}
}

int APIENTRY WinMain(HINSTANCE a, HINSTANCE b, LPSTR c, int d)
{
    setup_sdl(&window, &surface);

    //left paddle
    paddleLeft = malloc(sizeof(point2));
        paddleLeft->x = PADDLE_EDGE_GAP;
        paddleLeft->y = LOGICAL_RESOLUTION_Y/2;
    leftPoints = 0; 

    //ball
    ballCoords = malloc(sizeof(point2));
        ballCoords->x = BALL_INITIAL_COORDS_X;
        ballCoords->y = BALL_INITIAL_COORDS_Y;
    ballVelocity = malloc(sizeof(vector2));
	
	//ball goes in random X direction with 50/50 odds
	if(rand()%2==0)
		ballVelocity->x = BALL_INITIAL_VELOCITY_X;
	else 
		ballVelocity->x = BALL_INITIAL_VELOCITY_X*-1;
	ballVelocity->y = BALL_INITIAL_VELOCITY_Y;

    //right paddle
    paddleRight = malloc(sizeof(point2));
        paddleRight->x = LOGICAL_RESOLUTION_X-PADDLE_EDGE_GAP;
        paddleRight->y = LOGICAL_RESOLUTION_Y/2;
    rightPoints = 0;
    
    game_continue = true;
	
	time_t t;
	srand((unsigned) time(&t));

	#define NUM_THREADS 4
	pthread_t threads[NUM_THREADS];
	void* tasks[] = {render, do_ai, get_player_input, game_logic};
	const char* messages[] = {"rendering", "ai", "input", "logic"};
	
	//create threads
	for(int i=0;i<NUM_THREADS;i++)
	{
		int code = pthread_create( &threads[i], NULL, tasks[i], (void*) messages[i]);
		if(code!=0)
		{
			FILE* errorout = fopen("error.log", "a");
			fprintf(errorout, "pthread_create(%s) returned code %i", messages[i], code);
			fclose(errorout);
			exit(1);
		}
	}
	
    SDL_Event e;
	
	while(game_continue)
	{
		while(SDL_PollEvent(&e))
		{
			switch(e.type)
			{
				case SDL_WINDOWEVENT_CLOSE:
				case SDL_QUIT:
					game_continue = false;
				break;
			}
		}
		Sleep(LOGIC_DELAY);
	}

	//wait for threads
	for(int i=0; i < NUM_THREADS; i++)
		pthread_join(threads[i], NULL);
    
	//destroy window & quit SDL
	SDL_FreeSurface(surface);
	SDL_DestroyWindow(window);
	SDL_Quit();

    //free all resources
    free(paddleLeft);
    free(ballCoords);
    free(ballVelocity);
    free(paddleRight);

    exit(0);
}

void score_point(vector2 **ballVelocity, point2 **ballCoords, int *scoring)
{
    (*scoring) += POINTS_PER_GOAL; //increment points
	
	//update scoreboard
	//6 = len "65536\0"
	//21 = len "Pong: 65536 - 65536\0"
	char first[6], second[6]; 
	sprintf(first, "%i", rightPoints);
	sprintf(second, "%i", leftPoints);
	
	char buf[21]; sprintf(buf, "Pong: %s - %s", first, second);
	SDL_SetWindowTitle(window, buf);

    //reset ball coords
    (*ballCoords)->x = BALL_INITIAL_COORDS_X;
    (*ballCoords)->y = BALL_INITIAL_COORDS_Y;
    
    //and velocity
    //ball goes in random X direction with 50/50 odds
    if(rand() % (1 + 1 - 0) + 0) 
		(*ballVelocity)->x = BALL_INITIAL_VELOCITY_X;
    else 
		(*ballVelocity)->x = BALL_INITIAL_VELOCITY_X*-1;
    (*ballVelocity)->y = BALL_INITIAL_VELOCITY_Y*-1;
    Sleep(1000);
}

bool ball_colliding_paddle(point2 **ball, point2 **paddle)
{
    return (
        (*paddle)->x-PADDLE_WIDTH/2<=(*ball)->x&&   //p1 is left of
        (*paddle)->y-PADDLE_HEIGHT/2<=(*ball)->y&& //and above ball
        (*paddle)->x+PADDLE_WIDTH/2>=(*ball)->x&& //and p2 is right of
        (*paddle)->y+PADDLE_HEIGHT/2>=(*ball)->y //and below ball
    );
}


void ball_bounce_edge(double *yVelocity)
{
    //invert ball Y velocity
    *yVelocity*=-1;
}

void ball_bounce_paddle(vector2 **ballVelocity)
{	
    //invert ball X velocity with random variance in angle
	//generate new angle
	double angle = (double)((
		(*ballVelocity)->x<0? //if the ball is moving right
			((rand() % (BOUNCE_MAXARC-1-BOUNCE_MINARC))+BOUNCE_MINARC): //between min and max angle on right
			((rand() % (BOUNCE_MINARC-1-BOUNCE_MAXARC))+360-BOUNCE_MAXARC) //else between min and max on left
		)-90)*(M_PI/180); //remove 90 degree offset and convert to radians

	//overwrite ball velocity
	set_vector_with_angle_magnitude(ballVelocity,angle,get_magnitude(ballVelocity));
}

void move_paddle(int direction, point2 **paddleCoords)
{
    int i;
    if(direction==0) 
		return;
    else if(direction<0)
        i = (*paddleCoords)->y - PADDLE_SPEED;
    else
        i = (*paddleCoords)->y + PADDLE_SPEED;
    
    (*paddleCoords)->y = i<LOGICAL_RESOLUTION_Y-PADDLE_HEIGHT/2 ? (i>PADDLE_HEIGHT/2 ? i : PADDLE_HEIGHT/2) : LOGICAL_RESOLUTION_Y-PADDLE_HEIGHT/2; //update coords capped to game area
}

void ai_move(double ballVelocityX, point2 **ballCoords, point2 **paddleCoords, int *cmdStore)
{
	int diff;
	if(((*paddleCoords)->x-(*ballCoords)->x)*ballVelocityX>=0) //if ball is moving towards paddle, move towards ball
		diff = (int)(*ballCoords)->y-(*paddleCoords)->y;
	else //else move towards center
		diff = (int)(LOGICAL_RESOLUTION_Y/2)-(*paddleCoords)->y;
			
    if(diff<0) //if the target is above the ball, move down
		(*cmdStore) = -1;
	else if(diff>0) //and if it's below, move up
		(*cmdStore) = +1;
	else
		(*cmdStore) = 0;
}

int transform_value(int value, int oldSize, int newSize)
{
    return newSize*(((float)(value))/((float)(oldSize)));
    //1000 -> 1920
}

void draw_paddle(SDL_Surface **surface, point2 **paddle, Uint32 paddleColor)
{
    //instantiate rect for drawing
    SDL_Rect *rect = malloc(sizeof(SDL_Rect));
        rect->h = transform_value(PADDLE_HEIGHT, LOGICAL_RESOLUTION_Y, WINDOW_HEIGHT);
        rect->w = transform_value(PADDLE_WIDTH, LOGICAL_RESOLUTION_X, WINDOW_WIDTH);
    //get display coords
    rect->x = transform_value((*paddle)->x-PADDLE_WIDTH/2, LOGICAL_RESOLUTION_X, WINDOW_WIDTH);
    rect->y = transform_value((*paddle)->y-PADDLE_HEIGHT/2, LOGICAL_RESOLUTION_Y, WINDOW_HEIGHT);

    SDL_FillRect(*surface, rect, paddleColor);

    free(rect);
}

void render_frame(SDL_Window **window, SDL_Surface **surface,point2 **paddleLeft, point2 **paddleRight, point2 **ball){
    //clear the frame
    if(SDL_FillRect(*surface, NULL, COLOR_BLACK) == SDL_FAILURE_CODE)
        error("%s", SDL_GetError());
    
    draw_paddle(surface, paddleLeft, PADDLE_LEFT_COLOR);
    draw_paddle(surface, paddleRight, PADDLE_RIGHT_COLOR);

    //draw ball
    SDL_Rect *rect = malloc(sizeof(SDL_Rect));
	rect->h = transform_value(BALL_HEIGHT, LOGICAL_RESOLUTION_X, WINDOW_HEIGHT);
	rect->w = transform_value(BALL_WIDTH, LOGICAL_RESOLUTION_X, WINDOW_WIDTH);
	//get display coords
	rect->x = transform_value((*ball)->x-BALL_WIDTH/2, LOGICAL_RESOLUTION_X, WINDOW_WIDTH);
	rect->y = transform_value((*ball)->y-BALL_HEIGHT/2, LOGICAL_RESOLUTION_Y, WINDOW_HEIGHT);
	//draw
	SDL_FillRect(*surface, rect, BALL_COLOR);
	//deallocate memory
	free(rect);

    //update the window
    if(SDL_UpdateWindowSurface(*window) == SDL_FAILURE_CODE)
        error("%s", SDL_GetError());
}

void setup_sdl(SDL_Window **window, SDL_Surface **surface)
{
    if(SDL_Init(SDL_INIT_VIDEO)<0)
    {
        //exit if SDL cannot initialize
        error("%s", SDL_GetError());
        exit(1);
    }
    else
    {
        //create application window
        *window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_POS_X, WINDOW_POS_Y, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FLAGS);
        if(*window==NULL)
        {
            //exit if no window exists
			error("%s", SDL_GetError());
            exit(1);
        }
        else
        {   
            //get the application window's surface
            *surface = SDL_GetWindowSurface(*window);
            if(*surface==NULL)
            {
                //exit if no surface exists
				error("%s", SDL_GetError());
                exit(1);
            }
        }
    }
}