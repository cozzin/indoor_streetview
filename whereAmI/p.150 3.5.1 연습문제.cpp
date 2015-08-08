#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace cv;
using namespace std;

static char* SOURCE_IMAGE1 = "Source Image 1_original";

int main(){
	
	Mat image = imread("OH\\idx38_camera2.jpg");

	Mat out,grayImage,hsv,I202;
	
	vector<Mat> channel;
	cvtColor(image, hsv, CV_BGR2HSV);
	cvtColor(image, grayImage, CV_BGR2GRAY);

	split(hsv, channel);
	
	int x, y;
	float c;
	double tmp;
	//grayImage.copyTo(out);
	
	int histSize = 256;
	float range[] = { 0, 256 };
	const float* histRange = { range };
	bool uniform = true; 
	bool accumulate = false;
	Mat hist;
	calcHist(&grayImage, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);
	
	int darkness = 0; int brightness = 0;
	for (int i = 0; i < 256; i++){
		cout << "Value" << i << "=" << hist.at<float>(i) << endl;
		if (i < 127){
			darkness += hist.at<float>(i);
		}
		else if (i >= 127){
			brightness += hist.at<float>(i);
		}
	}
	cout <<endl << "dark = " << darkness << endl;
	cout << "bright = " << brightness << endl;
	cout << "dark/bright = " << (float) darkness / brightness *100 << endl;



	// gamma transformation
	//c = 4.0;
	//for (int i = 2; i < 3; i++){
	//	for (int y = 0; y < channel[i].rows; y++){
	//		for (int x = 0; x < channel[i].cols; x++){
	//			tmp = channel[i].at<uchar>(y, x) / 255.0; // r
	//			tmp = 1 * pow(tmp, c);
	//			channel[i].at<uchar>(y, x) = tmp * 255.0;
	//		}
	//	}
	//}
	//out.copyTo(I202);
	
	merge(channel, out);
	cvtColor(out, out, CV_HSV2BGR);

	namedWindow(SOURCE_IMAGE1, CV_WINDOW_AUTOSIZE);
	imshow(SOURCE_IMAGE1, image);
	imshow("v", channel[2]);
	imshow("out", out);
	
	waitKey(0);
	destroyAllWindows();
	return 0;
}