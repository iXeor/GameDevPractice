#include<iostream>
#include<conio.h>
using namespace std;
const char gStageData[] = "\
############\n\
#   ..  p  #\n\
#  o   o   #\n\
#          #\n\
#          #\n\
#          #\n\
#          #\n\
############\n";
const int gStageWidth = 12;
const int gStageHeight = 8;
enum Obj {
	OBJ_SPACE,
	OBJ_WALL,
	OBJ_GOAL,
	OBJ_BLOCK,
	OBJ_BLOCK_ON_GOAL,
	OBJ_MAN,
	OBJ_MAN_ON_GOAL,
	OBJ_UNKNOWN
};
void init(Obj* state, int width, int height, const char* stageData) {
	const char* stData = stageData;
	int pointX = 0;
	int pointY = 0;
	while (*stData != '\0') {
		Obj t;
		switch (*stData) {
		case '#':t = OBJ_WALL; break;
		case ' ':t = OBJ_SPACE; break;
		case '.':t = OBJ_GOAL; break;
		case 'o':t = OBJ_BLOCK; break;
		case 'O':t = OBJ_BLOCK_ON_GOAL; break;
		case 'p':t = OBJ_MAN; break;
		case 'P':t = OBJ_MAN_ON_GOAL; break;
		case '\n':pointX = 0; ++pointY; t = OBJ_UNKNOWN; break;
		default: t = OBJ_UNKNOWN;
		}
		++stData;
		if (t != OBJ_UNKNOWN) {
			state[pointY * width + pointX] = t;
			++pointX;
		}
	}
}
void print(const Obj* state, int width, int height) {
	system("CLS");
	const char block[] = { ' ','#','.','o','O','p','P' };
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			Obj o = state[j * width + i];
			cout << block[o];
		}
		cout << endl;
	}
}
void update(Obj* s, char input, int w, int h) {
	int dx = 0;
	int dy = 0;
	switch (input) {
	case 'a': dx = -1; break;
	case 'd': dx = 1; break;
	case 'w': dy = -1; break;
	case 's': dy = 1; break;
	case 'A': dx = -1; break;
	case 'D': dx = 1; break;
	case 'W': dy = -1; break;
	case 'S': dy = 1; break;
	}
	int i = -1;
	for (i = 0; i < w * h; ++i) {
		if (s[i] == OBJ_MAN || s[i] == OBJ_MAN_ON_GOAL) {
			break;
		}
	}
	int x = i % w;
	int y = i / w;
	int tx = x + dx;
	int ty = y + dy;
	if (tx < 0 || ty < 0 || tx >= w || ty >= h) {
		return;
	}
	int p = y * w + x;
	int tp = ty * w + tx;
	if (s[tp] == OBJ_SPACE || s[tp] == OBJ_GOAL) {
		s[tp] = (s[tp] == OBJ_GOAL) ? OBJ_MAN_ON_GOAL : OBJ_MAN;
		s[p] = (s[p] == OBJ_MAN_ON_GOAL) ? OBJ_GOAL : OBJ_SPACE;
	}
	else if (s[tp] == OBJ_BLOCK || s[tp] == OBJ_BLOCK_ON_GOAL) {
		int tx2 = tx + dx;
		int ty2 = ty + dy;
		if (tx2 < 0 || ty2 < 0 || tx2 >= w || ty2 >= h) { 
			return;
		}

		int tp2 = (ty + dy) * w + (tx + dx);
		if (s[tp2] == OBJ_SPACE || s[tp2] == OBJ_GOAL) {
			s[tp2] = (s[tp2] == OBJ_GOAL) ? OBJ_BLOCK_ON_GOAL : OBJ_BLOCK;
			s[tp] = (s[tp] == OBJ_BLOCK_ON_GOAL) ? OBJ_MAN_ON_GOAL : OBJ_MAN;
			s[p] = (s[p] == OBJ_MAN_ON_GOAL) ? OBJ_GOAL : OBJ_SPACE;
		}
	}
}
bool checkClear(const Obj* state, int width, int height) {
	for (int i = 0; i < width * height; i++) {
		if (state[i] == OBJ_BLOCK) { return false; }
	}
	return true;
}
int main() {
	Obj* state = new Obj[gStageWidth * gStageHeight];
	init(state, gStageWidth, gStageHeight, gStageData);
	while (1) {
		char input;
		print(state, gStageWidth, gStageHeight);
		if (checkClear(state, gStageWidth, gStageHeight)) { break; }
		cout << "W:↑,A:←,S:↓,D:→,Enter:确认" << endl;
		input=_getch();
		update(state, input, gStageWidth, gStageHeight);
	}
	cout << "恭喜！！！" << endl<<"是否继续？ \n1.继续\n2.结束"<<endl;
	delete[] state;
	while (1) {
		char x;
		x=_getch();
		if (x == '1') main();
		else if (x == '2') break;
		else cout << "请输入正确的指令" << endl;
	}
}