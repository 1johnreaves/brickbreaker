#include "sprites.h" //header file for all the sprites & stuff
#include "collision.c" //all the methods for moving stuff around, plus the TONC files go through it

#define BRICKS_MAX 28



//buffer instantiation
OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *aff_buffer= (OBJ_AFFINE*)obj_buffer; //not used here



int ball_xLeft, ball_yUp = 1; //acts as bool: is ball going left? is ball going up? y/n
int numBricks = BRICKS_MAX; //will decrement as stuff goes away
int newGame = 1; //is this a new game?
int ballSpeed = 2; //default speed; will increase on further levels





int main()
{

//=====================video/obj_buffer setup===============================

	oam_init(obj_buffer, 128); //video buffer to update OAM w/o weird animation errors
	REG_DISPCNT= DCNT_OBJ | DCNT_OBJ_1D;
	

//====================brick instantiation===========================

	memcpy(&tile_mem[4][0], brickTiles, brickTilesLen);
	memcpy(pal_obj_mem, brickPal, brickPalLen);
	u32 brick_tid= 0, brick_pb= 0;		// tile id, pal-bank

	int brick_x= 8, brick_y= 0;
	OBJ_ATTR * brickArray[numBricks];

	
	for(int i = 0; i < numBricks; i++)
	{
		OBJ_ATTR *brick= &obj_buffer[i+5];
		obj_set_attr(brick, 
			ATTR0_WIDE,				// Wide sprite
			ATTR1_SIZE_32,					// 32x16p, 
			ATTR2_PALBANK(brick_pb) | brick_tid);		// palbank 0, tile 0

		brickArray[i] = brick;


	}

	int * brickXArray[numBricks], brickYArray[numBricks];

	for(int i = 0; i < numBricks; i++)
	{
		brickXArray[i] = brick_x;
		brickYArray[i] = brick_y;
		
		brick_x = (brick_x + 32);
		if(brick_x > SCREEN_WIDTH-17) //screen width is enough room for 7.5 bricks, so gotta split them up
		{
			brick_y = (brick_y + 16) % SCREEN_HEIGHT;
			brick_x = 8;
		}
	}



//==========================paddle instantiation=====================================

int paddle_x = 100;
u32 paddle_tid= 64, paddle_pb= 0;		// tile id, pal-bank

memcpy(&tile_mem[4][64], paddleTiles, paddleTilesLen);

	OBJ_ATTR *paddle= &obj_buffer[3];
	obj_set_attr(paddle, 
		ATTR0_WIDE,				// Wide sprite
		ATTR1_SIZE_32,					// 32x16p, 
		ATTR2_PALBANK(paddle_pb) | paddle_tid);		// palbank 0, tile 0


//================ball instantiation=======================================

int ball_x = 100, ball_y = 100;
u32 ball_tid= 128, ball_pb= 0;		// tile id, pal-bank

memcpy(&tile_mem[4][128], ballTiles, ballTilesLen);

	OBJ_ATTR *ball= &obj_buffer[4];
	obj_set_attr(ball, 
		ATTR0_SQUARE,				// Square, regular sprite
		ATTR1_SIZE_16,					// 16x16p, 
		ATTR2_PALBANK(ball_pb) | ball_tid);		// palbank 0, tile 0


//================'gamestart' instantiation=======================================

int gamestart_x = (SCREEN_WIDTH - 64) / 2 - 4, gamestart_y = (SCREEN_HEIGHT - 64) / 2;
u32 gamestart_tid= 256, gamestart_pb= 0;		// tile id, pal-bank

memcpy(&tile_mem[4][256], gamestartTiles, gamestartTilesLen);

	OBJ_ATTR *gamestart= &obj_buffer[0];
	obj_set_attr(gamestart, 
		ATTR0_SQUARE,				// Square, regular sprite
		ATTR1_SIZE_64,					// 16x16p, 
		ATTR2_PALBANK(gamestart_pb) | gamestart_tid);		// palbank 0, tile 0


//================'gameend' instantiation=======================================

int gameend_x = (SCREEN_WIDTH - 64) / 2 - 4, gameend_y = (SCREEN_HEIGHT - 64) / 2;
u32 gameend_tid= 512, gameend_pb= 0;		// tile id, pal-bank

memcpy(&tile_mem[4][512], gameendTiles, gameendTilesLen);

	OBJ_ATTR *gameend= &obj_buffer[1];
	obj_set_attr(gameend, 
		ATTR0_SQUARE,				// Square, regular sprite
		ATTR1_SIZE_64,					// 16x16p, 
		ATTR2_PALBANK(gameend_pb) | gameend_tid);		// palbank 0, tile 0

obj_hide(gameend); //don't want the game over screen visible immediately

//==================='congrats' instantiation====================================

int congrats_x = (SCREEN_WIDTH - 64) / 2 - 4, congrats_y = (SCREEN_HEIGHT - 64) / 2;
u32 congrats_tid= 576, congrats_pb= 0;		// tile id, pal-bank

memcpy(&tile_mem[4][576], congratsTiles, congratsTilesLen);

	OBJ_ATTR *congrats= &obj_buffer[1];
	obj_set_attr(congrats, 
		ATTR0_SQUARE,				// Square, regular sprite
		ATTR1_SIZE_64,					// 16x16p, 
		ATTR2_PALBANK(congrats_pb) | congrats_tid);		// palbank 0, tile 0

obj_hide(congrats); //don't want the continue screen visible immediately



//=========loop time=========================
	while(1)
	{
		vid_vsync();



		//start screen
		if(newGame && !gameOver && numBricks != 0)
		{
			//if you want the menu to actually show up...well, you need these
			//placement
			gamestart->attr2= ATTR2_BUILD(gamestart_tid, gamestart_pb, 64);
			obj_set_pos(gamestart, gamestart_x, gamestart_y);


			//oam updating
			oam_copy(oam_mem, obj_buffer, 1); //here you're only loading the menu



			key_poll();
			if(key_tri_fire())
			{
				newGame = 0;
				obj_hide(gamestart);

			}
			
		}
		
		//the regular progress of the game
		if(!newGame && !gameOver && numBricks != 0)
		{
		
			key_poll(); //gotta have inputs

			paddle_x += key_tri_horz()*5; //moving the paddle around

			checkBounce(ball_x, ball_y, &ball_xLeft, &ball_yUp);
			modposBounce(&ball_x, &ball_y, ball_xLeft, ball_yUp, ballSpeed);


			if(ball_y >= (PADDLE_Y - 16)) //make sure the ball would actually hit the paddle before checking
			{paddleCollision(ball_x, &ball_y, paddle_x, &ball_xLeft, &ball_yUp, ballSpeed);}


			brickCollision(brickArray, brickXArray, brickYArray, ball_x, ball_y, &ball_xLeft, &ball_yUp, &numBricks);
		




			//brick placement
			for(int i = 0; i < numBricks; i++)
			{
				OBJ_ATTR * tempBrick = brickArray[i];
		
				tempBrick->attr2= ATTR2_BUILD(brick_tid, brick_pb, 0);
				obj_set_pos(tempBrick, brickXArray[i], brickYArray[i]);

			}
		
		



			//ball placement
			ball->attr2= ATTR2_BUILD(ball_tid, ball_pb, 64);
			obj_set_pos(ball, ball_x, ball_y);

		
			//paddle placement
			paddle->attr2= ATTR2_BUILD(paddle_tid, paddle_pb, 64);
			obj_set_pos(paddle, paddle_x, PADDLE_Y);


			//oam updating
			oam_copy(oam_mem, obj_buffer, numBricks + 10); //i have to load an extra sprite so it'll actually update the hiding part

		}

		//game over -- you dropped the ball
		if(gameOver && numBricks != 0)
		{

			obj_unhide(gameend, 0);

			gameend->attr2= ATTR2_BUILD(gameend_tid, gameend_pb, 64);
			obj_set_pos(gameend, gameend_x, gameend_y);


			//oam updating
			oam_copy(oam_mem, obj_buffer, 2); //here you're only loading the menus


			key_poll();
			if(key_tri_fire())
			{
				obj_unhide(gamestart, 0);
				ballSpeed = 2;
				gameOver = 0;
				brick_y = 0, brick_x = 8;
				newGame = ball_xLeft = ball_yUp = 1;
				ball_x = 100, ball_y = 100, paddle_x = 100;
				
				obj_hide(gameend);


				for(; numBricks < BRICKS_MAX; numBricks++) //reset the bricks for a new game
				{
					obj_unhide(brickArray[numBricks], 0);
				}

				for(int i = 0; i < numBricks; i++)
				{
					brickXArray[i] = brick_x;
					brickYArray[i] = brick_y;
		
					brick_x = (brick_x + 32);
					if(brick_x > SCREEN_WIDTH-17) //screen width is enough room for 7.5 bricks, so gotta split them up
					{
						brick_y = (brick_y + 16) % SCREEN_HEIGHT;
						brick_x = 8;
					}
				}


				//ball placement
				ball->attr2= ATTR2_BUILD(ball_tid, ball_pb, 64);
				obj_set_pos(ball, ball_x, ball_y);

		
				//paddle placement
				paddle->attr2= ATTR2_BUILD(paddle_tid, paddle_pb, 64);
				obj_set_pos(paddle, paddle_x, PADDLE_Y);

	
				//oam updating
				oam_copy(oam_mem, obj_buffer, numBricks + 10); //i have to load an extra sprite so it'll actually update the hiding part

			}
		}


		//next level! woot!
		if(numBricks == 0 && !gameOver)
		{
			obj_unhide(congrats, 0);

			congrats->attr2= ATTR2_BUILD(congrats_tid, congrats_pb, 64);
			obj_set_pos(congrats, congrats_x, congrats_y);


			//oam updating
			oam_copy(oam_mem, obj_buffer, 2); //here you're only loading the menus


			key_poll();
			if(key_tri_fire())
			{
				obj_unhide(gamestart, 0);
				gameOver = 0;
				brick_y = 0, brick_x = 8;
				newGame = ball_xLeft = ball_yUp = 1;
				ball_x = 100, ball_y = 100, paddle_x = 100;
				
				obj_hide(congrats);
				ballSpeed += 1; //increment speed of ball; faster later on

				for(; numBricks < BRICKS_MAX; numBricks++) //reset the bricks for a new game
				{
					obj_unhide(brickArray[numBricks], 0);
				}

				for(int i = 0; i < numBricks; i++)
				{
					brickXArray[i] = brick_x;
					brickYArray[i] = brick_y;
		
					brick_x = (brick_x + 32);
					if(brick_x > SCREEN_WIDTH-17) //screen width is enough room for 7.5 bricks, so gotta split them up
					{
						brick_y = (brick_y + 16) % SCREEN_HEIGHT;
						brick_x = 8;
					}
				}


				//ball placement
				ball->attr2= ATTR2_BUILD(ball_tid, ball_pb, 64);
				obj_set_pos(ball, ball_x, ball_y);

		
				//paddle placement
				paddle->attr2= ATTR2_BUILD(paddle_tid, paddle_pb, 64);
				obj_set_pos(paddle, paddle_x, PADDLE_Y);

	
				//oam updating
				oam_copy(oam_mem, obj_buffer, numBricks + 10); //i have to load an extra sprite so it'll actually update the hiding part
			}
		}
	}

	return 0;
}


