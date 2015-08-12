#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include <opencv2\nonfree\nonfree.hpp>
#include <iostream>
#include <fstream>
#include <cv.h>

using namespace std;

const int DIM_VECTOR = 128;

/*
SURF ������ ���Ͽ� ���
@param[in]	filename			surf ������ �����ϴ� ���ϸ�
@param[in]	imageKeypoints		surf Ű����Ʈ ����
@param[in]	imageDescriptors	surf Ư¡ ����
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

	//Ű����Ʈ�� ���� Ư¡���� ������ ���
	fout << imageKeypoints->total << ' ' << DIM_VECTOR << endl;

	//Ű����Ʈ�� ������ Ư¡ ���͸� ���
	for (int i = 0; i < imageKeypoints->total; i++) {
		CvSURFPoint* point = (CvSURFPoint*)cvGetSeqElem(imageKeypoints, i);
		float * descriptor = (float*)cvGetSeqElem(imageDescriptors, i);

		//Ű����Ʈ ���� ���(x, y, ������, ���ö�þ�)
		fout << point->pt.x << ' ' << point->pt.y << ' ' << point->size << ' ' << point->laplacian << ' ';
		//Ư¡���� ���� ���
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

	//surf ������ ������ �׷��̷� ����
	IplImage* grayImage = cvLoadImage(imageFile, CV_LOAD_IMAGE_GRAYSCALE);
	if (!grayImage) {
		cerr << "cannot find image file:" << imageFile << endl;
		return -1;
	}

	//����� Ű����Ʈ�� ǥ���ϱ����� �÷���
	IplImage* colorImage = cvLoadImage(imageFile, CV_LOAD_IMAGE_COLOR);
	if (!colorImage) {
		cerr << "cannot find image file: " << imageFile << endl;
		return -1;
	}

	CvMemStorage * storage = cvCreateMemStorage(0);
	CvSeq* imageKeypoints = 0;
	CvSeq* imageDescriptors = 0;
	CvSURFParams params = cvSURFParams(500, 1);

	//�������κ��� surf Ư¡ ����
	cvExtractSURF(grayImage, 0, &imageKeypoints, &imageDescriptors, storage, params);
	cout << "Image Descriptors: " << imageDescriptors->total << endl;

	//surf ���� ���Ͽ� ���
	writeSURF(surfFile, imageKeypoints, imageDescriptors);

	//���� Ű����Ʈ ǥ��
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

	//��ó��
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

	//camera �ʱ�ȭ
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

		//grayscale ��ȯ
		IplImage* grayImage = cvCreateImage(cvGetSize(captureImage), 8, 1);
		cvCvtColor(captureImage, grayImage, CV_BGR2GRAY);

		//surf����
		cvExtractSURF(grayImage, 0, &imageKeypoints, &imageDescriptors, storage, params);

		//keypoint ����
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