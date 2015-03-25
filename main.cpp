#include "SDL2/SDL.h"
#include <stdio.h>
#include <sys/time.h>

#define DEBUG 1

#define FPS 60
#define MSPS 1000

#define WINDOW_W 1366
#define WINDOW_H 768
#define WINDOW_X SDL_WINDOWPOS_UNDEFINED
#define WINDOW_Y SDL_WINDOWPOS_UNDEFINED
#define WINDOW_VIS SDL_WINDOW_SHOWN
#define WINDOW_FULLSCREEN SDL_WINDOW_FULLSCREEN
#define WINDOW_TITLE "Pong"

#define FULLSCREEN false
#define FULLSCREEN_MODE SDL_WINDOW_FULLSCREEN_DESKTOP

#define BG_COLOR 0x006699

#define PADDLE_W 25
#define PADDLE_H 100
#define PADDLE_SPEED 5

#define PADDLE1_X 0
#define PADDLE1_Y (WINDOW_H-PADDLE_H)/2
#define PADDLE1_IMG "paddle1.bmp"
#define PADDLE1_UP SDLK_w
#define PADDLE1_DOWN SDLK_s

#define PADDLE2_X (WINDOW_W-PADDLE_W)
#define PADDLE2_Y PADDLE1_Y
#define PADDLE2_IMG "paddle2.bmp"
#define PADDLE2_UP SDLK_UP
#define PADDLE2_DOWN SDLK_DOWN

#define BALL_SIZE 15
#define BALL_W BALL_SIZE
#define BALL_H BALL_SIZE
#define BALL_X (WINDOW_W-BALL_SIZE)/2
#define BALL_Y (WINDOW_H-BALL_SIZE)/2
#define BALL_SPEED 5
#define BALL_IMG "ball.bmp"

class Paddle
{
	public:
	int x, y, score;
	bool up, down;
	Paddle( int x2, int y2 )
	{
		x = x2;
		y = y2;
		up = false;
		down = false;
		score = 0;
	}
};

class Ball
{
	public:
	int x, y;
	int vx, vy;
	Ball( int x2, int y2 )
	{
		x = x2;
		y = y2;
		vx = -1;
		vy = 1;
	}
};

void debug(char* str);
void draw(SDL_Surface *src, SDL_Surface *dest, int x, int y);
void render(SDL_Surface *paddle1, SDL_Surface *paddle2, SDL_Surface *ball, SDL_Surface *screen, Paddle p1, Paddle p2, Ball b, SDL_Window *window);
bool collision(Paddle p, Ball b);
unsigned long getCurrentMs();


int main(int argc, char* argv[])
{
	SDL_Event event;

	SDL_Window* window = NULL;
	SDL_Surface* screen = NULL;
	SDL_Surface* paddle1 = NULL;
	SDL_Surface* paddle2 = NULL;
	SDL_Surface* ball = NULL;
	
	Paddle p1(PADDLE1_X,PADDLE1_Y);
	Paddle p2(PADDLE2_X,PADDLE2_Y);
	Ball b(BALL_X,BALL_Y);


	//	Set up window and surfaces
	window = SDL_CreateWindow( WINDOW_TITLE, WINDOW_X, WINDOW_Y, WINDOW_W, WINDOW_H, WINDOW_VIS | WINDOW_FULLSCREEN );

	screen = SDL_GetWindowSurface(window);
	paddle1 = SDL_LoadBMP( PADDLE1_IMG );
	paddle2 = SDL_LoadBMP( PADDLE2_IMG );
	ball = SDL_LoadBMP( BALL_IMG );

	//	Render initial game state
	render( paddle1, paddle2, ball, screen, p1, p2, b, window );

	//	Set up time
	unsigned long last_time, current_time, delta_time;

	last_time = getCurrentMs();
	delta_time = 0;

	//	Loop until user exits
	while(true)
	{
		current_time = getCurrentMs();
	
		//	1: Input
		while( SDL_PollEvent( &event ) != 0 )
		{
			if( event.type == SDL_QUIT )
				goto EXIT;
			else if( event.type == SDL_KEYDOWN )
			{
				switch( event.key.keysym.sym )
				{
					case PADDLE1_UP: 
						p1.up = true; 
						break;
					case PADDLE1_DOWN: 
						p1.down = true;
						break;
					case PADDLE2_UP: 
						p2.up = true;
						break;
					case PADDLE2_DOWN: 
						p2.down = true;
						break;
				}
			}
			else if( event.type == SDL_KEYUP )
			{
				switch( event.key.keysym.sym )
				{
					case PADDLE1_UP: 
						p1.up = false; 
						break;
					case PADDLE1_DOWN: 
						p1.down = false;
						break;
					case PADDLE2_UP: 
						p2.up = false;
						break;
					case PADDLE2_DOWN: 
						p2.down = false;
						break;
					case SDLK_ESCAPE:
						goto EXIT;
				}
			}
		}

		//	2: Logic
		
		//	Get time change
		delta_time += current_time - last_time;
		last_time = current_time;

		if( delta_time > MSPS / FPS )
		{
			//	Reset delta
			delta_time = 0;

			//	Move paddles
			if( p1.up ) p1.y -= PADDLE_SPEED;
			if( p1.down ) p1.y += PADDLE_SPEED;
			if( p2.up ) p2.y -= PADDLE_SPEED;
			if( p2.down ) p2.y += PADDLE_SPEED;

			if( p1.y < 0 ) p1.y = 0;
			else if( p1.y > WINDOW_H - PADDLE_H ) p1.y = WINDOW_H - PADDLE_H;
			if( p2.y < 0 ) p2.y = 0;
			else if( p2.y > WINDOW_H - PADDLE_H ) p2.y = WINDOW_H - PADDLE_H;
	
			//	Move ball
			b.x += b.vx*BALL_SPEED;
			b.y += b.vy*BALL_SPEED;
	
			//	Check collision (walls, paddles, out of bounds)
			if(b.x + BALL_W > WINDOW_W)
			{
				p1.score++;
				b.x = BALL_X;
				b.y = BALL_Y;
				b.vx *= -1;
			}
			else if(b.x < 0)
			{
				p2.score++;
				b.x = BALL_X;
				b.y = BALL_Y;
				b.vx *= -1;
			}
			else if(b.y < 0 || b.y + BALL_H > WINDOW_H) b.vy *= -1;
			else if(collision(p1,b) || collision(p2,b)) b.vx *= -1;
	
			//	3: Output
	
			render( paddle1, paddle2, ball, screen, p1, p2, b, window );
		}	

	}

	EXIT:

	//	Clean up images
	SDL_FreeSurface( paddle1 );
	paddle1 = NULL;
	SDL_FreeSurface( paddle2 );
	paddle2 = NULL;
	SDL_FreeSurface( ball );
	ball = NULL;
	
	//	Clean up window and exit
	SDL_DestroyWindow( window );
	SDL_Quit();


	printf("\n\nFinal Score: %d-%d\n\n\t", p1.score, p2.score);
	if(p1.score > p2.score) printf("Player 1 wins!");
	else if(p1.score < p2.score) printf("Player 2 wins!");
	else printf("Draw!");
	printf("\n\n");	

	return 0;
}

void debug(char* str)
{
	if(DEBUG) printf("%s\n",str);
}

void draw(SDL_Surface *src, SDL_Surface *dest, int x, int y)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;

	SDL_BlitSurface( src, NULL, dest, &rect );
}

void render(SDL_Surface *paddle1, SDL_Surface *paddle2, SDL_Surface *ball, SDL_Surface *screen, Paddle p1, Paddle p2, Ball b, SDL_Window *window)
{
//	if(screen == NULL)
//	{	
//		printf("Screen was null\n");
		screen = SDL_GetWindowSurface(window);
//	}

	//	Clear background
	SDL_FillRect( screen, NULL, SDL_MapRGB( screen->format, (BG_COLOR>>16)&0xFF, (BG_COLOR>>8)&0xFF, BG_COLOR&0xFF) );	

	//	Draw images to screen
	draw( paddle1, screen, p1.x, p1.y );
	draw( paddle2, screen, p2.x, p2.y );
	draw( ball, screen, b.x, b.y );

	//	Update surface
	SDL_UpdateWindowSurface( window );

}

bool collision(Paddle p, Ball b)
{
	int pL = p.x;
	int pR = p.x + PADDLE_W;
	int pT = p.y;
	int pB = p.y + PADDLE_H;

	int bL = b.x;
	int bR = b.x + BALL_W;
	int bT = b.y;
	int bB = b.y + BALL_H;

	return(pL < bR && pR > bL && pT < bB && pB > bT);
}

unsigned long getCurrentMs()
{
	struct timeval tv;
	unsigned long ms;

	gettimeofday(&tv,NULL);

	ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);

	return ms;
}