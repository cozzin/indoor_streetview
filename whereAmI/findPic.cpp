//  STREET VIEW ver0.1 :: HANDONG
//
//  Created by Seongho Hong / Seung Hee Ko / Mihyun Wendy Yang / on 2015. 7. 16..
//  Copyright (c) 2015�� Seongho Hong. All rights reserved.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream> // �ؽ�Ʈ ���� ���
#include <fstream> // �ؽ�Ʈ ���� �б�
#include <conio.h> // ����Ű �б�
#include <math.h>
#include <io.h> // ���� ���丮 ã��

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
	char tagging[MAX_TEXT]; // ��� ���� �±�

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
void makeDirectionMap(PTS_INFO* io_dstMap, int &out_num_pic, char * directionMapFileName); // ����ǥ �����
void makeTxtFile(PTS_INFO* io_dstMap, char * in_directionMapFileName, int in_pts_num); // �ؽ�Ʈ ���Ϸ� ����� 
void modifyDirectionMap(PTS_INFO* io_dstMap, char * in_directionMapFileName, int in_pts_num); // ����ǥ ����
int isDirectionMapExist(char* file); // ���� ���� ���� Ȯ��
void readTextDirectionMap(char * in_directionMapFileName, PTS_INFO* &io_dstMap); // ����ǥ txt �о����

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

// â �̸�
const char* win_name = "STREET VIEW ver0.1 :: HANDONG";
const char* txtFileName = "OH1234f.txt";
const char* imageFileName = "OH\\idx%d_camera%d.jpg";
char strPath[MAX_TEXT];
const int N = 0, W = 1, S = 2, E = 3;

int main(){

	PTS_INFO dirMap[MAX_PICTURE];
	int num_pic;
	char directionMapFileName[MAX_TEXT] = "";

	makeDirectionMap(dirMap, num_pic, directionMapFileName); // ����ǥ �����

	//���� �ʱ�ȭ
	int index = 0;
	int indexNum = -2;
	int user_compass = 0;

	//�ε��� �ް� ùȭ��
	while ((num_pic < indexNum) || (indexNum < 0)){
		system("cls");
		showDirMap(dirMap, num_pic);
		cout << "����ǥ �����Ϸ��� -1, �ε��� ��ȣ�� �̵��Ϸ��� 0 ~ " << (num_pic - 1) << " �Է�" << endl;
		cout << "input index number : ";
		cin >> indexNum;

		if ((num_pic < indexNum) || (indexNum < -1)){
			cout << "Error : out of range !" << endl;
			cout << "You have to input 0 < input < " << num_pic - 1 << endl;
			return -1;
		}

		else if (indexNum == -1){
			modifyDirectionMap(dirMap, directionMapFileName, num_pic);   //����ǥ ����
		}
	}

	Mat streetImage = imread(getFileName(dirMap[indexNum], 0)); //���� �ҷ����� 
	Mat revisedImage = slantCorrection(streetImage);

	MOUSECHANGE mouse;
	namedWindow(win_name, WINDOW_NORMAL);
	setMouseCallback(win_name, callBackFunc, &mouse); // ���콺 Ŭ�� �̺�Ʈ ����
	drawArrowAndGps(revisedImage, dirMap[indexNum], user_compass);// ȭ��ǥ ����
	mouse.user_dir = user_compass;
	imshow(win_name, revisedImage);
	resizeWindow(win_name, 640, 480);
	mouse.frameSize = revisedImage.size();

	while ((mouse.change == 0) || (dirMap[indexNum].direc[mouse.direction] == -1)){
		waitKey(20);
	}

	Mat temp = streetImage.clone();

	while (1){
		if ((mouse.change == 1) && (dirMap[indexNum].direc[mouse.direction] != -1)){      // ������ �ϴ� ���⿡ ������ ���� ����
			system("cls");
			showDirMap(dirMap, num_pic);
			indexNum = dirMap[indexNum].direc[mouse.direction];   //indexNUm�� �� ���� index �־��ֱ�
			user_compass = mouse.direction;
			streetImage = imread(getFileName(dirMap[indexNum], user_compass), 1); // ���� �ҷ�����
			revisedImage = slantCorrection(streetImage);
			drawArrowAndGps(revisedImage, dirMap[indexNum], user_compass); // ȭ��ǥ ����
			mouse.user_dir = user_compass;
			imshow(win_name, revisedImage);
			mouse.frameSize = revisedImage.size();
		}
		else if ((mouse.change == 4) && (dirMap[indexNum].updown[0] != -1))  {
			system("cls");
			showDirMap(dirMap, num_pic);
			indexNum = dirMap[indexNum].updown[0];   //indexNUm�� �� ���� index �־��ֱ�
			streetImage = imread(getFileName(dirMap[indexNum], user_compass), 1); // ���� �ҷ�����
			revisedImage = slantCorrection(streetImage);
			drawArrowAndGps(revisedImage, dirMap[indexNum], user_compass); // ȭ��ǥ ����
			imshow(win_name, revisedImage);
			mouse.frameSize = revisedImage.size();

		}
		else if ((mouse.change == 5) && (dirMap[indexNum].updown[1] != -1)){
			system("cls");
			showDirMap(dirMap, num_pic);
			indexNum = dirMap[indexNum].updown[1];   //indexNUm�� �� ���� index �־��ֱ�
			streetImage = imread(getFileName(dirMap[indexNum], user_compass), 1); // ���� �ҷ�����
			revisedImage = slantCorrection(streetImage);
			drawArrowAndGps(revisedImage, dirMap[indexNum], user_compass); // ȭ��ǥ ����
			imshow(win_name, revisedImage);
			mouse.frameSize = revisedImage.size();

		}

		revisedImage = slantCorrection(streetImage);
		slideImage(revisedImage, dirMap[indexNum], mouse, user_compass, indexNum); // ���� ������ UI

		mouse.change = 0; // 0���� �ٲ��־� �ٽ� ���콺 Ŭ�� ���� ���� �� �� �ֵ���
		waitKey(200);
	}

	//����
	destroyWindow(win_name);
	return 0;
}

///////////////////////////////////
//////��Ʈ��Ʈ �� ȭ�� ��Ʈ��//////
///////////////////////////////////

// jpg ���� �̸� �޾ƿ���
// compass���� ����ڰ� �ٶ󺸰� �ִ� ���� ������ �޾ƿ�
// �⺻������ �������� ���� 0 
String getFileName(PTS_INFO map, int compass){
	// 0 �� 1 �� 2 �� 3 ��
	char buf[MAX_TEXT];
	sprintf(buf, imageFileName, map.id, compass);
	//sprintf(buf, "%d.jpg", fileIndex,compass);

	String dst = buf;
	cout << buf << endl;
	return dst;
}

// index���� ���� �����ֱ�
void showRelation(int(*map)[MAX_DIRECTION]){

	cout << "########### STREET VIEW ver1.0 ##########" << endl;
	cout << "#index\t��\t��\t��\t��\t#" << endl;

	for (int i = 0; i < MAX_PICTURE; i++){
		cout << "# " << i << '\t';
		for (int j = 0; j < MAX_DIRECTION; j++){
			cout << map[i][j] << '\t';
		}
		cout << "#" << endl;
	}
	cout << "#########################################" << endl;
}

// ��ǥ ���� ���� ���� �����ֱ�
void showDirMap(PTS_INFO* in_dstMap, int in_num_pic){
	cout << "############### STREET VIEW ver0.1 ##############" << endl;
	cout << "#index\t��\t��\t��\t��\t��\t#" << endl;

	for (int i = 0; i < in_num_pic; i++){
		cout << "# " << i << '\t';
		for (int j = 0; j < MAX_DIRECTION; j++){
			cout << in_dstMap[i].direc[j] << '\t';
		}
		cout << in_dstMap[i].floor << "\t";

		cout << "#" << endl;
	}
	cout << "#################################################" << endl;
}

// txt �б�
void readText(String fileName, PTS_INFO* &io_dstMap){
	ifstream file(fileName);
	if (!file.is_open()){
		cout << "file is not exist" << endl;
		return;
	}
	char buffer[MAX_TEXT];
	char* token;
	int i = 0;
	while (!file.eof()){

		file.getline(buffer, 100);
		token = strtok(buffer, "\"id");

		for (int j = 0; token != NULL; j++){
			if (j == 0){
				io_dstMap[i].id = atoi(token);
				cout << token << endl;
			}
			else if (j == 2){ // 3��°
				io_dstMap[i].y = atof(token);
				cout << i << " : y ��ǥ " << token << endl;
			}
			else if (j == 3){ // 4��°
				io_dstMap[i].x = atof(token);
				cout << i << " : x ��ǥ " << token << endl;
			}
			else if (j == 4){ // 5��°
				io_dstMap[i].floor = atoi(token);
				cout << i << " : floor " << io_dstMap[i].floor << endl;
			}
			else if (j == 5) { //6��°
				io_dstMap[i].capturedCompass = atoi(token);
				cout << i << " : �Կ� �� �� ��ħ�� ����" << token << endl;
			}

			if (j == 3){
				token = strtok(NULL, "\";AF");
			}
			else{
				token = strtok(NULL, "\";");
			}


		}
		i++;
		cout << endl;
	}

	file.close();
}

// ���콺 �̺�Ʈ
void callBackFunc(int event, int x, int y, int flags, void* userdata){

	MOUSECHANGE* mouse = (MOUSECHANGE*)userdata;
	int rows = mouse->frameSize.height;
	int cols = mouse->frameSize.width;

	if (event == EVENT_LBUTTONDOWN){
		//cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
		// if ȭ��ǥ ���� Ŭ���ϸ� ��, ��, ��, ��
		//cout << "cols����" << cols/2 << endl;
		//cout << "rows����" << rows / 2 << endl;

		if (((x> cols / 2) && (x < cols / 2 + 100)) && (y>rows / 2 - 25) && (y < rows / 2 + 25)){
			mouse->direction = (mouse->user_dir + 3) % 4; //��
			mouse->change = 1;
		}

		else if (((x>cols / 2 - 100) && (x < cols / 2)) && (y>rows / 2 - 25) && (y < rows / 2 + 25)){
			mouse->direction = (mouse->user_dir + 1) % 4; //��
			mouse->change = 1;
		}

		else if (((x>cols / 2 - 25) && (x < cols / 2 + 25)) && (y>rows / 2) && (y < rows / 2 + 50)){
			mouse->direction = (mouse->user_dir + 2) % 4; //��
			mouse->change = 1;
			//cout << "��" << endl;
		}
		else if (((x>cols / 2 - 25) && (x < cols / 2 + 25)) && (y>rows / 2 - 50) && (y < rows / 2)){
			mouse->direction = (mouse->user_dir) % 4; //��
			mouse->change = 1;
			//cout << "��" << endl;
		}

		// ���� ������ ����
		if ((x > 0) && (x < cols * 1 / 15) && (y>0) && (y < rows)){
			//cout << "hi" << endl;
			mouse->change = 2;
		}
		else if ((x> cols * 14 / 15) && (x < cols) && (y>0) && (y < rows)){
			//cout << "there" << endl;
			mouse->change = 3;
		}

		// ���� �Ʒ��� �̵�
		if (((x>cols / 2 - 25) && (x < cols / 2 + 25)) && (y>0) && (y < rows * 1 / 10)){
			mouse->change = 4;
			//cout << "up" << endl;
		}
		else if (((x>cols / 2 - 25) && (x < cols / 2 + 25)) && (y>rows * 9 / 10) && (y < rows)){
			mouse->change = 5;
			//cout << "down" << endl;
		}
	}
	else if (event == EVENT_RBUTTONDOWN){
		//cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MBUTTONDOWN){
		//cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MOUSEMOVE){
		//cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;


	}
}

//ȭ��ǥ �׷��ֱ�
void drawArrowAndGps(Mat img, PTS_INFO map, int & in_user_compass){

	// user compass���� 0�� ī�޶��� ��ħ���� �����Դٰ� �����ϸ�


	if (map.direc[in_user_compass % 4] != -1){ // ��
		arrowedLine(img, Point(img.cols / 2, img.rows / 2), Point(img.cols / 2, img.rows / 2 - 50), Scalar(255, 255, 255), 5);
	}
	if (map.direc[(in_user_compass + 1) % 4] != -1){ // ��
		arrowedLine(img, Point(img.cols / 2, img.rows / 2), Point(img.cols / 2 - 100, img.rows / 2), Scalar(255, 255, 255), 5);
	}
	if (map.direc[(in_user_compass + 2) % 4] != -1){ // ��
		arrowedLine(img, Point(img.cols / 2, img.rows / 2), Point(img.cols / 2, img.rows / 2 + 50), Scalar(255, 255, 255), 5);
	}
	if (map.direc[(in_user_compass + 3) % 4] != -1){ // ��
		arrowedLine(img, Point(img.cols / 2, img.rows / 2), Point(img.cols / 2 + 100, img.rows / 2), Scalar(255, 255, 255), 5);
	}

	if (map.updown[0] != -1){ // ��
		arrowedLine(img, Point(img.cols / 2, img.rows * 1 / 10), Point(img.cols / 2, 0), Scalar(0, 255, 255), 5);
		//putText(img, floorText, Point(img.cols * 4 / 5, img.rows * 7 / 10), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 2);

	}
	if (map.updown[1] != -1){ // �Ʒ�
		arrowedLine(img, Point(img.cols / 2, img.rows * 9 / 10), Point(img.cols / 2, img.rows), Scalar(0, 255, 255), 5);
		//putText(img, floorText, Point(img.cols * 4 / 5, img.rows * 7 / 10), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 2);

	}

	char xText[MAX_TEXT];
	char yText[MAX_TEXT];
	char floorText[MAX_TEXT];

	sprintf(xText, "X :%f", map.x);
	sprintf(yText, "Y :%f", map.y);
	sprintf(floorText, "Floor : %d", map.floor);

	putText(img, floorText, Point(img.cols * 4 / 5, img.rows * 7 / 10), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 2);
	putText(img, xText, Point(img.cols * 4 / 5, img.rows * 8 / 10), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 2);
	putText(img, yText, Point(img.cols * 4 / 5, img.rows * 9 / 10), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 2);
}


// �¿� ȭ�� ��ȯ
// ���콺 �̺�Ʈ, ������� ������ ���
// �̹���, ��������, ���콺 ó��, ����� ����, �ε���
void slideImage(Mat & io_streetImage, PTS_INFO map, MOUSECHANGE & io_mouse, int & io_user_compass, int in_indexNum){

	Mat revisedImage;
	io_streetImage.copyTo(revisedImage);

	if (io_mouse.change == 2)
	{
		if (io_user_compass == E){
			io_user_compass = N;
			io_streetImage = imread(getFileName(map, N), 1);
			revisedImage = slantCorrection(io_streetImage);
			drawArrowAndGps(revisedImage, map, io_user_compass); // ȭ��ǥ ����
			imshow(win_name, revisedImage);
		}
		else if (io_user_compass == N){
			io_user_compass = W;
			io_streetImage = imread(getFileName(map, W), 1);
			revisedImage = slantCorrection(io_streetImage);
			drawArrowAndGps(revisedImage, map, io_user_compass); // ȭ��ǥ ����
			imshow(win_name, revisedImage);

		}
		else if (io_user_compass == W){
			io_user_compass = S;
			io_streetImage = imread(getFileName(map, S), 1);
			revisedImage = slantCorrection(io_streetImage);
			drawArrowAndGps(revisedImage, map, io_user_compass); // ȭ��ǥ ����
			imshow(win_name, revisedImage);

		}
		else if (io_user_compass == S){
			io_user_compass = E;
			io_streetImage = imread(getFileName(map, E), 1);
			revisedImage = slantCorrection(io_streetImage);
			drawArrowAndGps(revisedImage, map, io_user_compass); // ȭ��ǥ ����
			imshow(win_name, revisedImage);
		}
		io_mouse.user_dir = io_user_compass;
		io_mouse.frameSize = io_streetImage.size();
	}


	else if (io_mouse.change == 3)
	{
		if (io_user_compass == E){
			io_user_compass = S;
			io_streetImage = imread(getFileName(map, io_user_compass), 1);
			revisedImage = slantCorrection(io_streetImage);
			drawArrowAndGps(revisedImage, map, io_user_compass); // ȭ��ǥ ����
			imshow(win_name, revisedImage);

		}
		else if (io_user_compass == S){
			io_user_compass = W;
			io_streetImage = imread(getFileName(map, io_user_compass), 1);
			revisedImage = slantCorrection(io_streetImage);
			drawArrowAndGps(revisedImage, map, io_user_compass); // ȭ��ǥ ����
			imshow(win_name, revisedImage);
		}
		else if (io_user_compass == W){
			io_user_compass = N;
			io_streetImage = imread(getFileName(map, io_user_compass), 1);
			revisedImage = slantCorrection(io_streetImage);
			drawArrowAndGps(revisedImage, map, io_user_compass); // ȭ��ǥ ����
			imshow(win_name, revisedImage);

		}
		else if (io_user_compass == N){
			io_user_compass = E;
			io_streetImage = imread(getFileName(map, io_user_compass), 1);
			revisedImage = slantCorrection(io_streetImage);
			drawArrowAndGps(revisedImage, map, io_user_compass); // ȭ��ǥ ����
			imshow(win_name, revisedImage);

		}
		io_mouse.user_dir = io_user_compass;

		io_mouse.frameSize = io_streetImage.size();

	}
}



/////////////////////////////////////
//////��ǥ�� ���� ����ǥ �����//////
/////////////////////////////////////

//�� ���� �Ÿ� ���
double F_dist(double x1, double y1, double x2, double y2){
	return sqrt(((x1 - x2)*(x1 - x2)) + ((y1 - y2)*(y1 - y2)));
}

//�� ���� ���� ���
double F_ang(double x1, double y1, double x2, double y2){
	return (atan2((double)(y2 - y1), (double)(x2 - x1))*180.0f / 3.14159265);
}

//����ǥ ����� (���赵, ���� ����, ���赵 txt ���ϸ�)
void makeDirectionMap(PTS_INFO* io_dstMap, int &out_num_pic, char * io_directionMapFileName) {

	int i = 0, n = 0, j = 0;
	int fileExist;
	//char directionMapFileName[MAX_TEXT] = ""; // char �ʱ�ȭ
	cout << "���赵�� ������ �ؽ�Ʈ ���ϸ��� �Է��� �ּ��� (ex.OH123.txt) : ";
	cin >> io_directionMapFileName; // �Է¹���
	fileExist = isDirectionMapExist(io_directionMapFileName); // ���� ���� ���� Ȯ��

	cout << endl << "���� ������ �Է��Ͻÿ� \n" << endl;
	cin >> n;
	out_num_pic = n;

	if (fileExist == 0){
		double ang_result[MAX_PICTURE][MAX_PICTURE], dis_result[MAX_PICTURE][MAX_PICTURE];

		// 2���� �迭�� ���. �ʱ�ȭ
		for (i = 0; i < n; i++){
			io_dstMap[i].x = io_dstMap[i].y = 0.0; // x,y ��ǥ �ʱ�ȭ
			strcpy(io_dstMap[i].tagging, ""); // �±����� �ʱ�ȭ

			for (j = 0; j < 4; j++){
				io_dstMap[i].length[j] = MAX_DIST;
				io_dstMap[i].direc[j] = -1;
			}

			for (j = 0; j < 2; j++){
				io_dstMap[i].updown_length[j] = MAX_DIST;
				io_dstMap[i].updown[j] = -1;
			}


			for (j = 0; j < n; j++)
				ang_result[i][j] = dis_result[i][j] = 0.0;
		}

		// �ؽ�Ʈ ���Ͽ� �ִ� ���� ��ǥ�� �ҷ���
		readText(txtFileName, io_dstMap);

		for (i = 0; i < n; i++){
			for (j = 0; j < n; j++){
				if (i == j || io_dstMap[i].floor != io_dstMap[j].floor) continue;

				ang_result[i][j] = F_ang(io_dstMap[i].x, io_dstMap[i].y, io_dstMap[j].x, io_dstMap[j].y);
				dis_result[i][j] = F_dist(io_dstMap[i].x, io_dstMap[i].y, io_dstMap[j].x, io_dstMap[j].y);

				//printf("i:%d j:%d %f %f ang %f\n", i, j, io_dstMap[i].x, io_dstMap[i].y, ang_result[i][j]);

				if (-45 <= ang_result[i][j] && ang_result[i][j] < 45){
					//���� 0�� 1. 0�� 3
					if ((io_dstMap[i].length[3] >= dis_result[i][j])){
						io_dstMap[i].direc[3] = j;
						//io_dstMap[j].direc[1] = i;
						io_dstMap[i].length[3] = dis_result[i][j];
					}
				}

				else if (45 <= ang_result[i][j] && ang_result[i][j] < 135){
					//����
					if (io_dstMap[i].length[0] >= dis_result[i][j]){
						io_dstMap[i].direc[0] = j;
						//io_dstMap[j].direc[2] = i;
						io_dstMap[i].length[0] = dis_result[i][j];
						//printf("%d���� %d���� �Ÿ� : %f \n",i,j, io_dstMap[i].length[0]);
					}
				}

				else if (135 <= ang_result[i][j] || ang_result[i][j] < -135){
					//����
					if (io_dstMap[i].length[1] >= dis_result[i][j]){
						io_dstMap[i].direc[1] = j;
						//io_dstMap[j].direc[3] = i;
						io_dstMap[i].length[1] = dis_result[i][j];
					}
				}

				else if (-135 <= ang_result[i][j] && ang_result[i][j] < -45){
					//����
					if (io_dstMap[i].length[2] >= dis_result[i][j]){
						io_dstMap[i].direc[2] = j;
						//io_dstMap[j].direc[0] = i;
						io_dstMap[i].length[2] = dis_result[i][j];
					}
				}
			}

			for (int k = 0; k < 4; k++){
				if (io_dstMap[i].length[k] > 0.0001) io_dstMap[i].direc[k] = -1;
			}

		}



		makeTxtFile(io_dstMap, io_directionMapFileName, out_num_pic);//�ؽ�Ʈ ���Ϸ� ����
	}
	else{
		readTextDirectionMap(io_directionMapFileName, io_dstMap);
	}
}

//����ǥ �ؽ�Ʈ ���Ϸ� ����� ( ���赵, ���赵 txt ���ϸ� )
void makeTxtFile(PTS_INFO* io_dstMap, char * in_directionMapFileName, int in_pts_num){

	//ofstream outTxt(in_directionMapFileName);
	FILE *outTxt;
	outTxt = fopen(in_directionMapFileName, "w");
	// id / �ϼ����� / �Ÿ� / x, y / �� / �±�����

	for (int i = 0; i<in_pts_num; i++){
		fprintf(outTxt, "%d\t", i);
		fprintf(outTxt, "%d\t%d\t%d\t%d\t", io_dstMap[i].direc[0], io_dstMap[i].direc[1], io_dstMap[i].direc[2], io_dstMap[i].direc[3]);  // �ϼ�����
		fprintf(outTxt, "%d\t%d\t", io_dstMap[i].updown[0], io_dstMap[i].updown[1]);  // �� �Ʒ�
		fprintf(outTxt, "%f\t%f\t%f\t%f\t", io_dstMap[i].length[0], io_dstMap[i].length[1], io_dstMap[i].length[2], io_dstMap[i].length[3]); // �ϼ����� ����
		fprintf(outTxt, "%f\t%f\t", io_dstMap[i].updown_length[0], io_dstMap[i].updown_length[1]); // �� �Ʒ� ����
		fprintf(outTxt, "%f\t%f\t", io_dstMap[i].x, io_dstMap[i].y); // x,y ��ǥ
		fprintf(outTxt, "%d\t", io_dstMap[i].floor); // ��
		fprintf(outTxt, "%s\t", io_dstMap[i].tagging); // �±�����
		fprintf(outTxt, "\n");
	}

	//outTxt.close();
	fclose(outTxt);

	cout << txtFileName << " ���������� ����Ǿ����ϴ� :D" << endl;
	cout << "( �� ��ҵ��� tab���� ���еǾ� ���� )" << endl << endl;
}

//����ǥ ���� ( ���赵, ���赵 txt ���ϸ� )
void modifyDirectionMap(PTS_INFO* io_dstMap, char * in_directionMapFileName, int in_pts_num){
	char choice = 'y';
	while (choice == 'y'){
		int in_index = -1, in_direction = -1, in_pointIndex = -2;

		cout << "������ �ε���, ����, ����ų �ε����� �Է����ּ��� : " << endl;
		cin >> in_index >> in_direction >> in_pointIndex;

		if (in_pointIndex == -1){
			io_dstMap[in_index].direc[in_direction] = in_pointIndex;
			io_dstMap[in_index].length[in_direction] = 10000000.;

			system("cls");
			showDirMap(io_dstMap, in_pts_num);

			cout << "index " << in_index << "�� " << in_direction << "������ " << in_pointIndex << "�� �����Ǿ����ϴ�" << endl;

		}
		else{
			io_dstMap[in_index].direc[in_direction] = in_pointIndex;
			io_dstMap[in_pointIndex].direc[(in_direction + MAX_DIRECTION / 2) % MAX_DIRECTION] = in_index; // �����ϴ������� �ݴ����

			io_dstMap[in_index].length[in_direction] = F_dist(io_dstMap[in_index].x, io_dstMap[in_index].y, io_dstMap[in_pointIndex].x, io_dstMap[in_pointIndex].y);
			io_dstMap[in_pointIndex].length[(in_direction + MAX_DIRECTION / 2) % MAX_DIRECTION] = F_dist(io_dstMap[in_index].x, io_dstMap[in_index].y, io_dstMap[in_pointIndex].x, io_dstMap[in_pointIndex].y);

			system("cls");
			showDirMap(io_dstMap, in_pts_num);

			cout << "index " << in_index << "�� " << in_direction << "������ " << in_pointIndex << "�� �����Ǿ����ϴ�" << endl;
			cout << "index " << in_pointIndex << "�� " << (in_direction + MAX_DIRECTION / 2) % MAX_DIRECTION << "������ " << in_index << "�� �����Ǿ����ϴ�" << endl;

		}



		cout << "�� �����Ͻðڽ��ϱ�? (y/n) : " << endl;
		cin >> choice;

	}

	cout << in_directionMapFileName << "�� �����Ͻðڽ��ϱ�? (y/n) : " << endl;
	cin >> choice;
	if (choice = 'y'){
		makeTxtFile(io_dstMap, in_directionMapFileName, in_pts_num);
	}
	cout << "���赵 ���� �Ϸ�!" << endl;
}

// ������ �Է��� �ǹ��� ���赵(����)�� �����ϴ��� Ȯ��
int isDirectionMapExist(char* file)
{
	strcat(strPath, file);
	int nResult = access(strPath, 0);
	if (nResult == 0)
	{
		printf("�ش� �ǹ��� ����(���赵)�� �����մϴ�.");
		return 1;
	}
	else if (nResult == -1)
	{
		printf("�ش� �ǹ��� ����(���赵)�� �������� �ʽ��ϴ�.");
		return 0;
	}
}

// ����ǥ txt �о����
void readTextDirectionMap(char * in_directionMapFileName, PTS_INFO* &io_dstMap){
	ifstream file(in_directionMapFileName);

	char buffer[MAX_TEXT];
	char* token;
	int i = 0;
	while (!file.eof()){

		file.getline(buffer, 200);
		token = strtok(buffer, "\t");

		for (int j = 0; token != NULL; j++){
			if (j == 0){ // id
				io_dstMap[i].id = atoi(token);
			}
			else if (j == 1){ // ��
				io_dstMap[i].direc[0] = atoi(token);
			}
			else if (j == 2){ // ��
				io_dstMap[i].direc[1] = atoi(token);
			}
			else if (j == 3){ // ��
				io_dstMap[i].direc[2] = atoi(token);
			}
			else if (j == 4){ // ��
				io_dstMap[i].direc[3] = atoi(token);
			}

			else if (j == 5){ // ��
				io_dstMap[i].updown[0] = atoi(token);
			}
			else if (j == 6){ // �Ʒ�
				io_dstMap[i].updown[1] = atoi(token);
			}

			else if (j == 7){ // �� ����
				io_dstMap[i].length[0] = atof(token);
			}
			else if (j == 8){ // �� ����
				io_dstMap[i].length[1] = atof(token);
			}
			else if (j == 9){ // �� ����
				io_dstMap[i].length[2] = atof(token);
			}
			else if (j == 10){ // �� ����
				io_dstMap[i].length[3] = atof(token);
			}

			else if (j == 11){ // �� ����
				io_dstMap[i].updown_length[0] = atof(token);
			}
			else if (j == 12){ // �� ����
				io_dstMap[i].updown_length[1] = atof(token);
			}

			else if (j == 13){ // x ��ǥ
				io_dstMap[i].x = atof(token);
			}
			else if (j == 14){ // y ��ǥ
				io_dstMap[i].y = atof(token);
			}
			else if (j == 15){ // ��
				io_dstMap[i].floor = atoi(token);
			}
			else if (j == 16){ // ��
				strcpy(io_dstMap[i].tagging, token);
			}
			token = strtok(NULL, "\t");
		}

		i++;
		cout << endl;
	}

	file.close();
}

void hough_transform(Mat& im, Mat& orig, Mat gray, double* skew)
{
	double max_r = sqrt(pow(.5*im.cols, 2) + pow(.5*im.rows, 2));
	int angleBins = 180;
	Mat acc = Mat::zeros(Size(2 * max_r, angleBins), CV_32SC1);
	int cenx = im.cols / 2;
	int ceny = im.rows / 2;
	Mat vert, hori;
	orig.copyTo(vert);
	orig.copyTo(hori);

	*skew = 0.;

	for (int x = 1; x < im.cols - 1; x++)
	{
		for (int y = 1; y < im.rows - 1; y++)
		{
			if (im.at<uchar>(y, x) == 255)      // edge ����
			{
				for (int t = 0; t < angleBins; t++)
				{
					double r = (x - cenx)*cos((double)t / angleBins*CV_PI) + (y - ceny)*sin((double)t / angleBins*CV_PI);
					r += max_r;
					acc.at<int>(t, int(r))++;
				}
			}
		}
	}
	Mat thresh;
	normalize(acc, acc, 255, 0, NORM_MINMAX);
	convertScaleAbs(acc, acc);

	Point maxLoc, minLoc;
	minMaxLoc(acc, 0, 0, &minLoc, &maxLoc);   // ���� �ѷ��� ������

	double theta = (double)maxLoc.y / angleBins*CV_PI;   // ���� ���� ��ǥ�� y���� �������� �� ����

	double rho = -maxLoc.x + max_r;      // ����~���� ���� ��ǥ������ �Ÿ�

	Mat revisedImg, w_thres, b_thres, add, canny, rgb_canny, dst;
	vector<Vec4i> lines;
	Point center;

	double vertGrad[arraySize] = { 0.0 }, horiGrad[arraySize] = { 0.0 };
	double vertFrag[arraySize] = { 0.0 }, horiFrag[arraySize] = { 0.0 };
	int vertCount = 0, horiCount = 0;
	double vertGAVG = 0., vertFAVG = 0., horiGAVG = 0., horiFAVG = 0.;
	double vertSkewangle = 0., horiSkewangle = 0.;


	// �� ���Ⱑ ��Ƽ� ª�� ���´�.
	threshold(gray, w_thres, 120, 255, 0);
	inRange(gray, Scalar::all(0), Scalar::all(20), b_thres);

	add = b_thres + w_thres;
	Canny(add, canny, 50, 150, 3);

	cvtColor(canny, rgb_canny, CV_GRAY2BGR);

	HoughLinesP(canny, lines, 1, CV_PI / 180, 30, 10, 30);

	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4d I = lines[i];
		center = Point(I[2], I[3]) - Point(I[0], I[1]);
		double rad = atan2(center.x, center.y);
		double degree = (rad * 180) / CV_PI;
		double a, b;

		if (degree >0 && degree < 10){   // ����(y��)�� ����� �� ����

			b = I[1] - I[0] * degree;
			vertGrad[vertCount] = degree;
			vertFrag[vertCount++] = b;
			line(rgb_canny, Point(I[0], I[1]), Point(I[2], I[3]), Scalar(0, 0, 255), 1);

		}
		else if (degree>170 && degree < 180){
			degree = degree - 180;
			b = I[1] - I[0] * degree;
			if (degree >-10 && degree < 0) vertGrad[vertCount] = degree;

			vertFrag[vertCount++] = b;
			line(rgb_canny, Point(I[0], I[1]), Point(I[2], I[3]), Scalar(0, 0, 255), 1);

		}
		else if (degree > 80 && degree < 90) // ���� (x��)�� ����� �� ����
		{

			b = I[1] - I[0] * degree;
			horiGrad[horiCount] = degree;
			horiFrag[horiCount++] = b;
			line(rgb_canny, Point(I[0], I[1]), Point(I[2], I[3]), Scalar(0, 0, 255), 1);

		}
		else if (degree >90 && degree < 110){
			degree = degree - 180;
			b = I[1] - I[0] * degree;
			if (degree > -90 && degree < -80) horiGrad[horiCount] = degree;
			horiFrag[horiCount++] = b;
			line(rgb_canny, Point(I[0], I[1]), Point(I[2], I[3]), Scalar(0, 0, 255), 1);

		}
	}


	for (int j = 0; j < vertCount; j++)
	{
		vertGAVG += vertGrad[j];
		vertFAVG += vertFrag[j];
	}
	for (int j = 0; j < horiCount; j++)
	{
		horiGAVG += horiGrad[j];
		horiFAVG += horiFrag[j];
	}
	if (vertCount != 0.){
		vertGAVG /= vertCount;
		vertFAVG /= vertCount;

	}
	if (horiCount != 0.){
		horiGAVG /= horiCount;
		horiFAVG /= horiCount;
	}

	if (abs(sin(theta)) < 0.000001)//check vertical    <0.000001
	{
		cout << abs(sin(theta)) << endl;
		cout << abs(cos(theta)) << endl;
		double m = -cos(theta) / sin(theta);
		double b = rho / sin(theta) + im.rows / 2. - m*im.cols / 2.;
		Point2d p1 = Point2d(rho + im.cols / 2, 0);
		Point2d p2 = Point2d(rho + im.cols / 2, im.rows);
		line(orig, p1, p2, Scalar(0, 0, 255), 1);
		double skewangle;
		skewangle = p1.x - p2.x > 0 ? (atan2(p1.y - p2.y, p1.x - p2.x)*180. / CV_PI) : (atan2(p2.y - p1.y, p2.x - p1.x)*180. / CV_PI);
		*skew = skewangle;

	}


	if ((vertGAVG > -10 && vertGAVG < 0) || (vertGAVG> 0 && vertGAVG < 10)){

		Point2d p1 = Point2d(0, vertFAVG);
		Point2d p2 = Point2d(im.cols, im.cols*vertGAVG + vertFAVG);
		line(vert, p1, p2, Scalar(0, 0, 255), 1);

		vertSkewangle = p1.y - p2.y>0 ? (atan2(p1.y - p2.y, p1.x - p2.x)*180. / CV_PI) : (atan2(p2.y - p1.y, p2.x - p1.x)*180. / CV_PI);   // ���μ�
		*skew = -vertGAVG;
	}

	else{
		*skew = 0;
	}

	if (((horiGAVG > 40 && horiGAVG < 90) || (horiGAVG >-90 && horiGAVG < -40)) || ((horiGAVG > 0 && horiGAVG < 20) || (horiGAVG >-20 && horiGAVG < 0))) // ���� (x��)�� ����� �� ����
	{

		if (horiGAVG > 0) horiGAVG = 90 - horiGAVG;
		else horiGAVG = 90 + horiGAVG;
		Point2d p1 = Point2d(0, horiFAVG);
		Point2d p2 = Point2d(im.cols, im.cols*horiGAVG + horiFAVG);
		line(hori, p1, p2, Scalar(0, 0, 255), 1);
		horiSkewangle = p1.x - p2.x > 0 ? (atan2(p1.y - p2.y, p1.x - p2.x)*180. / CV_PI) : (atan2(p2.y - p1.y, p2.x - p1.x)*180. / CV_PI);   // ���μ�

		horiSkewangle = 90 - horiSkewangle;
		horiSkewangle = abs(horiSkewangle) < horiGAVG ? horiSkewangle : horiGAVG;
		*skew = horiSkewangle;


	}
	else{
		*skew = 0;
	}




	if (horiSkewangle != 0. && vertGAVG != 0. && (horiSkewangle <10. && horiSkewangle >2.5)) {
		if (vertGAVG > -2. && vertGAVG < 2.)
			*skew = -vertGAVG;
		else if (abs(vertGAVG - horiSkewangle) > 2.0)
			*skew = abs(vertGAVG) < abs(horiSkewangle) ? -vertGAVG : horiSkewangle;
		else
			*skew = abs(vertGAVG) > abs(horiSkewangle) ? -vertGAVG : horiSkewangle;
	}
	else if (horiSkewangle != 0. && vertGAVG != 0. && (horiSkewangle <2.5 && horiSkewangle >0.)){
		if (abs(vertGAVG) > abs(vertSkewangle))
			*skew = *skew = abs(vertSkewangle) < abs(horiSkewangle) ? -vertSkewangle : horiSkewangle;
		else
			*skew = abs(vertGAVG) > abs(horiSkewangle) ? -vertGAVG : horiSkewangle;
	}
	else if (horiSkewangle != 0. && vertGAVG != 0. && (horiSkewangle > 10.)) *skew = abs(vertGAVG) > abs(horiGAVG) ? -vertGAVG : horiSkewangle;
	else if (horiSkewangle == 0)   *skew = -vertGAVG;
	else if (vertGAVG == 0) *skew = horiSkewangle;
	else if (horiSkewangle == 0 && vertGAVG == 0)   *skew = 0.;

	if ((vertSkewangle < 3. && vertSkewangle >0.) && (horiSkewangle <3. && horiSkewangle >0.))
		*skew = vertSkewangle > horiSkewangle ? -vertSkewangle : horiSkewangle;

	if (vertGAVG > 3.5) *skew = 0.;
	if (vertGAVG != 0 && horiSkewangle != 0 && abs(vertGAVG - horiSkewangle) < 1.3 && abs(vertGAVG - horiSkewangle) > 1.0){
		if (abs(vertGAVG) < abs(horiSkewangle))   * skew = horiSkewangle;
		else *skew = vertGAVG;
	}
	if (vertGAVG != 0 && horiSkewangle != 0 && abs(vertGAVG - horiSkewangle) < 1.0)   *skew = abs(vertGAVG - horiSkewangle);

}

Mat preprocess1(Mat& im)
{
	Mat ret = Mat::zeros(im.size(), CV_32SC1);

	for (int x = 1; x < im.cols - 1; x++)
	{
		for (int y = 1; y < im.rows - 1; y++)
		{

			int gy = (im.at<uchar>(y - 1, x + 1) - im.at<uchar>(y - 1, x - 1))
				+ 2 * (im.at<uchar>(y, x + 1) - im.at<uchar>(y, x - 1))
				+ (im.at<uchar>(y + 1, x + 1) - im.at<uchar>(y + 1, x - 1));
			int gx = (im.at<uchar>(y + 1, x - 1) - im.at<uchar>(y - 1, x - 1))
				+ 2 * (im.at<uchar>(y + 1, x) - im.at<uchar>(y - 1, x))
				+ (im.at<uchar>(y + 1, x + 1) - im.at<uchar>(y - 1, x + 1));
			int g2 = (gy*gy + gx*gx);
			ret.at<int>(y, x) = g2;
		}
	}
	normalize(ret, ret, 255, 0, NORM_MINMAX);
	ret.convertTo(ret, CV_8UC1);
	threshold(ret, ret, 50, 255, THRESH_BINARY);
	return ret;
}

Mat preprocess2(Mat& im)
{
	// 1) assume white on black and does local thresholding
	// 2) only allow voting top is white and buttom is black(buttom text line)
	Mat thresh;
	//thresh=255-im;
	thresh = im.clone();
	adaptiveThreshold(thresh, thresh, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 15, -2);
	Mat ret = Mat::zeros(im.size(), CV_8UC1);
	for (int x = 1; x < thresh.cols - 1; x++)
	{
		for (int y = 1; y < thresh.rows - 1; y++)
		{
			bool toprowblack = thresh.at<uchar>(y - 1, x) == 0 || thresh.at<uchar>(y - 1, x - 1) == 0 || thresh.at<uchar>(y - 1, x + 1) == 0;
			bool belowrowblack = thresh.at<uchar>(y + 1, x) == 0 || thresh.at<uchar>(y + 1, x - 1) == 0 || thresh.at<uchar>(y + 1, x + 1) == 0;

			uchar pix = thresh.at<uchar>(y, x);
			if ((!toprowblack && pix == 255 && belowrowblack))
			{
				ret.at<uchar>(y, x) = 255;
			}
		}
	}
	return ret;
}

Mat rot(Mat& im, double thetaRad)
{
	cv::Mat rotated;
	double rskew = thetaRad* CV_PI / 180;
	double nw = abs(sin(thetaRad))*im.rows + abs(cos(thetaRad))*im.cols;
	double nh = abs(cos(thetaRad))*im.rows + abs(sin(thetaRad))*im.cols;
	cv::Mat rot_mat = cv::getRotationMatrix2D(Point2d(nw*.5, nh*.5), thetaRad * 180 / CV_PI, 1);
	Mat pos = Mat::zeros(Size(1, 3), CV_64FC1);
	pos.at<double>(0) = (nw - im.cols)*.5;
	pos.at<double>(1) = (nh - im.rows)*.5;
	Mat res = rot_mat*pos;
	rot_mat.at<double>(0, 2) += res.at<double>(0);
	rot_mat.at<double>(1, 2) += res.at<double>(1);
	cv::warpAffine(im, rotated, rot_mat, Size(nw, nh), cv::INTER_LANCZOS4);
	return rotated;
}

Mat slantCorrection(Mat& src)
{

	Mat im;
	src.copyTo(im);
	Mat gray;
	cvtColor(im, gray, CV_BGR2GRAY);

	Mat preprocessed = preprocess2(gray);
	double skew;
	hough_transform(preprocessed, im, gray, &skew);
	Mat rotated = rot(im, skew* CV_PI / 180);

	return rotated;
}