#pragma once

typedef enum TYPE
{
	type_mario,
	type_mushroommonster,
	type_tortoise,
	type_chomper,
	type_coin,
	type_mushroom,
	type_flower,
	type_star,
	type_bullet,
	type_box,
	type_brick,
	type_pipe,
	type_flag,
	type_ornament,
	/*type_mountain,
	type_clound,
	type_tree,*/
	type_castle,
	type_background
}TYPE;

typedef enum IDBITMAP
{
	ID_SMALLLEFTMARIO,
	ID_SMALLRIGHTMARIO,
	ID_DEADMARIO,
	ID_LEFTMARIO,
	ID_RIGHTMARIO,
	ID_LEFTFIREMARIO,
	ID_RIGHTFIREMARIO,
	ID_MUSHROOMMONSTER,
	ID_TORTOISE,
	ID_CHOMPER,
	ID_MUSHROOM,
	ID_FLOWERANDSTAR,
	ID_BULLET,
	ID_FLASHCOIN,
	ID_BACKGROUND
}IDBITMAP;

typedef enum STATE
{
	stand,jump,float_,squat,walk,shrink,dead,opened,invalidate,bigger,smaller,hide
}STATE;

typedef struct Game
{
	int point;
	bool newstage;
	WORD *map[2];
	int life;
	int totalgrade;
	bool pause;
	//bool quit;
	HWND hwnd;
	Draw *draw;
	int cx,cy;
	int current;
	int offset;
	int destinationsize;
	POINT start[4];
	int sourcesize;
	int acceleration;
	Chain *chain;
	Sound *sound;
}Game;

typedef struct Object
{
	Game *game;
	ID2D1Bitmap *D2DBitmap;
	TYPE type;
	IDBITMAP id;
	int priority;
	D2D1_RECT_F source;
	D2D1_RECT_F destination;
	STATE state;
	bool facetoleft;
	int speed;
	int duration;
	int falltime;

	void (*Destructor)(struct Object*);
	bool (*GetImage)(struct Object*);
	void (*ReleaseImage)(struct Object*);
	void (*SetDestination)(struct Object*,int,int,int);
	bool (*AlterSourceRectangle)(struct Object*);
	bool (*OutOfRange)(struct Object*,int,int);
	void (*Move)(struct Object*,Chain*,unsigned char[256]);
	void (*Gravity)(struct Object*,Chain*);
	bool (*Impact)(struct Object*,struct Object*,bool);
	void (*Reaction)(struct Object*);
	void* (*UpdateData)(struct Object*,bool);
}Object;

#define MAXPRIORITY 3

typedef enum ChildImage
{
	mountain1_1,mountain1_2,mountain1_3,mountain1_4,
	mountain2_1,mountain2_2,mountain2_3,mountain2_4,mountain2_5,mountain2_6,mountain2_7,mountain2_8,mountain2_9,
	bluecloud1_1,bluecloud1_2,bluecloud1_3,bluecloud1_4,bluecloud1_5,bluecloud1_6,
	bluecloud2_1,bluecloud2_2,bluecloud2_3,bluecloud2_4,bluecloud2_5,bluecloud2_6,bluecloud2_7,bluecloud2_8,
	bluecloud3_1,bluecloud3_2,bluecloud3_3,bluecloud3_4,
	redcloud1_1,redcloud1_2,redcloud1_3,redcloud1_4,redcloud1_5,redcloud1_6,
	redcloud2_1,redcloud2_2,redcloud2_3,redcloud2_4,redcloud2_5,redcloud2_6,redcloud2_7,redcloud2_8,
	redcloud3_1,redcloud3_2,redcloud3_3,redcloud3_4,
	tree1_1,tree1_2,
	tree2_1,tree2_2,tree2_3,tree2_4,
	tree3_1,tree3_2,tree3_3,
	orangepipe1,orangepipe2,orangepipe3,orangepipe4,orangepipe5,orangepipe6,orangepipe7,orangepipe8,orangepipe9,orangepipe10,
	orangepipe11,orangepipe12,orangepipe13,orangepipe14,
	greenpipe1,greenpipe2,greenpipe3,greenpipe4,greenpipe5,greenpipe6,greenpipe7,greenpipe8,greenpipe9,greenpipe10,
	greenpipe11,greenpipe12,greenpipe13,greenpipe14,
	silverpipe1,silverpipe2,silverpipe3,silverpipe4,silverpipe5,silverpipe6,silverpipe7,silverpipe8,silverpipe9,silverpipe10,
	silverpipe11,silverpipe12,silverpipe13,silverpipe14,
	bluepipe1,bluepipe2,bluepipe3,bluepipe4,bluepipe5,bluepipe6,bluepipe7,bluepipe8,bluepipe9,bluepipe10,
	bluepipe11,bluepipe12,bluepipe13,bluepipe14,
	castle1_1,castle1_2,castle1_3,castle1_4,castle1_5,castle1_6,castle1_7,castle1_8,castle1_9,castle1_10,castle1_11,castle1_12,
	castle1_13,castle1_14,castle1_15,castle1_16,castle1_17,castle1_18,castle1_19,castle1_20,castle1_21,castle1_22,castle1_23,
	castle1_24,castle1_25,
	castle2_1,castle2_2,castle2_3,castle2_4,castle2_5,castle2_6,castle2_7,castle2_8,castle2_9,castle2_10,castle2_11,castle2_12,
	castle2_13,castle2_14,castle2_15,castle2_16,castle2_17,castle2_18,castle2_19,castle2_20,castle2_21,castle2_22,castle2_23,
	castle2_24,castle2_25,castleflag,
	flag1_1,flag1_2,flag1_3,flag1_4,
	flag2_1,flag2_2,flag2_3,flag2_4,
	flag3_1,flag3_2,flag3_3,flag3_4,
	flag4_1,flag4_2,flag4_3,flag4_4,
	StoneBrick,CrackedStoneBrick,ChiseledStoneBrick
}ChildImage;

typedef enum Location
{
	LT,RT,LB,RB
}Location;

bool ObjectConstructor(Object *object,Game *game,IDBITMAP id);
void CommonAlter(Object *object,int x,int count);
void BackgroundCommonAlter(Object *object,Location location,int x,int y);
bool MoveIfValidate(Object *object,Chain *chain,int speed,int coefficient,bool updown);