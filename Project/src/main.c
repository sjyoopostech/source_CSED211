#include "lcd.h"
#include "s3c_timer.h"
#include "interrupt.h"
#include "graphics.h"

#include "res-gg.h"
#include "res-item.h"
#include "res-map.h"
#include "res-pm.h"
#include "res-score.h"
#include "res-board.h"

#define WIDTH 21
#define HEIGHT 17

int direction;
int cnt[4] = {0, 0, 0, 0};
int old_x, old_y;
int status_dir;

#define I (32)
#define J (42)
#define UP (3)
#define DOWN (1)
#define LEFT (2)
#define RIGHT (4)
#define N (4)
#define MODE (120)
#define PLAYER_I (28)
#define PLAYER_J (21)
#define GHOST_I (11)
#define GHOST_J (21)

typedef struct
{
	int i, j;
	int i_offset, j_offset;
	int dir_now, dir_next;
	int mode;
	int speed;
}Player;

typedef struct
{
	int valid;
	int i, j;
	int i_offset, j_offset;
	int i_dest, j_dest;
	int dir_now, dir_next;
	int speed;
}Ghost;

void Init(const int datamap[][J], int map[][J], Player* player, Ghost* ghost, int level, int* coin_cnt_left)
{
	int i, j;

	for (i = 0; i < I; i++)
	{
		for (j = 0; j < J; j++)
		{
			map[i][j] = datamap[i][j];

			if (map[i][j] == 1)(*coin_cnt_left)++;
		}
	}

	(*player).i = PLAYER_I; (*player).j = PLAYER_J;
	(*player).i_offset = 0; (*player).j_offset = 0;
	(*player).dir_now = 0; (*player).dir_next = 0;
	(*player).mode = 0; (*player).speed = 4;

	for (i = 0; i < N; i++)
	{
		ghost[i].i = GHOST_I; ghost[i].j = GHOST_J;
		ghost[i].i_offset = 0; ghost[i].j_offset = 0;
		ghost[i].valid = 0; ghost[i].dir_now = UP; ghost[i].dir_next = UP;
	}

	if (level == 1)
	{
		ghost[0].speed = 10;
		ghost[0].valid = 1;
	}
	else if (level == 2)
	{
		ghost[0].speed = 10;
		ghost[1].speed = 8;
		ghost[0].valid = 1;
		ghost[1].valid = 1;
	}
	else if (level == 3)
	{
		ghost[0].speed = 10;
		ghost[1].speed = 8;
		ghost[2].speed = 6;
		ghost[0].valid = 1;
		ghost[1].valid = 1;
		ghost[2].valid = 1;
	}
	else if (level == 4)
	{
		ghost[0].speed = 10;
		ghost[1].speed = 8;
		ghost[2].speed = 6;
		ghost[3].speed = 4;
		ghost[0].valid = 1;
		ghost[1].valid = 1;
		ghost[2].valid = 1;
		ghost[3].valid = 1;
	}
}

void Input(Player* player)
{
	int a;

	a = direction;

	if (a == 3) { if (player->dir_now == 0) { player->dir_now = UP; player->dir_next = UP; } else player->dir_next = UP; }
	else if (a == 1) { if (player->dir_now == 0) { player->dir_now = DOWN; player->dir_next = DOWN; } else player->dir_next = DOWN; }
	else if (a == 2) { if (player->dir_now == 0) { player->dir_now = LEFT; player->dir_next = LEFT; } else player->dir_next = LEFT; }
	else if (a == 0) { if (player->dir_now == 0) { player->dir_now = RIGHT; player->dir_next = RIGHT; } else player->dir_next = RIGHT; }
}

void offsetupdate(int map[][J], Player* player, Ghost* ghost)
{
	int i;
	/*direction update*/
	if (player->dir_next != player->dir_now)
	{
		if (player->dir_next == UP)
		{
			if (player->j_offset == 0 && map[player->i - 1][player->j] < 3)
			{
				player->dir_now = UP;
			}
		}
		else if (player->dir_next == DOWN)
		{
			if (player->j_offset == 0 && map[player->i + 1][player->j] < 3)
			{
				player->dir_now = DOWN;
			}
		}
		else if (player->dir_next == LEFT)
		{
			if (player->i_offset == 0 && map[player->i][player->j - 1] < 3)
			{
				player->dir_now = LEFT;
			}
		}
		else
		{
			if (player->i_offset == 0 && map[player->i][player->j + 1] < 3)
			{
				player->dir_now = RIGHT;
			}
		}
	}
	
	/*move*/
	
	if (player->dir_now == UP|| player->dir_now == DOWN)
	{
		if (player->dir_now == UP)
		{
			if (map[player->i - 1][player->j] < 3)
				player->i_offset--;
		}
		else
		{
			if (map[player->i + 1][player->j] < 3)
				player->i_offset++;
		}
		if (player->i_offset == (-1)*(player->speed) / 2)
		{
			player->i--;
			player->i_offset = 0;
		}
		else if (player->i_offset == player->speed / 2 )
		{
			player->i++;
			player->i_offset = 0;
		}
	}
	else
	{
		if (player->dir_now == LEFT)
		{
			if (map[player->i][player->j] == -10)
			{
				player->j = J - 1;
				player->j_offset = 0;
			}
			else if (map[player->i][player->j - 1] < 3)
				player->j_offset--;
		}
		else
		{
			if (map[player->i][player->j] == -11)
			{
				player->j = 0;
				player->j_offset = 0;
			}
			if (map[player->i][player->j + 1] < 3)
				player->j_offset++;
		}

		if (player->j_offset == (-1)*(player->speed) / 2)
		{
			player->j--;
			player->j_offset = 0;
		}
		else if (player->j_offset == player->speed / 2 )
		{
			player->j++;
			player->j_offset = 0;
		}
	}

	/*ghost*/
	for (i = 0; i < N; i++)
	{
		if (ghost[i].valid == 0) continue;

		ghost[i].i_dest = player->i;
		ghost[i].j_dest = player->j;
		
		if (ghost[i].i_dest < ghost[i].i)
		{
			ghost[i].dir_now = UP;
			if (ghost[i].j_dest < ghost[i].j)
				ghost[i].dir_next = LEFT;
			else if (ghost[i].j_dest > ghost[i].j)
				ghost[i].dir_next = RIGHT;
		}
		else if (ghost[i].i_dest > ghost[i].i)
		{
			ghost[i].dir_now = DOWN;
			if (ghost[i].j_dest < ghost[i].j)
				ghost[i].dir_next = LEFT;
			else if (ghost[i].j_dest > ghost[i].j)
				ghost[i].dir_next = RIGHT;
		}
		else
		{
			if (ghost[i].j_dest < ghost[i].j)
				ghost[i].dir_now = ghost[i].dir_next = LEFT;
			else
				ghost[i].dir_now = ghost[i].dir_next = RIGHT;
		}

		///////////////////////////////


		if (ghost[i].dir_now == UP && ghost[i].dir_next == RIGHT && map[ghost[i].i - 1][ghost[i].j] > 2)
		{
			ghost[i].dir_now = ghost[i].dir_next = RIGHT;
		}
		else if (ghost[i].dir_now == DOWN && ghost[i].dir_next == RIGHT && map[ghost[i].i + 1][ghost[i].j] > 2)
		{
			ghost[i].dir_now = ghost[i].dir_next = RIGHT;
		}
		else if (ghost[i].dir_now == DOWN && ghost[i].dir_next == LEFT && map[ghost[i].i + 1][ghost[i].j] > 2)
		{
			ghost[i].dir_now = ghost[i].dir_next = LEFT;
		}
		else if (ghost[i].dir_now == UP && ghost[i].dir_next == LEFT && map[ghost[i].i - 1][ghost[i].j] > 2)
		{
			ghost[i].dir_now = ghost[i].dir_next = LEFT;
		}

		///////////////////////////

		if (ghost[i].valid == 1)
		{
			if (ghost[i].dir_now == UP || ghost[i].dir_now == DOWN)
			{
				if (ghost[i].dir_now == UP)
				{
					if (map[ghost[i].i - 1][ghost[i].j] < 3 && ghost[i].j_offset == 0)
						ghost[i].i_offset--;
				}
				else
				{
					if (map[ghost[i].i + 1][ghost[i].j] < 3&& ghost[i].j_offset == 0)
						ghost[i].i_offset++;
				}

				if (ghost[i].i_offset == (-1)*(ghost[i].speed) / 2 && ghost[i].j_offset == 0)
				{
					ghost[i].i--;
					ghost[i].i_offset = 0;
				}
				else if (ghost[i].i_offset == ghost[i].speed / 2 && ghost[i].j_offset == 0)
				{
					ghost[i].i++;
					ghost[i].i_offset = 0;
				}
			}
			else
			{
				if (ghost[i].dir_now == LEFT)
				{
					if (map[ghost[i].i][ghost[i].j - 1] < 3&& ghost[i].i_offset == 0)
						ghost[i].j_offset--;
				}
				else
				{
					if (map[ghost[i].i][ghost[i].j + 1] < 3&& ghost[i].i_offset == 0)
						ghost[i].j_offset++;
				}
				if (ghost[i].j_offset == (-1)*(ghost[i].speed) / 2&& ghost[i].i_offset == 0)
				{
					ghost[i].j--;
					ghost[i].j_offset = 0;
				}
				else if (ghost[i].j_offset == ghost[i].speed / 2&& ghost[i].i_offset == 0)
				{
					ghost[i].j++;
					ghost[i].j_offset = 0;
				}
			}
		}

	}
}

void colcheck(int map[][J], Player* player, Ghost* ghost, int*score, int *life, int *coin_cnt_left)
{
	int i, IsCol = 0;

	if (map[player->i][player->j] == 1)
	{
		map[player->i][player->j] = 0;
		(*coin_cnt_left)--;
		(*score) += 10;
	}
	else if (map[player->i][player->j] == 2)
	{
		player->mode = MODE;
		map[player->i][player->j] = 0;
		(*score) += 50;
	}

	for (i = 0; i < N; i++)
	{
		if (!ghost[i].valid) continue;
		if (ghost[i].valid > 1) 
		{ printf("%d\n", ghost[i].valid);  ghost[i].valid--; if (ghost[i].valid == 1) { ghost[i].i = GHOST_I; ghost[i].j = GHOST_J; ghost[i].i_offset = ghost[i].j_offset = 0; continue; } }
		if ((*player).i == ghost[i].i && (*player).j == ghost[i].j)
		{
			IsCol = 1;
			break;
		}
	}

	if(IsCol)
	{
		/*normal*/
		if (!(*player).mode)
		{
			(*life)--;

			player->i = PLAYER_I;
			player->j = PLAYER_J;

			int xxx = 0;
			for(;xxx<4;xxx++){

			
				ghost[xxx].i = GHOST_I;
				ghost[xxx].j = GHOST_J;
				ghost[xxx].i_offset = 0;
				ghost[xxx].j_offset = 0;
			}

			player->i_offset = player->j_offset = 0;
		}
		/*fellet*/
		else
		{
			ghost[i].valid = 10;

			map[(*player).i][(*player).j] = 0;
			(*score) += 200;
		}
	}



	if (player->mode)player->mode--;
}

int gamecheck(int* life, int* coin_cnt_left)
{
	if (*life == 0) return 1;
	if (*coin_cnt_left == 0) return 2;
	return 0;
}

int proc(int map[][J], Player* player, Ghost* ghost, int level, int*score, int *life, int *coin_cnt_left)
{
	offsetupdate(map, player, ghost);
	
	colcheck(map, player,  ghost, score, life, coin_cnt_left);

	return gamecheck(life, coin_cnt_left);
}





unsigned char* dmap[36] = {
  map_blank,//0
  map_pacdot,//1
  map_powerpellet,//2
  map_cd_left_top,//3
  map_cd_left_bottom,//4
  map_cd_right_top,//5
  map_cd_right_bottom,//6
  map_cd_top,//7
  map_rd_right,//8
  map_cd_bottom,//9
  map_rd_left,//10
  map_cd_right_upper,//11
  map_cd_left_upper,//12
  map_cd_right_lower,//13
  map_cd_left_lower,//14
  map_c_left_top,//15
  map_c_left_bottom,//16
  map_c_right_top,//17
  map_c_right_bottom,//18
  map_c_top,//19
  map_c_right,//20
  map_c_bottom,//21
  map_c_left,//22
  map_rd_left_top,//23
  map_rd_right_top,//24
  map_rd_left_bottom,//25
  map_rd_right_bottom,//26
  map_rd_top,//27
  map_rd_right,//28
  map_rd_bottom,//29
  map_rd_left,//30
  map_rd_door_left,//31
  map_rd_door_right,//32
  map_rd_door,//33
  map_cd_upper_right,//34
  map_cd_upper_left//35
  };

static int unmap[32][42] = {
  /*00*/{ 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,34,35, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 5},
  /*01*/{10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,22,20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8},
  /*02*/{10, 0,15,19,19,19,17, 0,15,19,19,19,19,19,17, 0,15,19,17, 0,22,20, 0,15,19,17, 0,15,19,19,19,19,19,17, 0,15,19,19,19,17, 0, 8},
  /*03*/{10, 0,22, 0, 0, 0,20, 0,22, 0, 0, 0, 0, 0,20, 0,22, 0,20, 0,22,20, 0,22, 0,20, 0,22, 0, 0, 0, 0, 0,20, 0,22, 0, 0, 0,20, 0, 8},
  /*04*/{10, 0,16,21,21,21,18, 0,16,21,21,21,21,21,18, 0,16,21,18, 0,22,20, 0,16,21,18, 0,16,21,21,21,21,21,18, 0,16,21,21,21,18, 0, 8},
  /*05*/{10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,22,20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8},
  /*06*/{10, 0,15,19,19,19,19,19,19,17, 0,15,17, 0,15,19,19,19,17, 0,22,20, 0,15,19,19,19,17, 0,15,17, 0,15,19,19,19,19,19,19,17, 0, 8},
  /*07*/{10, 0,16,21,21,21,21,21,21,18, 0,22,20, 0,16,21,21,21,18, 0,16,18, 0,16,21,21,21,18, 0,22,20, 0,16,21,21,21,21,21,21,18, 0, 8},
  /*08*/{10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,22,20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,22,20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8},
  /*09*/{ 4, 9, 9, 9, 9, 9,17, 0,15,19,19,18,20, 0,15,19,19,19,19,19,19,19,19,19,19,19,19,17, 0,22,16,19,19,17, 0,15, 9, 9, 9, 9, 9, 6},
  /*10*/{ 0, 0, 0, 0, 0, 0,10, 0,16,21,21,17,20, 0,16,21,21,21,21,21,21,21,21,21,21,21,21,18, 0,22,15,21,21,18, 0, 8, 0, 0, 0, 0, 0, 0},
  /*11*/{ 0, 0, 0, 0, 0, 0,10, 0, 0, 0, 0,22,20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,22,20, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0},
  /*12*/{ 0, 0, 0, 0, 0, 0,10, 0,15,17, 0,22,20, 0,15,19,17, 0,23,27,27,31,33,33,32,27,27,24, 0,22,20, 0,15,17, 0, 8, 0, 0, 0, 0, 0, 0},
  /*13*/{ 7, 7, 7, 7, 7, 7,18, 0,22,20, 0,16,18, 0,22, 0,20, 0,28, 0, 0, 0, 0, 0, 0, 0, 0,30, 0,16,18, 0,22,20, 0,16, 7, 7, 7, 7, 7, 7},
  /*14*/{ 0, 0, 0, 0, 0, 0, 0, 0,22,20, 0, 0, 0, 0,22, 0,20, 0,28, 0, 0, 0, 0, 0, 0, 0, 0,30, 0, 0, 0, 0,22,20, 0, 0, 0, 0, 0, 0, 0, 0},
  /*15*/{ 9, 9, 9, 9, 9, 9,17, 0,22,16,19,19,17, 0,16,21,18, 0,25,29,29,29,29,29,29,29,29,26, 0,15,19,19,18,20, 0,15, 9, 9, 9, 9, 9, 9},
  /*16*/{ 0, 0, 0, 0, 0, 0,10, 0,22,15,21,21,18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,16,21,21,17,20, 0, 8, 0, 0, 0, 0, 0, 0},
  /*17*/{ 0, 0, 0, 0, 0, 0,10, 0,22,20, 0, 0, 0, 0,15,19,17, 0,15,19,19,19,19,17, 0,15,19,17, 0, 0, 0, 0,22,20, 0, 8, 0, 0, 0, 0, 0, 0},
  /*18*/{ 0, 0, 0, 0, 0, 0,10, 0,22,20, 0,15,17, 0,22, 0,20, 0,22, 0, 0, 0, 0,20, 0,22, 0,20, 0,15,17, 0,22,20, 0, 8, 0, 0, 0, 0, 0, 0},
  /*19*/{ 3, 7, 7, 7, 7, 7,18, 0,16,18, 0,22,20, 0,22, 0,20, 0,22, 0, 0, 0, 0,20, 0,22, 0,20, 0,22,20, 0,16,18, 0,16, 7, 7, 7, 7, 7, 5},
  /*20*/{10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,22,20, 0,22, 0,20, 0,22, 0, 0, 0, 0,20, 0,22, 0,20, 0,22,20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8},
  /*21*/{10, 0,15,19,19,19,19,19,17, 0,15,18,20, 0,22, 0,20, 0,22, 0, 0, 0, 0,20, 0,22, 0,20, 0,22,16,17, 0,15,19,19,19,19,19,17, 0, 8},
  /*22*/{10, 0,16,21,21,21,21,21,18, 0,16,17,20, 0,22, 0,20, 0,22, 0, 0, 0, 0,20, 0,22, 0,20, 0,22,15,18, 0,16,21,21,21,21,21,18, 0, 8},
  /*23*/{10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,22,20, 0,22, 0,20, 0,22, 0, 0, 0, 0,20, 0,22, 0,20, 0,22,20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8},
  /*24*/{14,19,19,19,19,17, 0,15,19,17, 0,22,20, 0,22, 0,20, 0,22, 0, 0, 0, 0,20, 0,22, 0,20, 0,22,20, 0,15,19,17, 0,15,19,19,19,19,13},
  /*25*/{12,21,21,21,21,18, 0,22, 0,20, 0,22,20, 0,22, 0,20, 0,22, 0, 0, 0, 0,20, 0,22, 0,20, 0,22,20, 0,22, 0,20, 0,16,21,21,21,21,11},
  /*26*/{10, 0, 0, 0, 0, 0, 0,22, 0,20, 0,16,18, 0,16,21,18, 0,16,21,21,21,21,18, 0,16,21,18, 0,16,18, 0,22, 0,20, 0, 0, 0, 0, 0, 0, 8},
  /*27*/{10, 0,15,19,19,17, 0,22, 0,20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,22, 0,20, 0,15,19,19,17, 0, 8},
  /*28*/{10, 0,22, 0, 0,20, 0,22, 0,20, 0,15,19,19,19,19,19,19,19,19,17, 0,15,19,19,19,19,19,19,19,17, 0,22, 0,20, 0,22, 0, 0,20, 0, 8},
  /*29*/{10, 0,16,21,21,18, 0,16,21,18, 0,16,21,21,21,21,21,21,21,21,18, 0,16,21,21,21,21,21,21,21,18, 0,16,21,18, 0,16,21,21,18, 0, 8},
  /*30*/{10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8},
  /*31*/{ 4, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 6}
};

const static int dxmap[I][J] = {
		{ 3,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,34,35,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,5 },
		{ 10,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,22,20,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,8 },
		{ 10,0,15,19,19,19,17,0,15,19,19,19,19,19,17,0,15,19,17,0,22,20,0,15,19,17,0,15,19,19,19,19,19,17,0,15,19,19,19,17,0,8 },
		{ 10,0,22,0,0,0,20,0,22,0,0,0,0,0,20,0,22,0,20,0,22,20,0,22,0,20,0,22,0,0,0,0,0,20,0,22,0,0,0,20,0,8 },
		{ 10,0,16,21,21,21,18,0,16,21,21,21,21,21,18,0,16,21,18,0,22,20,0,16,21,18,0,16,21,21,21,21,21,18,0,16,21,21,21,18,0,8 },
		{ 10,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,22,20,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,8 },
		{ 10,0,15,19,19,19,19,19,19,17,0,15,17,0,15,19,19,19,17,0,22,20,0,15,19,19,19,17,0,15,17,0,15,19,19,19,19,19,19,17,0,8 },
		{ 10,0,16,21,21,21,21,21,21,18,0,22,20,0,16,21,21,21,18,0,16,18,0,16,21,21,21,18,0,22,20,0,16,21,21,21,21,21,21,18,0,8 },
		{ 10,0,0,0,0,0,0,0,0,0,0,22,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,20,0,0,0,0,0,0,0,0,0,0,8 },
		{ 4,9,9,9,9,9,17,0,15,19,19,18,20,0,15,19,19,19,19,19,19,19,19,19,19,19,19,17,0,22,16,19,19,17,0,15,9,9,9,9,9,6 },
		{ 0,0,0,0,0,0,10,0,16,21,21,17,20,0,16,21,21,21,21,21,21,21,21,21,21,21,21,18,0,22,15,21,21,18,0,8,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,10,0,0,0,0,22,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,20,0,0,0,0,8,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,10,0,15,17,0,22,20,0,15,19,17,0,23,27,27,31,33,33,32,27,27,24,0,22,20,0,15,17,0,8,0,0,0,0,0,0 },
		{ 7,7,7,7,7,7,18,0,22,20,0,16,18,0,22,0,20,0,28,0,0,0,0,0,0,0,0,30,0,16,18,0,22,20,0,16,7,7,7,7,7,7 },
		{ 0,0,0,0,0,0,0,0,22,20,0,0,0,0,22,0,20,0,28,0,0,0,0,0,0,0,0,30,0,0,0,0,22,20,0,0,0,0,0,0,0,0 },
		{ 9,9,9,9,9,9,17,0,22,16,19,19,17,0,16,21,18,0,25,29,29,29,29,29,29,29,29,26,0,15,19,19,18,20,0,15,9,9,9,9,9,9 },
		{ 0,0,0,0,0,0,10,0,22,15,21,21,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,21,21,17,20,0,8,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,10,0,22,20,0,0,0,0,15,19,17,0,15,19,19,19,19,17,0,15,19,17,0,0,0,0,22,20,0,8,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,10,0,22,20,0,15,17,0,22,0,20,0,22,0,0,0,0,20,0,22,0,20,0,15,17,0,22,20,0,8,0,0,0,0,0,0 },
		{ 3,7,7,7,7,7,18,0,16,18,0,22,20,0,22,0,20,0,22,0,0,0,0,20,0,22,0,20,0,22,20,0,16,18,0,16,7,7,7,7,7,5 },
		{ 10,0,0,0,0,0,0,0,0,0,0,22,20,0,22,0,20,0,22,0,0,0,0,20,0,22,0,20,0,22,20,0,0,0,0,0,0,0,0,0,0,8 },
		{ 10,0,15,19,19,19,19,19,17,0,15,18,20,0,22,0,20,0,22,0,0,0,0,20,0,22,0,20,0,22,16,17,0,15,19,19,19,19,19,17,0,8 },
		{ 10,0,16,21,21,21,21,21,18,0,16,17,20,0,22,0,20,0,22,0,0,0,0,20,0,22,0,20,0,22,15,18,0,16,21,21,21,21,21,18,0,8 },
		{ 10,0,0,0,0,0,0,0,0,0,0,22,20,0,22,0,20,0,22,0,0,0,0,20,0,22,0,20,0,22,20,0,0,0,0,0,0,0,0,0,0,8 },
		{ 14,19,19,19,19,17,0,15,19,17,0,22,20,0,22,0,20,0,22,0,0,0,0,20,0,22,0,20,0,22,20,0,15,19,17,0,15,19,19,19,19,13 },
		{ 12,21,21,21,21,18,0,22,0,20,0,22,20,0,22,0,20,0,22,0,0,0,0,20,0,22,0,20,0,22,20,0,22,0,20,0,16,21,21,21,21,11 },
		{ 10,0,0,0,0,0,0,22,0,20,0,16,18,0,16,21,18,0,16,21,21,21,21,18,0,16,21,18,0,16,18,0,22,0,20,0,0,0,0,0,0,8 },
		{ 10,0,15,19,19,17,0,22,0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,20,0,15,19,19,17,0,8 },
		{ 10,0,22,0,0,20,0,22,0,20,0,15,19,19,19,19,19,19,19,19,17,0,15,19,19,19,19,19,19,19,17,0,22,0,20,0,22,0,0,20,0,8 },
		{ 10,0,16,21,21,18,0,16,21,18,0,16,21,21,21,21,21,21,21,21,18,0,16,21,21,21,21,21,21,21,18,0,16,21,18,0,16,21,21,18,0,8 },
		{ 10,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,8 },
		{ 4,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,6 }
	};

void initial_env(void) {
  disable_interrupts();
  send_x = 0;
  send_y = 0;
  lcd_init();
  gfx_init();
  mango_interrupt_init();
  enable_interrupts();
}

void change_dir(void) {
  int sub[4] = {0, 0, 0, 0};
  int max, index;
  int i;

  sub[0] = send_x - old_x;
  sub[1] = send_y - old_y;
  sub[2] = -sub[0];
  sub[3] = -sub[1];
  max = sub[0];
  index = 0;
  for (i = 1; i < 4; i++) {
    if (sub[i] > max) {
      index = i;
      max = sub[i];
    }
  }
  if (max < 3) index = -1;
  for (i = 0; i < 4; i++) {
    if (index != i) cnt[i] = 0;
  }
  if (index != -1) {
    cnt[index]++;
    if (cnt[index] >= 3) direction = index;
  }
}

void draw_image(int x, int y, int w, int h, unsigned char *data) {
  unsigned char *phy_addr = FB_ADDR;
  int i, j, k;
  int index = 0;
  for (i = y; i < y+h; i++) {
    for (j = x; j < x+w; j++) {
      for (k = 0; k < 3; k++) {
        *(phy_addr+800*4*i+4*j+k) = *(data+index);
        index++;
      }
      //*(phy_addr+800*4*i+4*j+3) = 0x0;
    }
  }
  set_wincon0_enable();
  set_vidcon0_enable();
}


void draw_image_without_blackbg(int x, int y, int w, int h, unsigned char *data) {
  unsigned char *phy_addr = FB_ADDR;
  int i, j, k;
  int index = 0;
  for (i = y; i < y+h; i++) {
    for (j = x; j < x+w; j++) {
      if ((*(data+index)) || (*(data+index+1)) || (*(data+index+2))) {
	for (k = 0; k < 3; k++) {
	  *(phy_addr+800*4*i+4*j+k) = *(data+index);
          index++;
	}
      }
      else index += 3;
    }
  }
  set_wincon0_enable();
  set_vidcon0_enable();
}

void draw_image_with_otherbg(int x, int y, int w, int h, unsigned char *data, unsigned char bg) {
  unsigned char *phy_addr = FB_ADDR;
  int i, j, k;
  int index = 0;
  for (i = y; i < y+h; i++) {
    for (j = x; j < x+w; j++) {
      for (k = 0; k < 3; k++) {
	if(*(data+index) != 0)
        	*(phy_addr+800*4*i+4*j+k) = *(data+index);
	else
		*(phy_addr+800*4*i+4*j+k) = bg;
        index++;
      }
    }
  }
  set_wincon0_enable();
  set_vidcon0_enable();
}

void draw_image_with_color(int x, int y, int w, int h, unsigned char bg) {
  unsigned char *phy_addr = FB_ADDR;
  int i, j, k;
  int index = 0;
  for (i = y; i < y+h; i++) {
    for (j = x; j < x+w; j++) {
      for (k = 0; k < 3; k++) {
	*(phy_addr+800*4*i+4*j+k) = bg;
        index++;
      }
      *(phy_addr+800*4*i+4*j+3) = 0x0;
    }
  }
  set_wincon0_enable();
  set_vidcon0_enable();
}

void die(int befx, int befy, int level, int life) {

	drawgg_red(0, 0, 0, 0, -1, 1);
	if( level > 1) drawgg_blue(0, 0, 0, 0, -1, 1);
if( level > 2) drawgg_yellow(0, 0, 0, 0, -1, 1);
if( level > 3) drawgg_pink(0, 0, 0, 0, -1, 1);
  draw_image(befx, befy, 30, 30, pm_die_0);
  mdelay(100);
  draw_image(befx, befy, 30, 30, pm_die_1);
  mdelay(100);
  draw_image(befx, befy, 30, 30, pm_die_2);
  mdelay(100);
  draw_image(befx, befy, 30, 30, pm_die_3);
  mdelay(100);
  draw_image(befx, befy, 30, 30, pm_die_4);
  mdelay(100);
  draw_image(befx, befy, 30, 30, pm_die_5);
  mdelay(100);
  draw_image(befx, befy, 30, 30, pm_die_6);
  mdelay(100);
  draw_image(befx, befy, 30, 30, pm_die_7);
  mdelay(100);
  draw_image(befx, befy, 30, 30, pm_die_8);
  mdelay(100);
  draw_image(befx, befy, 30, 30, pm_die_9);
  mdelay(100);
  draw_image(befx, befy, 30, 30, pm_die_a);
  mdelay(1400);
	drawlife(life);
  //draw_image_with_color(befx, befy, 30, 30, 0);
}



void drawgg_red(int pacx, int pacy, int dir, int status, int mode, int opt) {

	static int befx = 0, befy = 0;
	if(befx){
		if (opt) {
		draw_image_with_color(befx, befy, 30, 30, 0);
		return;
		}
}
if(mode == -1)
	return;


	befx = pacx;
	befy = pacy;
	if(mode==0) {

	switch(dir) {

	case 0: //right
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_blinky_right_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_blinky_right_1);
		break;
	case 1: //down
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_blinky_down_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_blinky_down_1);
		break;
	case 2: //left
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_blinky_left_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_blinky_left_1);
		break;
	case 3: //up
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_blinky_up_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_blinky_up_1);
		break;
	}
	}
	else if(mode > 45) {
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_scared_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_scared_1);
	} else if ((mode/5)%2){
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_unscared_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_unscared_1);
	} else {
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_scared_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_scared_1);
	}
	//Recover pacdot & powerpellet
	
}

void drawgg_pink(int pacx, int pacy, int dir, int status, int mode, int opt) {

	static int befx = 0, befy = 0;
	if(befx){
		if (opt) {
		draw_image_with_color(befx, befy, 30, 30, 0);
		return;
		}
}
if(mode == -1)
	return;

	befx = pacx;
	befy = pacy;
	if(mode==0) {
	switch(dir) {

	case 0: //right
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_pinky_right_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_pinky_right_1);
		break;
	case 1: //down
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_pinky_down_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_pinky_down_1);
		break;
	case 2: //left
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_pinky_left_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_pinky_left_1);
		break;
	case 3: //up
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_pinky_up_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_pinky_up_1);
		break;
	}	
}
	else if(mode > 45) {
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_scared_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_scared_1);
	} else if ((mode/5)%2){
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_unscared_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_unscared_1);
	} else {
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_scared_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_scared_1);
	}
}


void drawgg_blue(int pacx, int pacy, int dir, int status, int mode, int opt) {

	static int befx = 0, befy = 0;
	if(befx){
		if (opt) {
		draw_image_with_color(befx, befy, 30, 30, 0);
		return;
		}
}
if(mode == -1)
	return;

	befx = pacx;
	befy = pacy;
	if(mode==0) {
	switch(dir) {

	case 0: //right
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_inky_right_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_inky_right_1);
		break;
	case 1: //down
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_inky_down_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_inky_down_1);
		break;
	case 2: //left
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_inky_left_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_inky_left_1);
		break;
	case 3: //up
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_inky_up_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_inky_up_1);
		break;
	}	
}
	else if(mode > 45) {
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_scared_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_scared_1);
	} else if ((mode/5)%2){
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_unscared_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_unscared_1);
	} else {
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_scared_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_scared_1);
	}
}

void drawgg_yellow(int pacx, int pacy, int dir, int status, int mode, int opt) {

	static int befx = 0, befy = 0;
	if(befx){
		if (opt) {
		draw_image_with_color(befx, befy, 30, 30, 0);
		return;
		}
}
if(mode == -1)
	return;

	befx = pacx;
	befy = pacy;
	if(mode==0) {
	switch(dir) {

	case 0: //right
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_clyde_right_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_clyde_right_1);
		break;
	case 1: //down
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_clyde_down_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_clyde_down_1);
		break;
	case 2: //left
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_clyde_left_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_clyde_left_1);
		break;
	case 3: //up
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_clyde_up_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_clyde_up_1);
		break;
	}	
}
	else if(mode > 45) {
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_scared_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_scared_1);
	} else if ((mode/5)%2){
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_unscared_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_unscared_1);
	} else {
		if (status % 2 == 0) draw_image_without_blackbg(befx, befy, 30, 30, gg_scared_0);
		else draw_image_without_blackbg(befx, befy, 30, 30, gg_scared_1);
	}
}

void drawpm(int pacx, int pacy, int dir, int status, int life, int level) {

	static int befx = 0, befy = 0;
	static int beflife = 3;

	if (beflife != life){
		printf("nowlife=%d(befx=%d&befy=%d)!\n", life,befx,befy);
		die(befx, befy, level, life);
		beflife = life;
		return;
	}

	if (befx) draw_image_with_color(befx, befy, 30, 30, 0);

	befx = pacx;
	befy = pacy;


	switch(dir) {

	case 0: //right
		if (status % 4 == 0) draw_image_without_blackbg(befx, befy, 30, 30, pm_right_0);
		else if (status % 4 == 1 || status % 4 == 3) draw_image_without_blackbg(befx, befy, 30, 30, pm_right_1);
		else draw_image_without_blackbg(befx, befy, 30, 30, pm_right_2);
		break;
	case 1: //down
		if (status % 4 == 0) draw_image_without_blackbg(befx, befy, 30, 30, pm_down_0);
		else if (status % 4 == 1 || status % 4 == 3) draw_image_without_blackbg(befx, befy, 30, 30, pm_down_1);
		else draw_image_without_blackbg(befx, befy, 30, 30, pm_down_2);
		break;
	case 2: //left
		if (status % 4 == 0) draw_image_without_blackbg(befx, befy, 30, 30, pm_left_0);
		else if (status % 4 == 1 || status % 4 == 3) draw_image_without_blackbg(befx, befy, 30, 30, pm_left_1);
		else draw_image_without_blackbg(befx, befy, 30, 30, pm_left_2);
		break;
	case 3: //up
		if (status % 4 == 0) draw_image_without_blackbg(befx, befy, 30, 30, pm_up_0);
		else if (status % 4 == 1 || status % 4 == 3) draw_image_without_blackbg(befx, befy, 30, 30, pm_up_1);
		else draw_image_without_blackbg(befx, befy, 30, 30, pm_up_2);
		break;
	}	
}
static unsigned char* numlist[10] = {board_0, board_1, board_2, board_3, board_4, board_5, board_6, board_7, board_8, board_9};

void updatescoredisp(int score) {

  static int dish = 45;

  draw_image_with_color(25, dish, 120, 15, 20);
  //printf("score=%d\n", score);

  if(score>=10000000) {
	draw_image_with_otherbg(25, dish, 15, 15, numlist[score / 10000000], 20);
	score %= 10000000;
	draw_image_with_otherbg(40, dish, 15, 15, numlist[score / 1000000], 20);
	score %= 1000000;
	draw_image_with_otherbg(55, dish, 15, 15, numlist[score / 100000], 20);
	score %= 100000;
	draw_image_with_otherbg(70, dish, 15, 15, numlist[score / 10000], 20);
	score %= 10000;
	draw_image_with_otherbg(85, dish, 15, 15, numlist[score / 1000], 20);
	score %= 1000;
	draw_image_with_otherbg(100, dish, 15, 15, numlist[score / 100], 20);
	score %= 100;
	draw_image_with_otherbg(115, dish, 15, 15, numlist[score / 10], 20);
	score %= 10;
	draw_image_with_otherbg(130, dish, 15, 15, numlist[score], 20);
  } else if (score >=1000000) {
	draw_image_with_otherbg(32, dish, 15, 15, numlist[score / 1000000], 20);
	score %= 1000000;
	draw_image_with_otherbg(57, dish, 15, 15, numlist[score / 100000], 20);
	score %= 100000;
	draw_image_with_otherbg(62, dish, 15, 15, numlist[score / 10000], 20);
	score %= 10000;
	draw_image_with_otherbg(77, dish, 15, 15, numlist[score / 1000], 20);
	score %= 1000;
	draw_image_with_otherbg(92, dish, 15, 15, numlist[score / 100], 20);
	score %= 100;
	draw_image_with_otherbg(107, dish, 15, 15, numlist[score / 10], 20);
	score %= 10;
	draw_image_with_otherbg(122, dish, 15, 15, numlist[score], 20);
  } else if (score >=100000) {
	draw_image_with_otherbg(40, dish, 15, 15, numlist[score / 100000], 20);
	score %= 100000;
	draw_image_with_otherbg(55, dish, 15, 15, numlist[score / 10000], 20);
	score %= 10000;
	draw_image_with_otherbg(70, dish, 15, 15, numlist[score / 1000], 20);
	score %= 1000;
	draw_image_with_otherbg(85, dish, 15, 15, numlist[score / 100], 20);
	score %= 100;
	draw_image_with_otherbg(100, dish, 15, 15, numlist[score / 10], 20);
	score %= 10;
	draw_image_with_otherbg(115, dish, 15, 15, numlist[score], 20);
  } else if (score >= 10000) {
	draw_image_with_otherbg(47, dish, 15, 15, numlist[score / 10000], 20);
	score %= 10000;
	draw_image_with_otherbg(62, dish, 15, 15, numlist[score / 1000], 20);
	score %= 1000;
	draw_image_with_otherbg(77, dish, 15, 15, numlist[score / 100], 20);
	score %= 100;
	draw_image_with_otherbg(92, dish, 15, 15, numlist[score / 10], 20);
	score %= 10;
	draw_image_with_otherbg(107, dish, 15, 15, numlist[score], 20);
  } else if (score >=1000) {
	draw_image_with_otherbg(55, dish, 15, 15, numlist[score / 1000], 20);
	score %= 1000;
	draw_image_with_otherbg(70, dish, 15, 15, numlist[score / 100], 20);
	score %= 100;
	draw_image_with_otherbg(85, dish, 15, 15, numlist[score / 10], 20);
	score %= 10;
	draw_image_with_otherbg(100, dish, 15, 15, numlist[score], 20);
  } else if (score >=100) {
	draw_image_with_otherbg(62, dish, 15, 15, numlist[score / 100], 20);
	score %= 100;
	draw_image_with_otherbg(77, dish, 15, 15, numlist[score / 10], 20);
	score %= 10;
	draw_image_with_otherbg(92, dish, 15, 15, numlist[score], 20);
  } else if (score >=10) {
	draw_image_with_otherbg(70, dish, 15, 15, numlist[score / 10], 20);
	score %= 10;
	draw_image_with_otherbg(85, dish, 15, 15, numlist[score], 20);
  } else {
	draw_image_with_otherbg(77, dish, 15, 15, numlist[score], 20);
  }

}

void show(int map[][J], Player player, Ghost* ghost, int level, int score, int life)
{
    drawgg_red(ghost[0].j*15+163+ghost[0].j_offset*15/ghost[0].speed, ghost[0].i*15-7+ghost[0].i_offset*15/ghost[0].speed, (ghost[0].dir_now)%4, status_dir%4, player.mode,1);
    if (level >= 2) drawgg_pink(ghost[1].j*15+163+ghost[1].j_offset*15/ghost[1].speed, ghost[1].i*15-7+ghost[1].i_offset*15/ghost[1].speed, (ghost[1].dir_now)%4, status_dir%4, player.mode,1);
    if (level >= 3) drawgg_blue(ghost[2].j*15+163+ghost[2].j_offset*15/ghost[2].speed, ghost[2].i*15-7+ghost[2].i_offset*15/ghost[2].speed, (ghost[2].dir_now)%4, status_dir%4, player.mode,1);
    if (level >= 4) drawgg_yellow(ghost[3].j*15+163+ghost[3].j_offset*15/ghost[3].speed, ghost[3].i*15-7+ghost[3].i_offset*15/ghost[3].speed, (ghost[3].dir_now)%4, status_dir%4, player.mode,1);

    drawpm(player.j*15+163+player.j_offset*15/player.speed, player.i*15-7+player.i_offset*15/player.speed, (player.dir_now)%4, status_dir%4, life, level);
    drawgg_red(ghost[0].j*15+163+ghost[0].j_offset*15/ghost[0].speed, ghost[0].i*15-7+ghost[0].i_offset*15/ghost[0].speed, (ghost[0].dir_now)%4, status_dir%4, player.mode,0);
    if (level >= 2) drawgg_pink(ghost[1].j*15+163+ghost[1].j_offset*15/ghost[1].speed, ghost[1].i*15-7+ghost[1].i_offset*15/ghost[1].speed, (ghost[1].dir_now)%4, status_dir%4, player.mode,0);
    if (level >= 3) drawgg_blue(ghost[2].j*15+163+ghost[2].j_offset*15/ghost[2].speed, ghost[2].i*15-7+ghost[2].i_offset*15/ghost[2].speed, (ghost[2].dir_now)%4, status_dir%4, player.mode,0);
    if (level >= 4) drawgg_yellow(ghost[3].j*15+163+ghost[3].j_offset*15/ghost[3].speed, ghost[3].i*15-7+ghost[3].i_offset*15/ghost[3].speed, (ghost[3].dir_now)%4, status_dir%4, player.mode,0);
    updatescoredisp(score);
	//printf("pacman!\n");
}

void drawlife(int life) {

//103
int x;
if(life >0)
	draw_image_with_color(10,255,150,30,20);

for(x = 0; x <life; x++) {


draw_image_without_blackbg(10 +x * 30, 255, 30,30, board_life);

}
}

int main(void)
{
	initial_env();

	int xi, xj;
	int dispScore = 0;

	const int data_map[I][J] = {
		{ 3,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,34,35,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,5 },
		{ 10,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,22,20,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,8 },
		{ 10,0,15,19,19,19,17,0,15,19,19,19,19,19,17,0,15,19,17,0,22,20,0,15,19,17,0,15,19,19,19,19,19,17,0,15,19,19,19,17,0,8 },
		{ 10,0,22,0,0,0,20,0,22,0,0,0,0,0,20,0,22,0,20,0,22,20,0,22,0,20,0,22,0,0,0,0,0,20,0,22,0,0,0,20,0,8 },
		{ 10,0,16,21,21,21,18,0,16,21,21,21,21,21,18,0,16,21,18,0,22,20,0,16,21,18,0,16,21,21,21,21,21,18,0,16,21,21,21,18,0,8 },
		{ 10,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,22,20,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,8 },
		{ 10,0,15,19,19,19,19,19,19,17,0,15,17,0,15,19,19,19,17,0,22,20,0,15,19,19,19,17,0,15,17,0,15,19,19,19,19,19,19,17,0,8 },
		{ 10,0,16,21,21,21,21,21,21,18,0,22,20,0,16,21,21,21,18,0,16,18,0,16,21,21,21,18,0,22,20,0,16,21,21,21,21,21,21,18,0,8 },
		{ 10,0,0,0,0,0,0,0,0,0,0,22,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,20,0,0,0,0,0,0,0,0,0,0,8 },
		{ 4,9,9,9,9,9,17,0,15,19,19,18,20,0,15,19,19,19,19,19,19,19,19,19,19,19,19,17,0,22,16,19,19,17,0,15,9,9,9,9,9,6 },
		{ 0,0,0,0,0,0,10,0,16,21,21,17,20,0,16,21,21,21,21,21,21,21,21,21,21,21,21,18,0,22,15,21,21,18,0,8,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,10,0,0,0,0,22,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,20,0,0,0,0,8,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,10,0,15,17,0,22,20,0,15,19,17,0,23,27,27,31,33,33,32,27,27,24,0,22,20,0,15,17,0,8,0,0,0,0,0,0 },
		{ 7,7,7,7,7,7,18,0,22,20,0,16,18,0,22,0,20,0,28,0,0,0,0,0,0,0,0,30,0,16,18,0,22,20,0,16,7,7,7,7,7,7 },
		{ -10,0,0,0,0,0,0,0,22,20,0,0,0,0,22,0,20,0,28,0,0,0,0,0,0,0,0,30,0,0,0,0,22,20,0,0,0,0,0,0,0,-11 },
		{ 9,9,9,9,9,9,17,0,22,16,19,19,17,0,16,21,18,0,25,29,29,29,29,29,29,29,29,26,0,15,19,19,18,20,0,15,9,9,9,9,9,9 },
		{ 0,0,0,0,0,0,10,0,22,15,21,21,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,21,21,17,20,0,8,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,10,0,22,20,0,0,0,0,15,19,17,0,15,19,19,19,19,17,0,15,19,17,0,0,0,0,22,20,0,8,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,10,0,22,20,0,15,17,0,22,0,20,0,22,0,0,0,0,20,0,22,0,20,0,15,17,0,22,20,0,8,0,0,0,0,0,0 },
		{ 3,7,7,7,7,7,18,0,16,18,0,22,20,0,22,0,20,0,22,0,0,0,0,20,0,22,0,20,0,22,20,0,16,18,0,16,7,7,7,7,7,5 },
		{ 10,0,0,0,0,0,0,0,0,0,0,22,20,0,22,0,20,0,22,0,0,0,0,20,0,22,0,20,0,22,20,0,0,0,0,0,0,0,0,0,0,8 },
		{ 10,0,15,19,19,19,19,19,17,0,15,18,20,0,22,0,20,0,22,0,0,0,0,20,0,22,0,20,0,22,16,17,0,15,19,19,19,19,19,17,0,8 },
		{ 10,0,16,21,21,21,21,21,18,0,16,17,20,0,22,0,20,0,22,0,0,0,0,20,0,22,0,20,0,22,15,18,0,16,21,21,21,21,21,18,0,8 },
		{ 10,0,0,0,0,0,0,0,0,0,0,22,20,0,22,0,20,0,22,0,0,0,0,20,0,22,0,20,0,22,20,0,0,0,0,0,0,0,0,0,0,8 },
		{ 14,19,19,19,19,17,0,15,19,17,0,22,20,0,22,0,20,0,22,0,0,0,0,20,0,22,0,20,0,22,20,0,15,19,17,0,15,19,19,19,19,13 },
		{ 12,21,21,21,21,18,0,22,0,20,0,22,20,0,22,0,20,0,22,0,0,0,0,20,0,22,0,20,0,22,20,0,22,0,20,0,16,21,21,21,21,11 },
		{ 10,0,0,0,0,0,0,22,0,20,0,16,18,0,16,21,18,0,16,21,21,21,21,18,0,16,21,18,0,16,18,0,22,0,20,0,0,0,0,0,0,8 },
		{ 10,0,15,19,19,17,0,22,0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,20,0,15,19,19,17,0,8 },
		{ 10,0,22,0,0,20,0,22,0,20,0,15,19,19,19,19,19,19,19,19,17,0,15,19,19,19,19,19,19,19,17,0,22,0,20,0,22,0,0,20,0,8 },
		{ 10,0,16,21,21,18,0,16,21,18,0,16,21,21,21,21,21,21,21,21,18,0,16,21,21,21,21,21,21,21,18,0,16,21,18,0,16,21,21,18,0,8 },
		{ 10,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,8 },
		{ 4,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,6 }
	};


	draw_image_with_color(0,0,170,480,20);
	draw_image_without_blackbg(47,27,15,15,board_S);
	draw_image_without_blackbg(62,27,15,15,board_C);
	draw_image_without_blackbg(77,27,15,15,board_O);
	draw_image_without_blackbg(92,27,15,15,board_R);
	draw_image_without_blackbg(107,27,15,15,board_E);

	draw_image_without_blackbg(47,141,15,15,board_S);
	draw_image_without_blackbg(62,141,15,15,board_T);
	draw_image_without_blackbg(77,141,15,15,board_A);
	draw_image_without_blackbg(92,141,15,15,board_G);
	draw_image_without_blackbg(107,141,15,15,board_E);

	Player player;
	Ghost ghost[4];
	int map[I][J];
	int score = 0, life=3;
	int level = 1, coin_cnt_left = 0;
	int _check;
	status_dir = 0;
	int sub_status_dir = 0;
        int tempdir;
	int ii;
	drawlife(life);
	while(1)
	{
		tempdir = -1;
		direction = -1;
		Init(data_map, map, &player, ghost, level, &coin_cnt_left);
		for(xi = 0;xi < 32; xi++)
		    for(xj = 0;xj < 42; xj++)
		      draw_image(170 + 15*xj, 15 * xi, 15, 15, dmap[dxmap[xi][xj]]);

		draw_image_without_blackbg(440,450,15,15,board_yR);
		draw_image_without_blackbg(455,450,15,15,board_yE);
		draw_image_without_blackbg(470,450,15,15,board_yA);
		draw_image_without_blackbg(485,450,15,15,board_yD);
		draw_image_without_blackbg(500,450,15,15,board_yY);
		draw_image_without_blackbg(515,450,15,15,board_yimp);
		
		draw_image_with_otherbg(77, 159, 15, 15, numlist[level], 20);
		while (1)
		{
			sub_status_dir++;
			if (sub_status_dir % 2 == 0) status_dir++;
			
			
			Input(&player);

			_check = proc(map, &player, ghost, level, &score, &life, &coin_cnt_left);

			if (_check == 1) { printf("GAMEOVER\n"); mdelay(500); life = 3; level = 1; score = 0; break; }
			else if (_check == 2) { level++; break; }

			show(map, player, ghost, level, score, life);

			for (ii = 0; ii < 5; ii++) {
				old_x = send_x;
				old_y = send_y;
				mdelay(10);
				tempdir = direction;
				change_dir();
				if(tempdir == -1 && direction != tempdir) {
					draw_image_with_color(440,450,90,15,0);
					for (xj = 18; xj < 24; xj++) draw_image(170 + 15*xj, 450, 15, 15, dmap[data_map[30][xj]]);
				}
			}
			draw_image_with_otherbg(77, 159, 15, 15, numlist[level], 20);
			//updatescoredisp(dispScore);

		}

		if (level == 5) { printf("GAMECLEAR\n"); return 0; }
	}
}
