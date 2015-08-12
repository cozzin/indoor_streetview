#include <opencv2\highgui\highgui.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\nonfree\nonfree.hpp>
#include <stdio.h>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	//if (argc != 3)	return -1;
	Mat frame = imread("C:\\Users\\OSH\\Documents\\indoor_streetview\\Mark\\idx17_camera2.jpg");		
	Mat img_matches;


	
	Mat logo = imread("C:\\Users\\OSH\\Documents\\indoor_streetview\\Mark\\mark5.png");
	if (logo.empty()) return -1;
	
		// detecting keypoints
		SurfFeatureDetector detector(5000);
		vector<KeyPoint> keypoints1, keypoints2;
		detector.detect(frame, keypoints1);
		detector.detect(logo, keypoints2);

		// computing descriptors
		SurfDescriptorExtractor extractor;
		Mat descriptors1, descriptors2;
		extractor.compute(frame, keypoints1, descriptors1);
		extractor.compute(logo, keypoints2, descriptors2);

		// matching descriptors
		BFMatcher matcher(NORM_L2);
		vector<DMatch> matches;
		matcher.match(descriptors1, descriptors2, matches);


		drawMatches(frame, keypoints1, logo, keypoints2, matches, img_matches);
		imshow("matching", img_matches);

		waitKey(10000);
	

	return 0;
}