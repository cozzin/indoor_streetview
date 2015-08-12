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
* 2개 벡터의 유클리드거리의 계산해서 반환
*
* @param[in] vec     벡터1의 배열
* @param[in] mvec	 벡터2의 배열
* @param[in] length  벡터의 길이
*
* @return 유클리드 거리
*/

double euclidDistance(float* vec1, float* vec2, int length) {
	double sum = 0.0;
	for (int i = 0; i < length; i++) {
		sum += (vec1[i] - vec2[i]) * (vec1[i] - vec2[i]);
	}
	return sqrt(sum);
}

/**
* 최근접점 탐색
*
* @param[in]   vec             특징 벡터
* @param[in]   laplacian       라플라시안
* @param[in]   keypoints       키포인트의 집합（집합의 중심으로부터 최근접점의 탐색)
* @param[in]   descriptors     특징 벡터의 집합
*
* @return 최근접점의 인덱스（발견되지 않을 때는 -1）
*/

int nearestNeighbor(float* vec, int laplacian, CvSeq* keypoints, CvSeq* descriptors) {
	int neighbor = -1;
	double minDist = 1e6;

	for (int i = 0; i < descriptors->total; i++) {
		CvSURFPoint* pt = (CvSURFPoint*)cvGetSeqElem(keypoints, i);
		// 라플라시안이 다른 키포인트는 무시
		if (laplacian != pt->laplacian) continue;
		float* v = (float*)cvGetSeqElem(descriptors, i);
		double d = euclidDistance(vec, v, DIM_VECTOR);
		// 보다 가까운 점이 있으면 옮겨 놓기
		if (d < minDist) {
			minDist = d;
			neighbor = i;
		}
	}

	// 최근접점에서도 거리가 임계값 이상이라면 무시한다.
	if (minDist < THRESHOLD) {
		return neighbor;
	}

	// 최근접점이 없는 경우
	return -1;
}

/**
* 영상1의 키포인트와 가까운 화상 2의 키포인트를 짝(pair)으로 해 반환
*
* @param[in]  keypoints1       영상1의 키포인트
* @param[in]  descriptors1     영상1의 특징벡터
* @param[in]  keypoints2       영상2의 키포인트
* @param[in]  descriptors2     영상2의 특징벡터

* @param[out] ptpairs          유사 키포인트 인덱스의 열(2개당 1짝)
*
* @return 없음
*/

void findPairs(CvSeq* keypoints1, CvSeq* descriptors1,
	CvSeq* keypoints2, CvSeq* descriptors2,
	vector<int>& ptpairs) {
	ptpairs.clear();
	// 영상 1의 각 키포인트에 관해서 최근접점을 검색
	for (int i = 0; i < descriptors1->total; i++) {
		CvSURFPoint* pt1 = (CvSURFPoint*)cvGetSeqElem(keypoints1, i);
		float* desc1 = (float*)cvGetSeqElem(descriptors1, i);
		// 최근접점의 검색
		int nn = nearestNeighbor(desc1, pt1->laplacian, keypoints2, descriptors2);
		// 최근접점이 있을 경우 영상 1의 인덱스와 영상 2의 인덱스를 차례로 등록
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

	// 영상은 그레이스케일로 로드
	IplImage* grayImage1 = cvLoadImage(filename1, CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* grayImage2 = cvLoadImage(filename2, CV_LOAD_IMAGE_GRAYSCALE);
	if (!grayImage1 || !grayImage2) {
		cerr << "cannot find image file" << endl;
		exit(-1);
	}

	// 결과의 표현을 위해 컬러영상도 로드
	IplImage* colorImage1 = cvLoadImage(filename1, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH);
	IplImage* colorImage2 = cvLoadImage(filename2, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH);

	// 매칭을 표현하기 위한 영상의 표현
	CvSize sz = cvSize(colorImage1->width + colorImage2->width, colorImage1->height + colorImage2->height);
	IplImage* matchingImage = cvCreateImage(sz, IPL_DEPTH_8U, 3);

	// 영상 1의 표현
	cvSetImageROI(matchingImage, cvRect(0, 0, colorImage1->width, colorImage1->height));
	cvCopy(colorImage1, matchingImage);
	// 영상 2의 표현
	cvSetImageROI(matchingImage, cvRect(colorImage1->width, colorImage1->height, colorImage2->width, colorImage2->height));
	cvCopy(colorImage2, matchingImage);
	cvResetImageROI(matchingImage);

	CvSeq *keypoints1 = 0, *descriptors1 = 0;
	CvSeq *keypoints2 = 0, *descriptors2 = 0;
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSURFParams params = cvSURFParams(500, 1);

	// SURF의 추출
	cvExtractSURF(grayImage1, 0, &keypoints1, &descriptors1, storage, params);
	cvExtractSURF(grayImage2, 0, &keypoints2, &descriptors2, storage, params);

	// 특징 벡터의 유사도가 높은 키포인트들을 선으로 연결
	vector<int> ptpairs;  // keypoints의 인덱스가 2개씩 짝이 되도록 저장
	findPairs(keypoints1, descriptors1, keypoints2, descriptors2, ptpairs);

	// 2개씩 짝을 맞추어 직선을 긋는다.
	for (int i = 0; i < (int)ptpairs.size(); i += 2) {
		CvSURFPoint* pt1 = (CvSURFPoint*)cvGetSeqElem(keypoints1, ptpairs[i]);     // 영상 1의 키포인트
		CvSURFPoint* pt2 = (CvSURFPoint*)cvGetSeqElem(keypoints2, ptpairs[i + 1]); // 영상 2의 키포인트
		CvPoint from = cvPointFrom32f(pt1->pt);
		CvPoint to = cvPoint(cvRound(colorImage1->width + pt2->pt.x), cvRound(colorImage1->height + pt2->pt.y));
		cvLine(matchingImage, from, to, cvScalar(0, 255, 255));
	}

	// 키포인트 매칭의 결과를 표현
	cvShowImage("Keypoint Matching", matchingImage);
	cvWaitKey(0);

	// 후처리 - 메모리 해제 등
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
