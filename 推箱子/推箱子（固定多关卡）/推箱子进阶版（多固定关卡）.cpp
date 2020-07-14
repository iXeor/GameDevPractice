#include<iostream>
#include <fstream> //文件流头文件
#include <algorithm> //C++算法库
#include<conio.h>
using namespace std;
void readFile(char** buffer, int* size, const char* filename);
//二维数组类/模板类
template< class T > class Array2D {
public:
	Array2D() : mArray(0) {}
	~Array2D() {
		delete[] mArray;
		mArray = 0;  //为安全起见，把指针值设为0
	}
	void setSize(int size0, int size1) {
		mSize0 = size0;
		mSize1 = size1;
		mArray = new T[size0 * size1];
	}
	T& operator()(int index0, int index1) {
		return mArray[index1 * mSize0 + index0];
	}
	const T& operator()(int index0, int index1) const {
		return mArray[index1 * mSize0 + index0];
	}
private:
	T* mArray;
	int mSize0;
	int mSize1;
};

//状态类
class State {
public:
	State(const char* stageData, int size);
	void update(char input);
	void draw() const;
	bool hasCleared() const;
private:
	enum Object {
		OBJ_SPACE,
		OBJ_WALL,
		OBJ_BLOCK,
		OBJ_MAN,
		OBJ_UNKNOWN,
	};
	void setSize(const char* stageData, int size);

	int mWidth;
	int mHeight;
	Array2D< Object > mObjects;
	Array2D< bool > mGoalFlags;
};
int i=0;
int argc;
char** argv;
const char* name[] = { "stageData01.txt","stageData02.txt","stageData03.txt","stageData04.txt","stageData05.txt" };//暂时设置五个地图
int main() {
	const char* filename = name[i%5];//防止i值溢出，做成循环关卡
	if (argc >= 2) {
		filename = argv[1];
	}
	char* stageData;
	int fileSize;
	readFile(&stageData, &fileSize, filename);
	if (!stageData) {
		cout << "Stage Data Not Found (>_<;;;)" << endl;
		return 1;
	}
	State* state = new State(stageData, fileSize);

	//主循环
	while (true) {
		state->draw();
		//通关检测
		if (state->hasCleared()) {
			break; //通关后跳出循环
		}
		//操作指南
		cout << endl;
		cout << "Current Map [ "<<(i%5)+1<<" ]"<< endl<<endl;//显示当前关卡号
		cout << "----------------------------" << endl;
		cout << "| W:↑ | A:← | S:↓ | D:→ |"<<endl<<"| N:Next Map |P:Previous Map|\n----------------------------\n|   R:Reset   |    X;Exit   |\n----------------------------" << endl; //操作说明
		char input;
		input = _getch(); //免按回车
		//刷新
		state->update(input);
	}
	//打印通关祝贺的信息
	system("CLS");//日常清屏
	cout << "Congratulations!!!" << endl << "Continue? \n1.Yep\n2.No" << endl;
	delete[] stageData;
	stageData = 0;
	//回收复位
	while (1) {
		char x;
		x = _getch();
		if (x == '1') { i++; main(); }//继续游戏，读取下一关卡地图
		else if (x == '2') exit(0);//这个位置的终止语句换成exit()应该更合理些
		else cout << "请输入正确的指令" << endl;
	}
	
	return 0;
}

//实现读取地图文件的方法
void readFile(char** buffer, int* size, const char* filename) {
	ifstream in(filename);
	if (!in) {
		*buffer = 0;
		*size = 0;
	}
	else {
		in.seekg(0, ifstream::end);
		*size = static_cast<int>(in.tellg());
		in.seekg(0, ifstream::beg);
		*buffer = new char[*size];
		in.read(*buffer, *size);
	}
}

State::State(const char* stageData, int size) {
	//确保容量
	setSize(stageData, size);
	//确保空间
	mObjects.setSize(mWidth, mHeight);
	mGoalFlags.setSize(mWidth, mHeight);
	//预设初始值
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			mObjects(x, y) = OBJ_WALL; //多余部分都设置为墙壁
			mGoalFlags(x, y) = false; //非终点
		}
	}
	int x = 0;
	int y = 0;
	for (int i = 0; i < size; ++i) {
		Object t;
		bool goalFlag = false;
		switch (stageData[i]) {
		case '#': t = OBJ_WALL; break;
		case ' ': t = OBJ_SPACE; break;
		case 'o': t = OBJ_BLOCK; break;
		case 'O': t = OBJ_BLOCK; goalFlag = true; break;
		case '.': t = OBJ_SPACE; goalFlag = true; break;
		case 'p': t = OBJ_MAN; break;
		case 'P': t = OBJ_MAN; goalFlag = true; break;
		case '\n': x = 0; ++y; t = OBJ_UNKNOWN; break; //换行处理
		default: t = OBJ_UNKNOWN; break;
		}
		if (t != OBJ_UNKNOWN) { //这个if处理的意义在如果遇到未定义的元素值就跳过它
			mObjects(x, y) = t; //写入
			mGoalFlags(x, y) = goalFlag; //终点信息
			++x;
		}
	}
}

void State::setSize(const char* stageData, int size) {
	mWidth = mHeight = 0; //初始化
	//当前位置
	int x = 0;
	int y = 0;
	for (int i = 0; i < size; ++i) {
		switch (stageData[i]) {
		case '#': case ' ': case 'o': case 'O':
		case '.': case 'p': case 'P':
			++x;
			break;
		case '\n':
			++y;
			//更新最大值
			mWidth = max(mWidth, x);
			mHeight = max(mHeight, y);
			x = 0;
			break;
		}
	}
}

void State::draw() const {
	system("CLS");
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			Object o = mObjects(x, y);
			bool goalFlag = mGoalFlags(x, y);
			if (goalFlag) {
				switch (o) {
				case OBJ_SPACE: cout << '.'; break;
				case OBJ_WALL: cout << '#'; break;
				case OBJ_BLOCK: cout << 'O'; break;
				case OBJ_MAN: cout << 'P'; break;
				}
			}
			else {
				switch (o) {
				case OBJ_SPACE: cout << ' '; break;
				case OBJ_WALL: cout << '#'; break;
				case OBJ_BLOCK: cout << 'o'; break;
				case OBJ_MAN: cout << 'p'; break;
				}
			}
		}
		cout << endl;
	}
}

void State::update(char input) {
	//移动量
	int dx = 0;
	int dy = 0;
	switch (input) {
	case 'a': dx = -1; break; //向左
	case 'A': dx = -1; break; //向左
	case 'd': dx = 1; break; //右
	case 'D': dx = 1; break; //右
	case 'w': dy = -1; break; //上。Y朝下为正
	case 'W': dy = -1; break; //上。
	case 's': dy = 1; break; //下。
	case 'S': dy = 1; break; //下。
	case 'R':main();//重置当前关卡。
	case 'r':main();//重置当前关卡。
	case 'N': {i += 1; main(); }//跳至下一关。
	case 'n': {i += 1; main(); }//跳至下一关。
	case 'P': {if (i > 1) i -= 1; else i = 5; main(); }//跳至上一关。
	case 'p': {if (i > 1) i -= 1; else i = 5; main(); }//跳至上一关。
	case 'X': exit(0);//退出
	case 'x': exit(0);//退出
	}
	//使用短变量名
	int w = mWidth;
	int h = mHeight;
	Array2D< Object >& o = mObjects;
	//查找小人的坐标
	int x, y;
	x = y = -1; //危险值
	bool found = false;
	for (y = 0; y < mHeight; ++y) {
		for (x = 0; x < mWidth; ++x) {
			if (o(x, y) == OBJ_MAN) {
				found = true;
				break;
			}
		}
		if (found) {
			break;
		}
	}
	//移动后的坐标
	int tx = x + dx;
	int ty = y + dy;
	//判断坐标的极端值。不允许超出合理值范围
	if (tx < 0 || ty < 0 || tx >= w || ty >= h) {
		return;
	}
	//A.该方向上是空白或者终点。则小人移动
	if (o(tx, ty) == OBJ_SPACE) {
		o(tx, ty) = OBJ_MAN;
		o(x, y) = OBJ_SPACE;
		//B.如果该方向上是箱子。并且该方向的下下个格子是空白或者终点，则允许移动
	}
	else if (o(tx, ty) == OBJ_BLOCK) {
		//检测同方向上的下下个格子是否位于合理值范围
		int tx2 = tx + dx;
		int ty2 = ty + dy;
		if (tx2 < 0 || ty2 < 0 || tx2 >= w || ty2 >= h) { //按键无效
			return;
		}
		if (o(tx2, ty2) == OBJ_SPACE) {
			//按顺序替换
			o(tx2, ty2) = OBJ_BLOCK;
			o(tx, ty) = OBJ_MAN;
			o(x, y) = OBJ_SPACE;
		}
	}
}

//只要还存在一个goalFlag值，就是false，不能判定为通关
bool State::hasCleared() const {
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			if (mObjects(x, y) == OBJ_BLOCK) {
				if (mGoalFlags(x, y) == false) {
					return false;
				}
			}
		}
	}
	return true;
}