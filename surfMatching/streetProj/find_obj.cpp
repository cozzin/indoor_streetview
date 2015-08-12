#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include <opencv2\opencv.hpp>

#define MAX_TEXT 256

using namespace cv;
using namespace std;

const char* imageFileName = "C:\\Users\\OSH\\Documents\\indoor_streetview\\Mark\\Mark\\mark%d.png";
string src = "C:\\Users\\OSH\\Documents\\indoor_streetview\\Mark\\Mark\\idx44_camera1.jpg";
//KakaoTalk_20150812_162149779
//KakaoTalk_20150812_162149352
//1,1 / 14,0 / 16,0 / 17,2 / 21,2 / 39,2 / 39,3 / 41,3 / 44,1 / 

String getFileName(int i) {
	char buf[MAX_TEXT];
	sprintf(buf, imageFileName, i);

	String dst = buf;
	cout << buf << endl;
	return dst;
}

/** @function main */
int main(){
	Mat frame = imread(src);
	Mat find;
	double temp;
	double min;

	for (int i = 1; i<11; i++) {
		Mat object = imread(getFileName(i));
		if (object.empty()) { printf("error");  return -1; }
	
		//Detect the keypoints using SURF Detector
		int minHessian = 200;
		SurfFeatureDetector detector(minHessian);
		vector<KeyPoint> keypoints_object, keypoints_scene;
		detector.detect(object, keypoints_object);
		detector.detect(frame, keypoints_scene);

		//Calculate descriptors (feature vectors)
		SurfDescriptorExtractor extractor;
		Mat descriptors_object, descriptors_scene;
		extractor.compute(object, keypoints_object, descriptors_object);
		extractor.compute(frame, keypoints_scene, descriptors_scene);

		//Matching descriptor vectors using FLANN matcher
		FlannBasedMatcher matcher;
		vector< DMatch > matches;
		matcher.match(descriptors_object, descriptors_scene, matches);

		double max_dist = 0; double min_dist = 100;
		//Quick calculation of max and min distances between keypoints
		for (int j = 0; j < descriptors_object.rows; j++) {
			double dist = matches[j].distance;
			if (dist < min_dist) min_dist = dist;
			if (dist > max_dist) max_dist = dist;
		}
		printf("min_dist : %f, max_dist : %f\n", min_dist, max_dist);

	//	printf("-- Max dist : %f \n", max_dist);
	//	printf("-- Min dist : %f \n", min_dist);

		//Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
		vector< DMatch > good_matches;
		for (int j = 0; j < descriptors_object.rows; j++) {
			if (matches[j].distance < 3 * min_dist)   {
				good_matches.push_back(matches[j]);
			}
		}	

		Mat img_matches;
		drawMatches(object, keypoints_object, frame, keypoints_scene,
			good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

		temp = min_dist;
		if (i == 1) min = temp;

		if (min >= temp) {
			min = temp;
			//Localize the object
			vector<Point2f> obj;
			vector<Point2f> scene;
			printf("good_match_size : %d || good_match_cap : %d\n", good_matches.size(), good_matches.capacity());
			printf("good_matches.data() : %d\n", good_matches.data());
			for (int j = 0; j < good_matches.size(); j++) {
				//Get the keypoints from the good matches
				obj.push_back(keypoints_object[good_matches[j].queryIdx].pt);
				scene.push_back(keypoints_scene[good_matches[j].trainIdx].pt);
			}

			Mat H = findHomography(obj, scene, CV_RANSAC);
			//Finds a perspective transformation between two planes

			//Get the corners from the image_1 ( the object to be "detected" )
			vector<Point2f> obj_corners(4);
			obj_corners[0] = cvPoint(0, 0); obj_corners[1] = cvPoint(object.cols, 0);
			obj_corners[2] = cvPoint(object.cols, object.rows); obj_corners[3] = cvPoint(0, object.rows);
			vector<Point2f> scene_corners(4);

			perspectiveTransform(obj_corners, scene_corners, H);
			//Performs the perspective matrix transformation of vectors.

			//Draw lines between the corners (the mapped object in the scene - image_2 )
			line(img_matches, scene_corners[0] + Point2f(object.cols, 0), scene_corners[1] + Point2f(object.cols, 0), Scalar(0, 0, 255), 2);
			line(img_matches, scene_corners[1] + Point2f(object.cols, 0), scene_corners[2] + Point2f(object.cols, 0), Scalar(0, 255, 0), 2);
			line(img_matches, scene_corners[2] + Point2f(object.cols, 0), scene_corners[3] + Point2f(object.cols, 0), Scalar(255, 0, 0), 2);
			line(img_matches, scene_corners[3] + Point2f(object.cols, 0), scene_corners[0] + Point2f(object.cols, 0), Scalar(0, 255, 255), 2);

			img_matches.copyTo(find);
		}
		printf("min %f\n", min);

		imshow("MatchingResult", img_matches);
		int key = cvWaitKey(10);
		if (key == 27) {
			break;
		}
	}
	imshow("result", find);
	waitKey();
	system("pause");
	return 0;
}