#include "opencv2\highgui\highgui.hpp"
#include <opencv2\nonfree\nonfree.hpp>
#include <iostream>
#include <cv.h>
#include <vector>
#include <cmath>

using namespace std;

const int DIM_VECTOR = 128;
const double THRESHOLD = 0.3;
//	cv::initModule_nonfree();

/**
* 2�� ������ ��Ŭ����Ÿ��� ����ؼ� ��ȯ
*
* @param[in] vec     ����1�� �迭
* @param[in] mvec	 ����2�� �迭
* @param[in] length  ������ ����
*
* @return ��Ŭ���� �Ÿ�
*/

double euclidDistance(float* vec1, float* vec2, int length) {
	double sum = 0.0;
	for (int i = 0; i < length; i++) {
		sum += (vec1[i] - vec2[i]) * (vec1[i] - vec2[i]);
	}
	return sqrt(sum);
}

/**
* �ֱ����� Ž��
*
* @param[in]   vec             Ư¡ ����
* @param[in]   laplacian       ���ö�þ�
* @param[in]   keypoints       Ű����Ʈ�� ���գ������� �߽����κ��� �ֱ������� Ž��)
* @param[in]   descriptors     Ư¡ ������ ����
*
* @return �ֱ������� �ε������߰ߵ��� ���� ���� -1��
*/

int nearestNeighbor(float* vec, int laplacian, CvSeq* keypoints, CvSeq* descriptors) {
	int neighbor = -1;
	double minDist = 1e6;

	for (int i = 0; i < descriptors->total; i++) {
		CvSURFPoint* pt = (CvSURFPoint*)cvGetSeqElem(keypoints, i);
		// ���ö�þ��� �ٸ� Ű����Ʈ�� ����
		if (laplacian != pt->laplacian) continue;
		float* v = (float*)cvGetSeqElem(descriptors, i);
		double d = euclidDistance(vec, v, DIM_VECTOR);
		// ���� ����� ���� ������ �Ű� ����
		if (d < minDist) {
			minDist = d;
			neighbor = i;
		}
	}

	// �ֱ����������� �Ÿ��� �Ӱ谪 �̻��̶�� �����Ѵ�.
	if (minDist < THRESHOLD) {
		return neighbor;
	}

	// �ֱ������� ���� ���
	return -1;
}

/**
* ����1�� Ű����Ʈ�� ����� ȭ�� 2�� Ű����Ʈ�� ¦(pair)���� �� ��ȯ
*
* @param[in]  keypoints1       ����1�� Ű����Ʈ
* @param[in]  descriptors1     ����1�� Ư¡����
* @param[in]  keypoints2       ����2�� Ű����Ʈ
* @param[in]  descriptors2     ����2�� Ư¡����

* @param[out] ptpairs          ���� Ű����Ʈ �ε����� ��(2���� 1¦)
*
* @return ����
*/

void findPairs(CvSeq* keypoints1, CvSeq* descriptors1,
	CvSeq* keypoints2, CvSeq* descriptors2,
	vector<int>& ptpairs) {
	ptpairs.clear();
	// ���� 1�� �� Ű����Ʈ�� ���ؼ� �ֱ������� �˻�
	for (int i = 0; i < descriptors1->total; i++) {
		CvSURFPoint* pt1 = (CvSURFPoint*)cvGetSeqElem(keypoints1, i);
		float* desc1 = (float*)cvGetSeqElem(descriptors1, i);
		// �ֱ������� �˻�
		int nn = nearestNeighbor(desc1, pt1->laplacian, keypoints2, descriptors2);
		// �ֱ������� ���� ��� ���� 1�� �ε����� ���� 2�� �ε����� ���ʷ� ���
		if (nn >= 0) {
			ptpairs.push_back(i);
			ptpairs.push_back(nn);
		}
	}
}

int main(int argc, char** argv) {
	cv::initModule_nonfree();
	const char* filename1 = argc == 3 ? argv[1] : "C:\\Users\\OSH\\Documents\\indoor_streetview\\Mark\\mark4.png";
	const char* filename2 = argc == 3 ? argv[2] : "logo.png";

	cvNamedWindow("Keypoint Matching");

	// ������ �׷��̽����Ϸ� �ε�
	IplImage* grayImage1 = cvLoadImage(filename1, CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* grayImage2 = cvLoadImage(filename2, CV_LOAD_IMAGE_GRAYSCALE);
	if (!grayImage1 || !grayImage2) {
		cerr << "cannot find image file" << endl;
		exit(-1);
	}

	// ����� ǥ���� ���� �÷����� �ε�
	IplImage* colorImage1 = cvLoadImage(filename1, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH);
	IplImage* colorImage2 = cvLoadImage(filename2, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH);

	// ��Ī�� ǥ���ϱ� ���� ������ ǥ��
	CvSize sz = cvSize(colorImage1->width + colorImage2->width, colorImage1->height + colorImage2->height);
	IplImage* matchingImage = cvCreateImage(sz, IPL_DEPTH_8U, 3);

	// ���� 1�� ǥ��
	cvSetImageROI(matchingImage, cvRect(0, 0, colorImage1->width, colorImage1->height));
	cvCopy(colorImage1, matchingImage);
	// ���� 2�� ǥ��
	cvSetImageROI(matchingImage, cvRect(colorImage1->width, colorImage1->height, colorImage2->width, colorImage2->height));
	cvCopy(colorImage2, matchingImage);
	cvResetImageROI(matchingImage);

	CvSeq *keypoints1 = 0, *descriptors1 = 0;
	CvSeq *keypoints2 = 0, *descriptors2 = 0;
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSURFParams params = cvSURFParams(500, 1);

	// SURF�� ����
	cvExtractSURF(grayImage1, 0, &keypoints1, &descriptors1, storage, params);
	cvExtractSURF(grayImage2, 0, &keypoints2, &descriptors2, storage, params);

	// Ư¡ ������ ���絵�� ���� Ű����Ʈ���� ������ ����
	vector<int> ptpairs;  // keypoints�� �ε����� 2���� ¦�� �ǵ��� ����
	findPairs(keypoints1, descriptors1, keypoints2, descriptors2, ptpairs);

	// 2���� ¦�� ���߾� ������ �ߴ´�.
	for (int i = 0; i < (int)ptpairs.size(); i += 2) {
		CvSURFPoint* pt1 = (CvSURFPoint*)cvGetSeqElem(keypoints1, ptpairs[i]);     // ���� 1�� Ű����Ʈ
		CvSURFPoint* pt2 = (CvSURFPoint*)cvGetSeqElem(keypoints2, ptpairs[i + 1]); // ���� 2�� Ű����Ʈ
		CvPoint from = cvPointFrom32f(pt1->pt);
		CvPoint to = cvPoint(cvRound(colorImage1->width + pt2->pt.x), cvRound(colorImage1->height + pt2->pt.y));
		cvLine(matchingImage, from, to, cvScalar(0, 255, 255));
	}

	// Ű����Ʈ ��Ī�� ����� ǥ��
	cvShowImage("Keypoint Matching", matchingImage);
	cvWaitKey(0);

	// ��ó�� - �޸� ���� ��
	cvReleaseImage(&grayImage1);
	cvReleaseImage(&grayImage2);
	cvReleaseImage(&colorImage1);
	cvReleaseImage(&colorImage2);
	cvClearSeq(keypoints1);
	cvClearSeq(descriptors1);
	cvClearSeq(keypoints2);
	cvClearSeq(descriptors2);
	cvReleaseMemStorage(&storage);

	return 0;
}
