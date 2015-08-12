#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream> // 텍스트 파일 출력
#include <fstream> // 텍스트 파일 읽기
#include <conio.h> // 방향키 읽기
#include <math.h>
#include <io.h> // 파일 디렉토리 찾기

#include "dijkstraHeader.h"

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>

#define MAX_PICTURE 100
#define MAX_DIRECTION 4
#define MAX_TEXT 256
#define MAX_DIST 10000000.0
#define arraySize 10000


using namespace std;
using namespace cv;

//입력받은 점의 인덱스와 좌표값
typedef struct pts_info{
	int id; // 점 id
	int direc[4]; // 북서남동
	int updown[2]; // 위 아래
	double updown_length[2]; // 위 아래 길이
	double length[4]; // 점 사이의 길이
	double x, y; // x, y 좌표
	int capturedCompass; // 자이로센서의 x 축
	int floor; // 층 정보
	char tagging[10]; // 장소 정보 태깅

}PTS_INFO;

// 마우스 이벤트 처리
typedef struct MouseChange{
	int direction = -1;
	int change = 0;
	Size frameSize; // 버튼 영역 설정하려고 받음
	int user_dir; // 유저가 바라보는 방향
}MOUSECHANGE;

// 위치 관계표 만들 때 사용
double F_dist(double x1, double y1, double x2, double y2); // 두점 거리
double F_ang(double x1, double y1, double x2, double y2); // 두점 각도
void makeDirectionMap(PTS_INFO* io_dstMap, int &out_num_pic, char *directionMapFileName, pGraph g); // 관계표 만들기
void makeTxtFile(PTS_INFO* io_dstMap, char * in_directionMapFileName, int in_pts_num, pGraph g); // 텍스트 파일로 만들기 
void modifyDirectionMap(PTS_INFO* io_dstMap, char * in_directionMapFileName, int in_pts_num, pGraph g); // 관계표 수정
int isDirectionMapExist(char* file); // 파일 존재 여부 확인
void readTextDirectionMap(char * in_directionMapFileName, PTS_INFO*	&io_dstMap, pGraph g); // 관계표 txt 읽어오기

// 스트리트 뷰 화면 구현에 사용
void showRelation(int(*map)[MAX_DIRECTION]); // 콘솔창에 사진간의 관계 보여주기
void showDirMap(PTS_INFO *dirMap, int in_num_pic); // 콘솔창에 사진간의 관계 보여주기
String getFileName(PTS_INFO map, int compass); // 숫자 넣으면 .jpg로 만들어주기
void readText(String fileName, PTS_INFO* &io_dstMap); // 텍스트 파일에서 사진 관계 불러오기
void slideImage(Mat & io_streetImage, PTS_INFO map, MOUSECHANGE & io_mouse, int & io_user_compass, int in_indexNum); //좌우 화면 전환


// 스트리트 뷰 화면 컨트롤
//void drawArrow(Mat img, int map[MAX_DIRECTION]); // 화살표 그리기
void drawArrowAndGps(Mat img, PTS_INFO mapint, int & in_user_compass);
void callBackFunc(int event, int x, int y, int flags, void* userdata); // 마우스 클릭 이벤트

// 스트리트 뷰 화면 자동 회전
void hough_transform(Mat& im, Mat& orig, Mat gray, double* skew);
Mat preprocess1(Mat& im);
Mat preprocess2(Mat& im);
Mat rot(Mat& im, double thetaRad);
Mat slantCorrection(Mat& im);

// 최단 경로
void shortestPath(int startIdx, int endIdx, pGraph g);
int V = 53;
int path[MAX_PICTURE];

// 창 이름
const char* win_name = "STREET VIEW ver0.1 :: HANDONG";
const char* txtFileName = "OH1234f.txt";
const char* imageFileName = "OH\\idx%d_camera%d.jpg";
char strPath[MAX_TEXT];
const int N = 0, W = 1, S = 2, E = 3;