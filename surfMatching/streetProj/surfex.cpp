#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include <opencv2\nonfree\nonfree.hpp>
#include <iostream>
#include <fstream>
#include <cv.h>

using namespace std;

const int DIM_VECTOR = 128;

/*
SURF 정보를 파일에 출력
@param[in]	filename			surf 정보를 저장하는 파일명
@param[in]	imageKeypoints		surf 키포인트 정보
@param[in]	imageDescriptors	surf 특징 정보
@return
*/

#if 1
void writeSURF(const char* filename, CvSeq* imageKeypoints, CvSeq* imageDescriptors) {
	fstream fout;
	fout.open(filename, ios::out);
	if (!fout.is_open()) {
		cerr << "cannot open file : " << filename << endl;
		return;
	}

	//키포인트의 수와 특징량의 차원수 기록
	fout << imageKeypoints->total << ' ' << DIM_VECTOR << endl;

	//키포인트의 정보와 특징 벡터를 기록
	for (int i = 0; i < imageKeypoints->total; i++) {
		CvSURFPoint* point = (CvSURFPoint*)cvGetSeqElem(imageKeypoints, i);
		float * descriptor = (float*)cvGetSeqElem(imageDescriptors, i);

		//키포인트 정보 기록(x, y, 사이즈, 라플라시안)
		fout << point->pt.x << ' ' << point->pt.y << ' ' << point->size << ' ' << point->laplacian << ' ';
		//특징벡터 정보 기록
		for (int j = 0; j < DIM_VECTOR; j++) {
			fout << descriptor[j] << ' ';
		}
		fout << endl;
	}
	fout.close();
}


int main(int argc, char** argv) {
	cv::initModule_nonfree();

	const char* imageFile = argc == 3 ? argv[1] : "C:\\Users\\OSH\\Documents\\indoor_streetview\\Mark\\mark4.png";
	const char* surfFile = argc == 3 ? argv[2] : "lena.surf";

	//surf 추출할 영상을 그레이로 읽음
	IplImage* grayImage = cvLoadImage(imageFile, CV_LOAD_IMAGE_GRAYSCALE);
	if (!grayImage) {
		cerr << "cannot find image file:" << imageFile << endl;
		return -1;
	}

	//결과에 키포인트를 표현하기위해 컬러로
	IplImage* colorImage = cvLoadImage(imageFile, CV_LOAD_IMAGE_COLOR);
	if (!colorImage) {
		cerr << "cannot find image file: " << imageFile << endl;
		return -1;
	}

	CvMemStorage * storage = cvCreateMemStorage(0);
	CvSeq* imageKeypoints = 0;
	CvSeq* imageDescriptors = 0;
	CvSURFParams params = cvSURFParams(500, 1);

	//영상으로부터 surf 특징 추출
	cvExtractSURF(grayImage, 0, &imageKeypoints, &imageDescriptors, storage, params);
	cout << "Image Descriptors: " << imageDescriptors->total << endl;

	//surf 정보 파일에 기록
	writeSURF(surfFile, imageKeypoints, imageDescriptors);

	//영상에 키포인트 표현
	for (int i = 0; i < imageKeypoints->total; i++){
		CvSURFPoint *point = (CvSURFPoint*)cvGetSeqElem(imageKeypoints, i);
		CvPoint center;
		int radius;
		center.x = cvRound(point->pt.x);
		center.y = cvRound(point->pt.y);
		radius = cvRound(point->size * 1.2 / 9.0 * 2.0);
		cvCircle(colorImage, center, radius, cvScalar(0, 255, 255), 1, 8, 0);
	}

	cvNamedWindow("surf");
	cvShowImage("surf", colorImage);
	cvWaitKey(0);

	//후처리
	cvReleaseImage(&grayImage);
	cvReleaseImage(&colorImage);
	cvClearSeq(imageKeypoints);
	cvClearSeq(imageDescriptors);
	cvReleaseMemStorage(&storage);
	cvDestroyAllWindows();

	return 0;
}
#endif

#if 0
int main(int argc, char**argv) {
	cv::initModule_nonfree();
	CvCapture* capture;

	//camera 초기화
	if ((capture = cvCreateCameraCapture(0)) == NULL) {
		cerr << "cannot find camera" << endl;
		return -1;
	}

	cvNamedWindow("surf");

	IplImage* captureImage = cvQueryFrame(capture);
	while (true) {
		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* imageKeypoints = 0;
		CvSeq* imageDescriptors = 0;
		CvSURFParams params = cvSURFParams(500, 1);

		captureImage = cvQueryFrame(capture);

		//grayscale 변환
		IplImage* grayImage = cvCreateImage(cvGetSize(captureImage), 8, 1);
		cvCvtColor(captureImage, grayImage, CV_BGR2GRAY);

		//surf추출
		cvExtractSURF(grayImage, 0, &imageKeypoints, &imageDescriptors, storage, params);

		//keypoint 추출
		for (int i = 0; i < imageKeypoints->total; i++){
			CvSURFPoint* point = (CvSURFPoint*)cvGetSeqElem(imageKeypoints, i);
			CvPoint center;
			int radius;
			center.x = cvRound(point->pt.x);
			center.y = cvRound(point->pt.y);
			radius = cvRound(point->size * 0.4);
			//cvCircle(captureImage, center, 2, cvScalar(0, 255, 255), CV_FILLED);
			cvCircle(captureImage, center, 2, cvScalar(0, 255, 255), 1, 8, 0);
			//cvCircle()
		}

		cvShowImage("surf", captureImage);

		cvReleaseImage(&grayImage);
		cvClearSeq(imageKeypoints);
		cvClearSeq(imageDescriptors);
		cvReleaseMemStorage(&storage);

		int key = cvWaitKey(30);
		if (key == 27) {
			break;
		}

	}

	cvReleaseCapture(&capture);
	cvDestroyAllWindows();
	
	return 0;
}
#endif