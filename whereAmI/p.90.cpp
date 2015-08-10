#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

void rotate_90n(Mat &src, Mat &dst, int angle);

int main(){
	
	Mat srcimg = imread("1.jpg");
	Mat arrow = imread("arrow.jpg", 1);
	Size size = srcimg.size();
	Size arrowSize, half;
	Mat Rarrow, Larrow, Uarrow, Darrow;
	
	
	/*
	//vector<Mat> channels(4);
	inRange(arrow, Scalar::all(255), Scalar::all(255), alpha);
	//inRange(arrow, Scalar(250,0,0), Scalar(255,0,0), alpha);
	bitwise_not(alpha, alpha);

	Mat bgr[3], sbgr[3];
	split(arrow, bgr);
	split(srcimg, sbgr);
	//Size qurter(size.width / 2, size.height / 2);
	Mat tmp[4] = { bgr[0], bgr[1], bgr[2], alpha };
	Mat dst;
	merge(tmp, 4, dst);
	//imwrite("arrow.jpg", dst);
	tmp = { sbgr[0], sbgr[1], sbgr[2], alpha };
	
	imshow("dst", dst);
	*/

	// 검은색 배경 투명하게 만들기
	Mat dst;
	Mat tmp, ttmp, alpha, salpha;

	cvtColor(arrow, tmp, CV_BGR2GRAY);
	cvtColor(srcimg, ttmp, CV_BGR2GRAY);
	threshold(tmp, alpha, 100, 255, THRESH_BINARY);
	threshold(ttmp, salpha, 100, 255, THRESH_BINARY);

	Mat rgb[3], srgb[3];
	split(arrow, rgb);
	split(srcimg, srgb);

	Mat rgba[4] = { rgb[0], rgb[1], rgb[2], alpha };
	Mat srgba[4] = { srgb[0], srgb[1], srgb[2], salpha };
	merge(rgba, 4, arrow);
	merge(srgba, 4, srcimg);
	//imwrite("transparent.png", arrow);
	
	arrow.copyTo(Rarrow);
	arrow.copyTo(Uarrow);
	rotate_90n(Uarrow, Uarrow, -90);
	arrow.copyTo(Larrow);
	rotate_90n(Larrow, Larrow, -180);
	arrow.copyTo(Darrow);
	rotate_90n(Darrow, Darrow, 90);
	/*
	Mat dst;
	arrow.copyTo(dst);
	dst.convertTo(dst, CV_8UC4);
	imshow("dst",dst);
	*/
	//Mat targetMat(arrow.size().width, arrow.size().height, CV_8UC4);
	/*
	targetMat = new Mat(targetSize, CV_8UC4, colorScalar);
	Scalar colorScalar = new Scalar(125, 125, 200, 0.6);
	*/
	half.width = size.width / 2;
	half.height = size.height / 2;
	arrowSize.width = arrow.size().width;
	arrowSize.height = arrow.size().height;

	Rect Right(half.width, size.height / 2, arrow.size().width, arrow.size().height);
	Rect Up(half.width - arrow.size().width, size.height / 2 - arrow.size().height, arrow.size().width, arrow.size().height);
	Rect Left(half.width - 2 * arrow.size().width, size.height / 2, arrow.size().width, arrow.size().height);
	Rect Down(half.width - arrow.size().width, size.height / 2 + arrow.size().height, arrow.size().width, arrow.size().height);
	//Rect roiRect(30, 30, 20, 20);
	//Rect roiRight(512 - 512 / 4, 512 - 512 / 4, qurter.width / 2, qurter.height / 2);
	Mat roiR(srcimg, Right);
	Mat roiU(srcimg, Up);
	Mat roiL(srcimg, Left);
	Mat roiD(srcimg, Down);
//	Mat roiR(arrow, roiRight);

	//flip(roiR, roiR, 1);
	
	//resize(result, result, srcimg.size());
	

	//original.width = 512;
	//original.height = 512;
	//half.width = size.width / 2;
	//half.height = size.height / 2;
	

	//rectangle(srcimg, Point(0, 0), Point(512 / 2, 512 / 2), Scalar(0, 255, 255), 1);
	//rectangle(srcimg, Point(512 - 512 / 4, 512- 512 / 4), Point(512, 512), Scalar::all(255),1);
	//originalSize.copyTo(halfSize);
	//flip(roi, roi, 0);
	//addWeighted(srciㅣmg, 0.0, arrow, 1.0, 0.0, srcimg);
	addWeighted(roiR, 0.4, Rarrow, 0.6, 0.0, roiR);
	//rotate_90n(Larrow, Larrow, -90);
	addWeighted(roiU, 0.4, Uarrow, 0.6, 0.0, roiU);
	addWeighted(roiL, 0.4, Larrow, 0.6, 0.0, roiL);
	addWeighted(roiD, 0.4, Darrow, 0.6, 0.0, roiD);

	namedWindow("pic", WINDOW_AUTOSIZE);
	//namedWindow("half", WINDOW_AUTOSIZE);
	//imshow("roi", roi);
	//imshow("arrow", arrow);
	imshow("pic", srcimg);
	//imshow("half", src);
	/*
	Mat dstImage(512, 512, CV_8UC3, Scalar(255, 255, 255));

	rectangle(dstImage, Point(0, 0), Point(512, 512), Scalar(255, 0, 0), 2);
	rectangle(dstImage, Point(0, 0), Point(200, 200), Scalar(0, 255, 0), 2);
	//dstImage.adjustROI(-100, -100, -100, -100);

	imshow("Draw", dstImage);
	*/
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