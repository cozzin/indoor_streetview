#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream> // �ؽ�Ʈ ���� ���
#include <fstream> // �ؽ�Ʈ ���� �б�
#include <conio.h> // ����Ű �б�
#include <math.h>
#include <io.h> // ���� ���丮 ã��

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

//�Է¹��� ���� �ε����� ��ǥ��
typedef struct pts_info{
	int id; // �� id
	int direc[4]; // �ϼ�����
	int updown[2]; // �� �Ʒ�
	double updown_length[2]; // �� �Ʒ� ����
	double length[4]; // �� ������ ����
	double x, y; // x, y ��ǥ
	int capturedCompass; // ���̷μ����� x ��
	int floor; // �� ����
	char tagging[10]; // ��� ���� �±�

}PTS_INFO;

// ���콺 �̺�Ʈ ó��
typedef struct MouseChange{
	int direction = -1;
	int change = 0;
	Size frameSize; // ��ư ���� �����Ϸ��� ����
	int user_dir; // ������ �ٶ󺸴� ����
}MOUSECHANGE;

// ��ġ ����ǥ ���� �� ���
double F_dist(double x1, double y1, double x2, double y2); // ���� �Ÿ�
double F_ang(double x1, double y1, double x2, double y2); // ���� ����
void makeDirectionMap(PTS_INFO* io_dstMap, int &out_num_pic, char *directionMapFileName, pGraph g); // ����ǥ �����
void makeTxtFile(PTS_INFO* io_dstMap, char * in_directionMapFileName, int in_pts_num, pGraph g); // �ؽ�Ʈ ���Ϸ� ����� 
void modifyDirectionMap(PTS_INFO* io_dstMap, char * in_directionMapFileName, int in_pts_num, pGraph g); // ����ǥ ����
int isDirectionMapExist(char* file); // ���� ���� ���� Ȯ��
void readTextDirectionMap(char * in_directionMapFileName, PTS_INFO*	&io_dstMap, pGraph g); // ����ǥ txt �о����

// ��Ʈ��Ʈ �� ȭ�� ������ ���
void showRelation(int(*map)[MAX_DIRECTION]); // �ܼ�â�� �������� ���� �����ֱ�
void showDirMap(PTS_INFO *dirMap, int in_num_pic); // �ܼ�â�� �������� ���� �����ֱ�
String getFileName(PTS_INFO map, int compass); // ���� ������ .jpg�� ������ֱ�
void readText(String fileName, PTS_INFO* &io_dstMap); // �ؽ�Ʈ ���Ͽ��� ���� ���� �ҷ�����
void slideImage(Mat & io_streetImage, PTS_INFO map, MOUSECHANGE & io_mouse, int & io_user_compass, int in_indexNum); //�¿� ȭ�� ��ȯ


// ��Ʈ��Ʈ �� ȭ�� ��Ʈ��
//void drawArrow(Mat img, int map[MAX_DIRECTION]); // ȭ��ǥ �׸���
void drawArrowAndGps(Mat img, PTS_INFO mapint, int & in_user_compass);
void callBackFunc(int event, int x, int y, int flags, void* userdata); // ���콺 Ŭ�� �̺�Ʈ

// ��Ʈ��Ʈ �� ȭ�� �ڵ� ȸ��
void hough_transform(Mat& im, Mat& orig, Mat gray, double* skew);
Mat preprocess1(Mat& im);
Mat preprocess2(Mat& im);
Mat rot(Mat& im, double thetaRad);
Mat slantCorrection(Mat& im);

// �ִ� ���
void shortestPath(int startIdx, int endIdx, pGraph g);
int V = 53;
int path[MAX_PICTURE];

// â �̸�
const char* win_name = "STREET VIEW ver0.1 :: HANDONG";
const char* txtFileName = "OH1234f.txt";
const char* imageFileName = "OH\\idx%d_camera%d.jpg";
char strPath[MAX_TEXT];
const int N = 0, W = 1, S = 2, E = 3;