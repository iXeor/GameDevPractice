#include<iostream>
#include <fstream> //�ļ���ͷ�ļ�
#include <algorithm> //C++�㷨��
#include<conio.h>
using namespace std;
void readFile(char** buffer, int* size, const char* filename);
//��ά������/ģ����
template< class T > class Array2D {
public:
	Array2D() : mArray(0) {}
	~Array2D() {
		delete[] mArray;
		mArray = 0;  //Ϊ��ȫ�������ָ��ֵ��Ϊ0
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

//״̬��
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
const char* name[] = { "stageData01.txt","stageData02.txt","stageData03.txt","stageData04.txt","stageData05.txt" };//��ʱ���������ͼ
int main() {
	const char* filename = name[i%5];//��ֹiֵ���������ѭ���ؿ�
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

	//��ѭ��
	while (true) {
		state->draw();
		//ͨ�ؼ��
		if (state->hasCleared()) {
			break; //ͨ�غ�����ѭ��
		}
		//����ָ��
		cout << endl;
		cout << "Current Map [ "<<(i%5)+1<<" ]"<< endl<<endl;//��ʾ��ǰ�ؿ���
		cout << "----------------------------" << endl;
		cout << "| W:�� | A:�� | S:�� | D:�� |"<<endl<<"| N:Next Map |P:Previous Map|\n----------------------------\n|   R:Reset   |    X;Exit   |\n----------------------------" << endl; //����˵��
		char input;
		input = _getch(); //�ⰴ�س�
		//ˢ��
		state->update(input);
	}
	//��ӡͨ��ף�ص���Ϣ
	system("CLS");//�ճ�����
	cout << "Congratulations!!!" << endl << "Continue? \n1.Yep\n2.No" << endl;
	delete[] stageData;
	stageData = 0;
	//���ո�λ
	while (1) {
		char x;
		x = _getch();
		if (x == '1') { i++; main(); }//������Ϸ����ȡ��һ�ؿ���ͼ
		else if (x == '2') exit(0);//���λ�õ���ֹ��任��exit()Ӧ�ø�����Щ
		else cout << "��������ȷ��ָ��" << endl;
	}
	
	return 0;
}

//ʵ�ֶ�ȡ��ͼ�ļ��ķ���
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
	//ȷ������
	setSize(stageData, size);
	//ȷ���ռ�
	mObjects.setSize(mWidth, mHeight);
	mGoalFlags.setSize(mWidth, mHeight);
	//Ԥ���ʼֵ
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			mObjects(x, y) = OBJ_WALL; //���ಿ�ֶ�����Ϊǽ��
			mGoalFlags(x, y) = false; //���յ�
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
		case '\n': x = 0; ++y; t = OBJ_UNKNOWN; break; //���д���
		default: t = OBJ_UNKNOWN; break;
		}
		if (t != OBJ_UNKNOWN) { //���if������������������δ�����Ԫ��ֵ��������
			mObjects(x, y) = t; //д��
			mGoalFlags(x, y) = goalFlag; //�յ���Ϣ
			++x;
		}
	}
}

void State::setSize(const char* stageData, int size) {
	mWidth = mHeight = 0; //��ʼ��
	//��ǰλ��
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
			//�������ֵ
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
	//�ƶ���
	int dx = 0;
	int dy = 0;
	switch (input) {
	case 'a': dx = -1; break; //����
	case 'A': dx = -1; break; //����
	case 'd': dx = 1; break; //��
	case 'D': dx = 1; break; //��
	case 'w': dy = -1; break; //�ϡ�Y����Ϊ��
	case 'W': dy = -1; break; //�ϡ�
	case 's': dy = 1; break; //�¡�
	case 'S': dy = 1; break; //�¡�
	case 'R':main();//���õ�ǰ�ؿ���
	case 'r':main();//���õ�ǰ�ؿ���
	case 'N': {i += 1; main(); }//������һ�ء�
	case 'n': {i += 1; main(); }//������һ�ء�
	case 'P': {if (i > 1) i -= 1; else i = 5; main(); }//������һ�ء�
	case 'p': {if (i > 1) i -= 1; else i = 5; main(); }//������һ�ء�
	case 'X': exit(0);//�˳�
	case 'x': exit(0);//�˳�
	}
	//ʹ�ö̱�����
	int w = mWidth;
	int h = mHeight;
	Array2D< Object >& o = mObjects;
	//����С�˵�����
	int x, y;
	x = y = -1; //Σ��ֵ
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
	//�ƶ��������
	int tx = x + dx;
	int ty = y + dy;
	//�ж�����ļ���ֵ��������������ֵ��Χ
	if (tx < 0 || ty < 0 || tx >= w || ty >= h) {
		return;
	}
	//A.�÷������ǿհ׻����յ㡣��С���ƶ�
	if (o(tx, ty) == OBJ_SPACE) {
		o(tx, ty) = OBJ_MAN;
		o(x, y) = OBJ_SPACE;
		//B.����÷����������ӡ����Ҹ÷�������¸������ǿհ׻����յ㣬�������ƶ�
	}
	else if (o(tx, ty) == OBJ_BLOCK) {
		//���ͬ�����ϵ����¸������Ƿ�λ�ں���ֵ��Χ
		int tx2 = tx + dx;
		int ty2 = ty + dy;
		if (tx2 < 0 || ty2 < 0 || tx2 >= w || ty2 >= h) { //������Ч
			return;
		}
		if (o(tx2, ty2) == OBJ_SPACE) {
			//��˳���滻
			o(tx2, ty2) = OBJ_BLOCK;
			o(tx, ty) = OBJ_MAN;
			o(x, y) = OBJ_SPACE;
		}
	}
}

//ֻҪ������һ��goalFlagֵ������false�������ж�Ϊͨ��
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