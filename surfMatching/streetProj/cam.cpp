#define MAX_CAM 2

#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
//#include "opencv2\stitching.hpp"
#include <iostream>
#include <cstring>
#include <ctime>

using namespace std;
using namespace cv;

void camCapture(int index, int cam_id, const char win_name[], Mat *frame);
string makeFileName(int index, int cam_id);


//string year, mon, day, hour, minute, sec;
//string currt = " ";

int index = 0;

void camCapture(int index, int cam_id, const char win_name[], Mat *frame){
	VideoCapture inVid(cam_id);// cam_id 카메라 열기
	inVid >> *frame;
	//if(cam_id != 2)
	imwrite(makeFileName(index, cam_id), *frame);
	imshow(win_name, *frame);
	inVid.release();
}

string makeFileName(int index, int cam_id){	//cam_id : 0=동, 1=서, 2=남, 3=북

	/*
	if (cam_id == 0){	// cam_id = 0인 카메라의 시간 정보만 받아오기
	time_t cur_time;
	struct tm* pTime;

	time(&cur_time);
	pTime = localtime(&cur_time);


	string zeroMon = "0";
	string zeroDay = "0";
	string zeroHour = "0";
	string zeroMin = "0";
	string zeroSec = "0";

	year = to_string(pTime->tm_year + 1900);
	mon = to_string(pTime->tm_mon + 1);
	day = to_string(pTime->tm_mday);
	hour = to_string(pTime->tm_hour);
	minute = to_string(pTime->tm_min);
	sec = to_string(pTime->tm_sec);

	if (pTime->tm_hour <= 9){	// 0~9시일 경우, 00~09시로 바꿔줌
	zeroHour.append(hour);
	hour = zeroHour;
	}
	if (pTime->tm_min <= 9){
	zeroMin.append(minute);
	minute = zeroMin;
	}
	if (pTime->tm_sec <= 9){
	zeroSec.append(sec);
	sec = zeroSec;
	}
	if (pTime->tm_mon <= 9){
	zeroMon.append(mon);
	mon = zeroMon;
	}
	if (pTime->tm_mday <= 9){
	zeroDay.append(day);
	day = zeroDay;
	}


	fileName = "camera";
	string fileNum = to_string(cam_id);
	fileName.append(fileNum);
	fileName.append(underbar);
	year.append(mon);
	year.append(day);
	year.append(hour);
	year.append(minute);
	year.append(sec);
	year.append(fileExtension);
	fileName.append(year);
	}
	*/
	string fileName = "idx";
	string idx = to_string(index);
	string underbar = "_";
	string fileNum = to_string(cam_id);
	string fileExtension = ".jpg";

	fileName.append(idx);
	fileName.append(underbar);
	fileName.append("camera");

	//if (cam_id >= 2)	// cam_id = 2인 노트북 내장 웹캠 사진 없애기
	fileNum = to_string((cam_id));

	fileName.append(fileNum);
	fileName.append(fileExtension);


	cout << fileName << "생성되었습니다.\n";
	return fileName;
}

int main(int argc, char *argv[]){
	Mat frame[MAX_CAM];

	vector<Mat> images; // 파노라마 전 vector에 저장
	Mat panorama; // 파노라마 출력 이미지
	const char win_name[MAX_CAM][10] = { "CAM1", "CAM2"};
	const char finalWinName[10] = "PANORAMA";
	double fps = 20;
	char x;
	unsigned long AAtime = 0, BBtime = 0; //check processing time
	//Stitcher stitcher = Stitcher::createDefault();

	/* 현재시간 저장하는 변수
	time_t cur_time;
	struct tm* pTime;

	time(&cur_time);
	pTime = localtime(&cur_time);

	string zeroMon = "0";
	string zeroDay = "0";
	string zeroHour = "0";
	string zeroMin = "0";
	string zeroSec = "0";
	*/

	for (int i = 0; i < MAX_CAM; i++){
		namedWindow(win_name[i]);
		resizeWindow(win_name[i], 320, 240);
	}

	for (int i = 0; i < MAX_CAM; i++){
		camCapture(index, i, win_name[i], &frame[i]);
		/*
		VideoCapture inVid(i);// cam_id 카메라 열기
		inVid >> *frame;
		imwrite(makeFileName(i), *frame);
		imshow(win_name[i], *frame);
		inVid.release();
		*/
	}
	index++;
	while (1){
		cout << "\n1. 새로운 사진 찍기\n2. 사진 다시 찍기\n3. 파노라마 만들기\n0. 끝내기\n어느 것을 하시겠습니까? \n";
		x = cvWaitKey(0);

		// 캡쳐
		if (x == '1'){
			for (int i = 0; i < MAX_CAM; i++){
				camCapture(index, i, win_name[i], &frame[i]);
			}
			index++;
		}
		// 사진을 다시 찍을 경우 찍은 사진 초기화. 다시 같은 index number를 가진 상태에서 찍기.
		else if (x == '2'){
			index--;
			for (int i = 0; i < MAX_CAM; i++){
				camCapture(index, i, win_name[i], &frame[i]);
			}
			index++;
		}
		// 파노라마 생성
		else if (x == 3){
			/*
			for (int j = 0; j < MAX_CAM; j++){
			destroyWindow(win_name[j]);
			if (j != 1)	images.push_back(frame[j]);	// CAM2 제외
			}

			cout << "\n파노라마 사진 생성 중\n";
			AAtime = getTickCount(); //check processing time

			//Stitcher::Status status = stitcher.composePanorama(images, panorama);
			stitcher.stitch(images, panorama);


			//if (Stitcher::OK == status){
			BBtime = getTickCount(); //check processing time
			cout << (BBtime - AAtime) / getTickFrequency() << " 초 걸렸습니다. \n"; //check processing time
			cout << "panorama.jpg 생성 완료\n";
			/*
			// 파노라마 사진 이름을 현재 시간으로 (나중엔 마지막 카메라의 날짜와 같아지게 수정 필요.)
			zeroMon = "0";
			zeroDay = "0";
			zeroHour = "0";
			zeroMin = "0";
			zeroSec = "0";
			year = to_string(pTime->tm_year + 1900);
			mon = to_string(pTime->tm_mon + 1);
			day = to_string(pTime->tm_mday);
			hour = to_string(pTime->tm_hour);
			minute = to_string(pTime->tm_min);
			sec = to_string(pTime->tm_sec);

			if (pTime->tm_hour <= 9){	// 0~9시일 경우, 00~09시로 바꿔줌
			zeroHour.append(hour);
			hour = zeroHour;
			}
			if (pTime->tm_min <= 9){
			zeroMin.append(minute);
			minute = zeroMin;
			}
			if (pTime->tm_sec <= 9){
			zeroSec.append(sec);
			sec = zeroSec;
			}
			if (pTime->tm_mon <= 9){
			zeroMon.append(mon);
			mon = zeroMon;
			}
			if (pTime->tm_mday <= 9){
			zeroDay.append(day);
			day = zeroDay;
			}


			year.append(mon);
			year.append(day);
			year.append(hour);
			year.append(minute);
			year.append(sec);
			year.append(fileExtension);


			imwrite("panorama.jpg", panorama);
			//rename("panorama.jpg", year.c_str());
			namedWindow(finalWinName);
			imshow(finalWinName, panorama);
			//}
			/*
			else{
			cout << "\nStitching failed!" << endl;
			system("pause");
			return -1;
			}
			*/
			return -1;
		}
		else if (x == 27)	// ESC키를 누르면 종료
			break;
		else {	// ESC, 1, 2 이외의 숫자를 입력할 경우
			cout << "잘못된 숫자를 입력하셨습니다." << endl;
		}
	}

	system("pause");
	cv::waitKey(0);

	return 0;
}