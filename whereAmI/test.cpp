#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

void rotate_90n(Mat &src, Mat &dst, int angle);

int main(){

	Mat srcimg = imread("1.jpg");
	Mat arrow = imread("arrow.jpg", 1);
	Size size = srcimg.size();
	Size original, half;
	Mat Rarrow, Larrow, Uarrow, Darrow;
	arrow.copyTo(Rarrow);
	arrow.copyTo(Larrow);
	arrow.copyTo(Uarrow);
	arrow.copyTo(Darrow);
	rotate_90n(Larrow, Larrow, -90);
	imshow("Larrow", Larrow);
	waitKey();
}


void rotate_90n(Mat &src, Mat &dst, int angle)
{
	dst.create(src.rows, src.cols, src.type());
	if (angle == 270 || angle == -90){
		// Rotate clockwise 270 degrees
		cv::flip(src.t(), dst, 0);
	}
	else if (angle == 180 || angle == -180){
		// Rotate clockwise 180 degrees
		cv::flip(src, dst, -1);
	}
	else if (angle == 90 || angle == -270){
		// Rotate clockwise 90 degrees
		cv::flip(src.t(), dst, 1);
	}
	else if (angle == 360 || angle == 0){
		if (src.data != dst.data){
			src.copyTo(dst);
		}
	}
}