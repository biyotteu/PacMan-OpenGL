#include <iostream>
#include ".\include\GL\freeglut.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <time.h>

using namespace std;
using dd = pair<double, double>;

void Init(); // 초기 값 설정
void Render(); // 화면 출력
void Process(int value); // 일정 프레임 마다 동작 처리
void DoKeyboard(int key, int x, int y); // 키보드 인풋 처리
double GetDist(dd a, dd b); // 두점 사이의 거리
void AddFruit(); // 과일 추가 
bool IsCrash(); // 충돌 확인
void MoveEenemy(); // 적 움직이기

clock_t start,addTime; // 시작 시간, 과일 추가 시간

dd player; // 플레이어 위치
vector<dd> fruit, enemy; // 과일, 적 위치
vector<double> enemySpeed; // 적의 속도

int Width = 800, Height = 800; // 창의 가로 세로 크기
int mouthAngle, playerDirection,score; // 플레이어 입 각도, 플레이어 방향, 총 먹은 과일 수

const double enemySize = 20.0, fruitSize = 15.0; // 적의 크기, 과일의 크기
double playerSpeed = 10, playerSize = 1.0; // 플레이어 속도, 플레이어 크기 배율 (과일 먹을 수록 배율이 커짐)
double playerAngles[4] = { 180,90,0,270 }, playerEye[8] = {-7,13,13,7,7,13,-13,-7}; // 플레이어 방향 마다 입의 각도와 눈의 위치 
double nx[4] = { -1,0,1,0 }, ny[4] = { 0,1,0,-1 }; // 키보드 인풋 -> 플레이어 방향


int main(int argc, char** argv) {

	Init(); // 초기화
	glutInit(&argc, argv); // gl 초기화
	glutInitWindowSize(Width, Height); // window 사이즈 설정
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // 더블 버퍼와 rgb 사용
	glutCreateWindow("PACMAN"); // window 생성

	glutDisplayFunc(Render); // 렌더 함수 콜백 등록
	glutSpecialFunc(DoKeyboard); // 키보드 입력 함수 등록
	glutTimerFunc(10, Process, 1); // 10ms 마다 Process 실행

	glutMainLoop();
	return 0;
}

// 거리 계산
double GetDist(dd a, dd b) { 
	// 피타고라스 정리
	return sqrt((a.first - b.first) * (a.first - b.first) + (a.second - b.second) * (a.second - b.second)); 
}

// 과일 생성
void AddFruit() { 
	while (1) {
		// 과일 위치 랜덤 추출
		dd tmp = {rand()%Width,rand()%Height}; 

		// 위치가 window 안인지 확인
		if (tmp.first + fruitSize > Width || tmp.first - fruitSize < 0 || tmp.second + fruitSize > Height || tmp.second - fruitSize < 0) continue; 
		
		// 플레이어와 충돌하는지 확인																																		   
		if (playerSize * 25.0 + fruitSize + 10.0 > GetDist(tmp, player)) continue; 
		
		// 이미 생성된 과일들과 충돌하는지 확인
		bool cmp = false;
		for (int i = 0; i < fruit.size(); i++) {
			if (2 * fruitSize > GetDist(fruit[i], tmp)) {
				cmp = true;
				break;
			}
		}

		// 이미 생성된 적들과 충돌하는지 확인
		for (int i = 0; i < enemy.size(); i++) {
			if (enemySize + fruitSize > GetDist(enemy[i], tmp)) {
				cmp = true;
				break;
			}
		}
		if (cmp) continue;

		// 과일 추가
		fruit.push_back(tmp);
		break;
	}
}

// 적 생성
void AddEnemy() {
	while (1) {
		// 적 위치 랜덤 추출
		dd tmp = { rand() % Width,rand() % Height };

		// window 안인지 확인
		if (tmp.first + enemySize > Width || tmp.first - enemySize < 0 || tmp.second + enemySize > Height || tmp.second - enemySize < 0) continue;
		
		// 플레이어와 충돌 확인
		if (playerSize * 25.0 + enemySize + 200 > GetDist(tmp, player)) continue;
		
		// 이미 생성된 적들과 충돌 확인 (적부터 생성하기 때문에 과일과 충돌 확인 필요X)
		bool cmp = false;
		for (int i = 0; i < enemy.size(); i++) {
			if (2 * enemySize > GetDist(enemy[i], tmp)) {
				cmp = true;
				break;
			}
		}
		if (cmp) continue;

		// 적 추가
		enemy.push_back(tmp);
		// 적 속도 추가
		enemySpeed.push_back((double)(0.5 + (rand()%101)/200.0));
		break;
	}
}

// 초기화
void Init() { 
	// 시작 시간 설정
	start = addTime = clock(); 

	srand((unsigned int)time(NULL));

	// 초기 플레이어 위치
	player.first = Width/2;
	player.second = Height/2;

	// 적, 과일 생성
	for (int i = 0; i < 3; i++) AddEnemy();
	for (int i = 0; i < 8; i++) AddFruit();
}

// 충돌 확인
bool IsCrash() { 
	// 과일 충돌 확인
	for (int i = 0; i < fruit.size();i++) {
		// 플레이어와 충돌 확인
		if (playerSize * 25.0 + fruitSize > GetDist(fruit[i], player)) {
			// 과일 삭제
			fruit.erase(fruit.begin() + i);
			// 플레이어 크기 증가
			playerSize += 0.1;
			// 먹은 과일 수 증가
			score++;
			break;
		}

		// 적과 충돌 확인
		bool cmp = false;
		for (int j = 0; j < enemy.size(); j++) {
			if (enemySize + fruitSize > GetDist(fruit[i], enemy[j])) {
				// 과일 삭제
				fruit.erase(fruit.begin() + i);
				// 적의 속도 증가
				enemySpeed[j] += 0.2;
				cmp = true;
				break;
			}
		}
		if (cmp) break;
		
	}

	// 적 충돌 확인
	for (int i = 0; i < enemy.size(); i++) {
		// 플레이어와 충돌 확인
		if (playerSize * 25.0 + enemySize > GetDist(enemy[i], player)) {
			// 플레이 타임 계산, 결과출력, 종료
			clock_t end = clock();
			double playTime = (double)((end - start) / (double)CLOCKS_PER_SEC);
			if (playTime >= 30.0) cout << "You Win!!\n";
			else cout << "You Lose...\n";
			cout << "Collected Fruit: " << score << "\n";
			if (playTime < 30.0) cout << "Time Later: "<< (double)playTime <<"\n";
			exit(1);
		}
	}
}

// 일정 시간 마다 반복
void Process(int value) {
	// 3초 마다 과일 생성
	clock_t now = clock();
	if ((double)((now - addTime) / (double)CLOCKS_PER_SEC) >= 3) {
		addTime = now;
		AddFruit();
	}
	// 충돌 확인
	IsCrash();
	// 화면 다시 출력
	glutPostRedisplay();
	// 플레이어 입 각도 변경
	mouthAngle += value;
	// 입이 닫혔다 열렸다 설정
	if(mouthAngle <= 0 || mouthAngle >= 30) glutTimerFunc(10, Process, -value);
	else glutTimerFunc(10, Process, value);
	// 적 이동
	MoveEenemy();
}

// 플레이어 출력
void DisplayPlayer() {
	
	double rad = 25.0 * playerSize, x, y, angle;

	// 플레이어 몸통(노란색)
	glColor3f(1.0f, 1.0f, 0.0f);

	// 폴리곤 타입
	glBegin(GL_POLYGON);
	glVertex2f(player.first, player.second);

	// 부채꼴 모양 그리기
	for (int i = mouthAngle; i < 360 - mouthAngle; i++)
	{
		angle = (i + playerAngles[playerDirection]) * 3.141592 / 180;
		x = rad * cos(angle) + player.first;
		y = rad * sin(angle) + player.second;
		glVertex2f(x, y);
	}
	glEnd();

	// 플레이어 눈 (검은색)
	glColor3f(0.0f, 0.0f, 0.0f);

	// 눈 크기
	rad = 3 * playerSize;

	// 폴리곤 타입
	glBegin(GL_POLYGON);

	// 플레이어 방향을 고려해서 원 그리기
	for (int i = 0; i < 360; i++)
	{
		angle = i * 3.141592 / 180;
		x = rad * cos(angle) + player.first + playerEye[playerDirection * 2] * playerSize;
		y = rad * sin(angle) + player.second + playerEye[playerDirection * 2 + 1] * playerSize;
		glVertex2f(x, y);
	}
	glEnd();
}

// 과일 출력
void DisplayFruit() {
	double rad = fruitSize, x, y, angle;
	for (dd fr : fruit) {
		// 과일 몸통 (빨간색)
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);

		// 원 그리기
		for (int i = 0; i < 360; i++)
		{
			angle = i * 3.141592 / 180;
			x = rad * cos(angle) + fr.first;
			y = rad * sin(angle) + fr.second;
			glVertex2f(x, y);
		}
		glEnd();

		// 과일 꼭지 (초록색)
		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_POLYGON);
		x = fr.first;
		y = fr.second + fruitSize;
		glVertex2f(x + 2, y - 6);
		glVertex2f(x - 2, y - 6);
		glVertex2f(x - 2, y + 6);
		glVertex2f(x + 2, y + 6);
		glEnd();
	}
}

// 적 출력
void DislpayEnemy() {
	double rad = enemySize, x, y, angle;
	for (dd en : enemy) {
		// 적 몸통
		rad = enemySize;
		glColor3f(0.2f, 0.2f, 0.3f);
		glBegin(GL_POLYGON);
		glVertex2f(en.first, en.second);

		// 원 그리기
		for (int i = 0; i <= 360; i++)
		{
			angle = i * 3.141592 / 180;
			x = rad * cos(angle) + en.first;
			y = rad * sin(angle) + en.second;
			glVertex2f(x, y);
		}
		glEnd();

		// 적 눈
		x = en.first;
		y = en.second;

		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_POLYGON);
		glVertex2f(x + 4.0, y + 3.0);
		glVertex2f(x + 13.0, y + 8.0);
		glVertex2f(x + 11.0, y + 10.0);
		glVertex2f(x + 2.0, y + 5.0);
		glEnd();

		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_POLYGON);
		glVertex2f(x + 4.0, y + 10.0);
		glVertex2f(x + 13.0, y + 5.0);
		glVertex2f(x + 11.0, y + 3.0);
		glVertex2f(x + 2.0, y + 8.0);
		glEnd();

		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_POLYGON);
		glVertex2f(x - 4.0, y + 3.0);
		glVertex2f(x - 13.0, y + 8.0);
		glVertex2f(x - 11.0, y + 10.0);
		glVertex2f(x - 2.0, y + 5.0);
		glEnd();

		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_POLYGON);
		glVertex2f(x - 4.0, y + 10.0);
		glVertex2f(x - 13.0, y + 5.0);
		glVertex2f(x - 11.0, y + 3.0);
		glVertex2f(x - 2.0, y + 8.0);
		glEnd();

		// 적 입
		glColor3f(0.8f, 0.4f, 0.4f);
		glBegin(GL_POLYGON);
		rad = enemySize / 4;
		for (int i = 0; i <= 360; i++)
		{
			angle = i * 3.141592 / 180;
			x = rad * cos(angle) + en.first;
			y = rad * sin(angle) + en.second - 6.0;
			glVertex2f(x, y);
		}
		glEnd();
	}
}

// 적 이동
void MoveEenemy() {
	for (int i = 0; i < enemy.size(); i++) {
		// 플레이어 쪽으로 방향 설정
		double dirx = (player.first > enemy[i].first ? 1.0 : -1.0);
		double diry = (player.second > enemy[i].second ? 1.0 : -1.0);
		// 다음 위치 설정
		double x = enemy[i].first + dirx * enemySpeed[i];
		double y = enemy[i].second + diry * enemySpeed[i];
		dd tmp = { x,y };

		// 다음 위치가 다른 적들과 충돌하는지 확인
		bool cmp = true;
		for (int j = 0; j < enemy.size(); j++) {
			if (i == j) continue;
			if (2 * enemySize > GetDist(enemy[j], tmp)) {
				cmp = false;
				break;
			}
		}
		if (cmp) {
			enemy[i].first = x;
			enemy[i].second = y;
		}
		else { // 충돌하면 충돌하지 않는 방향으로 이동
			enemy[i].first += -dirx * enemySpeed[i];
			enemy[i].second += -diry * enemySpeed[i];
		}
		
	}
}

// 출력
void Render()
{
	// 흰색 배경
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// 좌표계 설정
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (double)Width, 0.0, (double)Height);

	// 플레이어, 적, 과일 출력
	DisplayPlayer();
	DislpayEnemy();
	DisplayFruit();

	// 더블 버퍼
	glutSwapBuffers();
}

// 키보드 입력
void DoKeyboard(int key, int x, int y)
{
	// 100 ~ 103 까지가 방향키
	playerDirection = key - 100;

	// 플레이어 방향 지정
	double nextX = player.first + nx[playerDirection] * playerSpeed;
	double nextY = player.second + ny[playerDirection] * playerSpeed;

	// 플레이어가 window를 못 벗어나게 지정
	if (!(nextX > Width - playerSize * 25.0 || nextX < playerSize * 25.0 || nextY > Height - playerSize * 25.0 || nextY < playerSize * 25.0)) {
		player.first = nextX;
		player.second = nextY;
	}

	// 화면 다시 출력
	glutPostRedisplay();
}

