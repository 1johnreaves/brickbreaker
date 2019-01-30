#include <string.h>
#include "toolbox.h"
#include "text.c"

#define SCREEN_BOTTOM (SCREEN_HEIGHT - SPRITE_SIZE)
#define SCREEN_SIDE (SCREEN_WIDTH - SPRITE_SIZE)
#define PADDLE_Y 140

int gameOver = 0;



//flips a given pointer
void flip(int * dirPointer)
{
	if(*dirPointer)
	{*dirPointer = 0;}
	else
	{*dirPointer = 1;}
}


void brickCollision(OBJ_ATTR * bricks[], int * brick_x_list[], int * brick_y_list[], int ballX, int ballY, int * ball_x_left, int * ball_y_up, int * numBricks)
{
	int i, tempBrickX, tempBrickY, tempBrickXMax, tempBrickYMax;

	int ballXMax = ballX + SPRITE_SIZE;
	int ballYMax = ballY + SPRITE_SIZE;
	
	for(i = 0; i < *numBricks; i++)
	{
		tempBrickX = brick_x_list[i];
		tempBrickY = brick_y_list[i];
		tempBrickXMax = tempBrickX + 32;
		tempBrickYMax = tempBrickY + 16;
		

		//hit from left
		if( ( ballXMax >= tempBrickX && ( ballXMax <= tempBrickXMax) ) && ( (ballY + (SPRITE_SIZE / 2)) <= tempBrickYMax && (ballY + (SPRITE_SIZE / 2)) >= tempBrickY ) )
		{
			flip(ball_x_left);

			//swap stuff to the final one
			if(i != --*numBricks)
			{
				//x&y lists
				int tbx2 = brick_x_list[*numBricks];
				int tby2 = brick_y_list[*numBricks];

				brick_x_list[i] = tbx2;
				brick_y_list[i] = tby2;

				brick_x_list[*numBricks] = tempBrickX;
				brick_y_list[*numBricks] = tempBrickY;



				//bricks in obj_buffer
				OBJ_ATTR elimBrick = *bricks[i];
				*bricks[i] = *bricks[*numBricks];
				*bricks[*numBricks] = elimBrick;
				

			} 

			obj_hide(bricks[*numBricks]);

				
		}
	
		//hit from right
		if( ( ballX <= tempBrickXMax && ( ballX >= tempBrickX) ) && ( (ballY + (SPRITE_SIZE / 2)) <= tempBrickYMax && (ballY + (SPRITE_SIZE / 2)) >= tempBrickY ) )
		{
			flip(ball_x_left);			

			//swap stuff to the final one
			if(i != --*numBricks)
			{
				//x&y lists
				int tbx2 = brick_x_list[*numBricks];
				int tby2 = brick_y_list[*numBricks];

				brick_x_list[i] = tbx2;
				brick_y_list[i] = tby2;

				brick_x_list[*numBricks] = tempBrickX;
				brick_y_list[*numBricks] = tempBrickY;



				//bricks in obj_buffer
				OBJ_ATTR elimBrick = *bricks[i];
				*bricks[i] = *bricks[*numBricks];
				*bricks[*numBricks] = elimBrick;
				

			} 

			obj_hide(bricks[*numBricks]);
		
		}
	
		//hit from top
		if( ( ballYMax >= tempBrickY && ( ballYMax <= tempBrickYMax) ) && ( (ballX + (SPRITE_SIZE / 2)) <= tempBrickXMax && (ballX + (SPRITE_SIZE / 2)) >= tempBrickX ) )
		{
			flip(ball_y_up);			

			//swap stuff to the final one
			if(i != --*numBricks)
			{
				//x&y lists
				int tbx2 = brick_x_list[*numBricks];
				int tby2 = brick_y_list[*numBricks];

				brick_x_list[i] = tbx2;
				brick_y_list[i] = tby2;

				brick_x_list[*numBricks] = tempBrickX;
				brick_y_list[*numBricks] = tempBrickY;



				//bricks in obj_buffer
				OBJ_ATTR elimBrick = *bricks[i];
				*bricks[i] = *bricks[*numBricks];
				*bricks[*numBricks] = elimBrick;
				

			} 

			obj_hide(bricks[*numBricks]);


		}
	
		//hit from bottom
		if( ( ballY <= tempBrickYMax && ( ballY >= tempBrickY) ) && ( (ballX + (SPRITE_SIZE / 2)) <= tempBrickXMax && (ballX + (SPRITE_SIZE / 2)) >= tempBrickX ) )
		{
			flip(ball_y_up);			

			//swap stuff to the final one
			if(i != --*numBricks)
			{
				//x&y lists
				int tbx2 = brick_x_list[*numBricks];
				int tby2 = brick_y_list[*numBricks];

				brick_x_list[i] = tbx2;
				brick_y_list[i] = tby2;

				brick_x_list[*numBricks] = tempBrickX;
				brick_y_list[*numBricks] = tempBrickY;



				//bricks in obj_buffer
				OBJ_ATTR elimBrick = *bricks[i];
				*bricks[i] = *bricks[*numBricks];
				*bricks[*numBricks] = elimBrick;
				

			} 

			obj_hide(bricks[*numBricks]);


		}
	

	}	
}





//see if the ball collided with the paddle
void paddleCollision(int ballX, int * ballY, int paddleX, int * ball_x_left, int * ball_y_up, int ballSpeed)
{
	int paddleL = paddleX;      //find the left and right
	int paddleR = paddleX + 32; //ends of the paddle
	
	int ballMid = ballX + (SPRITE_SIZE / 2); //we'll test collision with the middle


	if( (ballMid >= paddleL && ballMid <= paddleR) && (*ballY + 16) >= (PADDLE_Y - ballSpeed) )
	{
		flip(ball_y_up);
		*ballY = PADDLE_Y - 16; //hopefully should stop the stuttering
		
		if(ballMid >= paddleL + 8 && ballMid <= paddleL) //more side bouncing on the ends
		{flip(ball_x_left);}

		else if(ballMid >= paddleR - 8 && ballMid <= paddleR)
		{flip(ball_x_left);}
	}
	
}






//change position of ball--this is how it moves around
void modposBounce(int * pX, int * pY, int xLeft, int yUp, int amt)
{

	if(xLeft)
	{*pX -= amt;}

	else
	{*pX += amt;} 
	


	if(yUp)
	{*pY -= amt;}
	
	else
	{*pY += amt;}


}


 

//see if the ball needs to bounce
void checkBounce(int x, int y, int * xLeft, int * yUp)
{
	if(*xLeft && x <= 0)
	{*xLeft = 0;}
	else if(!(*xLeft) && x >= SCREEN_SIDE)
	{*xLeft = 1;}

	if(*yUp && y <= 0)
	{*yUp = 0;}
	else if (!(*yUp) && y >= SCREEN_BOTTOM)
	{gameOver = 1;} //marker

}







