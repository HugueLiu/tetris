#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <linux/input.h>
#include <fcntl.h>
#include <pthread.h>

#define HIDE				"\e[8m"
#define NORMAL				"\033[0m"
#define HEIGHT_CONT 		20 //CAN BE CHANGED
#define WIDTH_CONT 			12 //CAN BE CHANGED

#define LOW					4
#define HIGH				HEIGHT_CONT+LOW + 1
#define LEFT 				1
#define RIGHT				LEFT + WIDTH_CONT + 1

#define KEYBOARD_PATH "/dev/input/event3" //may be different

#define TTY_PATH            "/dev/tty"
#define STTY_US             "stty raw -echo -F "
#define STTY_DEF            "stty -raw echo -F "

bool a[HIGH][RIGHT];
// bool DOWN = 0,LEFT = 0,RIGHT = 0;

typedef struct Block{
	int x;
	int y;
}Block;

typedef struct Group{
	Block *b[4];
}Group;

void init();
void draw();
void generate();
void moveDown(Group *g); // 向下移动
void moveLR(Group *g,int n); //左右移动，p为移动步数，右为正
bool isBottom(Group *g); //下侧是否有障碍
bool isLSide(Group *g); //左侧是否有障碍
bool isRSide(Group *g); //右侧是否有障碍
bool hasBlock(Group *g,int i,int x,int y);
bool isOnBottom(Group *g,int i);
bool isOnLeft(Group *g,int i);
bool isOnRight(Group *g,int i);
void rotate(Group *p);
void listen();
void getScore();


Group *g;
bool stop;
int main(){
	// system(STTY_US TTY_PATH);
	init();
	int i;
	g = malloc(sizeof(Group));
	for(i = 0; i < 4; i++){
		g->b[i] = malloc(sizeof(Block));
	}
	generate(g);
	pthread_t id;
	int ret=pthread_create(&id,NULL,(void*)listen,NULL); // 成功返回0，错误返回错误编号
	if(ret!=0) {
		printf ("Create pthread error!\n");
		return -1;
	}
	stop = false;
	while(1){
		generate(g);
		while(!isBottom(g)){
			if(!stop){
				moveDown(g);
				getScore();
				draw();
				usleep(200000);
			}
		}
	}
	pthread_join(id,NULL);
	return 0;
}

void init(){
	srand((unsigned)time(NULL));
	
	int x,y;
	for(y = LOW; y < HIGH; y++){
		for(x = LEFT; x < RIGHT; x++){
			a[y][x] = 0;
		}
	}
	for(x = 0; x < RIGHT; x++){
		a[HIGH-1][x] = 1;
	}
	for(y = 0; y < HIGH; y++){
		a[y][0] = 1;
		a[y][RIGHT-1] = 1;
	}
}

void draw(){
	system("clear");
	int x,y;
	for(y = LOW; y < HIGH; y++){
		for(x = 0; x < RIGHT; x++){
			if(a[y][x] == 0){
				printf(HIDE "口");
			}else{
				printf(NORMAL "口");
			}
		}
		printf("\n");
	}
	printf(NORMAL);
}

void generate(Group *g){
	int i,dir,which,x,y;
	Block *p = g->b[0];
	p->x = rand() % WIDTH_CONT + LEFT;
	p->y = 3;
	for(i = 1; i < 4; i++){
		while(1){
			which = rand() % i;
			dir = rand() % 4;
			Block *p = g->b[which];
			switch(dir){
				case 0:
					x = p->x + 1;
					y = p->y;
					break;
				case 1:
					x = p->x;
					y = p->y - 1;
					break;
				case 2:
					x = p->x - 1;
					y = p->y;
					break;
				case 3:
					x = p->x;
					y = p->y + 1;
					break;
			}
			if(y < 4 && !hasBlock(g,i,x,y)){
				Block *p = g->b[i];
				p->x = x;
				p->y = y;
				break;
			}
		}
	}
}

void moveDown(Group *g){
	int i;
	for(i = 0; i < 4; i++){
		int x = g->b[i]->x;
		int y = g->b[i]->y;
		a[y][x] = 0;
	}
	for(i = 0; i < 4; i++){
		int x = g->b[i]->x;
		int y = g->b[i]->y + 1;
		a[y][x] = 1;
		g->b[i]->y = y;
	}
}

void moveLR(Group *g,int n){
	if((n > 0 && isRSide(g)) || ((n < 0 && isLSide(g)))){
		return;
	}
	int i;
	for(i = 0; i < 4; i++){
		int x = g->b[i]->x;
		int y = g->b[i]->y;
		a[y][x] = 0;
	}
	for(i = 0; i < 4; i++){
		int x = g->b[i]->x + n;
		int y = g->b[i]->y;
		a[y][x] = 1;
		g->b[i]->x = x;
	}
}

bool isBottom(Group *g){
	int i;
	for(i = 0; i < 4; i++){
		Block *b = g->b[i];
		if(!isOnBottom(g,i)){
			continue;
		}
		if(a[b->y+1][b->x]){
			return true;
		}
	}
	return false;
}

bool isLSide(Group *g){
	int i;
	for(i = 0; i < 4; i++){
		Block *b = g->b[i];
		if(!isOnLeft(g,i)){
			continue;
		}
		if(a[b->y][b->x-1]){
			return true;
		}
	}
	return false;
}

bool isRSide(Group *g){
	int i;
	for(i = 0; i < 4; i++){
		Block *b = g->b[i];
		if(!isOnRight(g,i)){
			continue;
		}
		if(a[b->y][b->x+1]){
			return true;
		}
	}
	return false;
}

bool hasBlock(Group *g,int i,int x,int y){
	for(i--;i >= 0; i--){
		if((g->b[i]->x == x && g->b[i]->y == y)){
			return true;
		}
	}
	return false;
}

bool isOnBottom(Group *g,int i){
	int j;
	Block *bi,*b;
	bi = g->b[i];
	for(j = 0; j < 4; j++){
		if(j == i){
			continue;
		}
		b = g->b[j];
		if(bi->x == b->x && bi->y == b->y - 1){
			return false;
		}

	}
	return true;
}

bool isOnLeft(Group *g,int i){
	int j;
	Block *bi,*b;
	bi = g->b[i];
	for(j = 0; j < 4; j++){
		if(j == i){
			continue;
		}
		b = g->b[j];
		if(bi->y == b->y && bi->x == b->x + 1){
			return false;
		}

	}
	return true;
}

bool isOnRight(Group *g,int i){
	int j;
	Block *bi,*b;
	bi = g->b[i];
	for(j = 0; j < 4; j++){
		if(j == i){
			continue;
		}
		b = g->b[j];
		if(bi->y == b->y && bi->x == b->x - 1){
			return false;
		}

	}
	return true;
}

void rotate(Group *g){
	int b[3][2];
	int cx = g->b[0]->x;
	int cy = g->b[0]->y;
	int i,x,y,nx,ny;
	stop = true;
	for(i = 1; i < 4; i++){
		x = g->b[i]->x;
		y = g->b[i]->y;
		nx = cx + cy - y;
		ny = cy - cx + x;
		if(nx < 1 || nx >= RIGHT-1 || a[ny][nx]){
			printf("can't rotate....cx:%d,cy:%d,",cx,cy);
			printf("nx:%d,ny:%d,x:%d,y:%d,a:%d",nx,ny,x,y,a[ny][nx]);
			return;
		}
		b[i-1][0] = nx;
		b[i-1][1] = ny;
	}
	for(i = 0; i < 3; i++){
		a[g->b[i+1]->y][g->b[i+1]->x] = 0;
		x = b[i][0];
		y = b[i][1];
		a[y][x] = 1;
		g->b[i+1]->x = x;
		g->b[i+1]->y = y;
	}
	stop = false;
}

void listen(){
	int keys_fd;
	char ret[2];
	struct input_event t;
	keys_fd = open(KEYBOARD_PATH, O_RDONLY|O_NONBLOCK);
	if(keys_fd <= 0){
		printf("open /dev/input/event3 device error!\n");
		exit(0);
	}
	while(1){
		// usleep(50000);
		if(read(keys_fd, &t, sizeof(t)) == sizeof(t)){
			if(t.type==EV_KEY && t.value == 0){
				switch(t.code){
				case KEY_ESC:
					// system(STTY_DEF TTY_PATH);
					exit(0);
				case KEY_UP:
					rotate(g);
					draw();
					break;
				case KEY_LEFT:
					// LEFT = t.value;
					moveLR(g,-1);
					draw();
					break;
				case KEY_RIGHT:
					// RIGHT = t.value;
					moveLR(g,1);
					draw();
					break;
				case KEY_DOWN:
					if(!isBottom(g)){
						moveDown(g);
						draw();
						getScore();
					}
					break;
				case KEY_SPACE:
					stop = false;
				}
            }
		}
    }
}

void getScore(){
	int x,y,i,j;
	for(y = LOW; y < HIGH - 1; y++){
		for(x = LEFT; x < RIGHT - 1; x++){
			if(a[y][x] == 0){
				break;
			}
		}
		if(x == RIGHT - 1){
			for(j = y; j >= 0; j--){
				for(i = LEFT; i < RIGHT - 1; i++){
					a[j][i] = a[j-1][i];
				}
			}
			for(i = LEFT; i < RIGHT - 1; i++){
				a[0][i] = 0;
			}
			draw();
		}
	}
}