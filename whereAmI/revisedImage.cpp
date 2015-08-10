#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

Mat revisedImage(Mat streetImage);
void rotateImage(Mat image);

int main(){
	Mat src = imread("OH\\idx0_camera0.jpg");


	revisedImage(src);
	//rotateImage(src);
	waitKey();
}


Mat revisedImage(Mat Image){
	Mat revisedImg, gray, w_thres, b_thres, add, canny, rgb_canny, dst;
	vector<Vec4i> lines;
	Point center;
	double grad;

	CvPoint2D32f centralPoint = CvPoint2D32f(Image.size().width / 2, Image.size().height / 2); // 회전 기준점
	CvMat* rotationMatrix = cvCreateMat(2, 3, CV_32FC1); // 회전 기준

	// 보정해야할 만큼 기울어져 있는지 확인 (세로선들을 검출한 후, 기울기 측정)
	cvtColor(Image, gray, CV_BGR2GRAY);
	// 선 굵기가 얇아서 짧게 나온다.
	threshold(gray, w_thres, 120, 255, 0);
	inRange(gray, Scalar::all(0), Scalar::all(20), b_thres);

	add = b_thres + w_thres;
	Canny(add, canny, 50, 150, 3);

	cvtColor(canny, rgb_canny, CV_GRAY2BGR);


	HoughLinesP(canny, lines, 1, CV_PI / 180, 30, 10, 30);
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4d I = lines[i];
		//I[0] = 0.0f;
		//I[1] = 0.0f;
		//I[2] = -1.0f;
		//I[3] = 1.0f;

		center = Point(I[2], I[3]) - Point(I[0], I[1]);
		double rad = atan2(center.x, center.y);
		double degree = (rad * 180) / CV_PI;
		
		/*
		double degree = 0.0;
		degree = (I[3] - I[1]) / (I[2] - I[0]);
		*/
		if (degree >0 && degree < 10){
			line(rgb_canny, Point(I[0], I[1]), Point(I[2], I[3]), Scalar(0, 0, 255), 1);
			printf("vertical degree:%f\n", degree);
			//기울기(grad) 구하기
			grad = (I[3] - I[1]) / (I[2] - I[0]);
			/*
			//y절편(b) 구하기	y = ax + b
			b = I[1] - I[0] * a;
			*/
		}
		else if (degree>170 && degree < 180){
			degree = 180 - degree;
			line(rgb_canny, Point(I[0], I[1]), Point(I[2], I[3]), Scalar(0, 0, 255), 1);
			printf("vertical degree:%f\n", degree);
			
		}
		else if (degree > 70 && degree < 90) // 수평 (x축)에 가까운 선 검출
		{
			printf("horizontal degree:%f\n", degree);
			line(rgb_canny, Point(I[0], I[1]), Point(I[2], I[3]), Scalar(0, 0, 255), 1);
			
		}
		else if (degree >90 && degree < 110)
		{
			degree = degree - 180;
			printf("horizontal degree:%f\n", degree);
			line(rgb_canny, Point(I[0], I[1]), Point(I[2], I[3]), Scalar(0, 0, 255), 1);

		}

		imshow("Hough", rgb_canny);
		waitKey(0);
	}

	
	
	//rotateImage(Image, dst, )

	imshow("src", Image);
	//imshow("gray", gray);
	//imshow("Extracting white", w_thres);
	//imshow("Extracting black", b_thres);
	imshow("Hough", rgb_canny);
	//waitKey();

	// 사진 보정 (세로선들만 검출하고 y축과 평행하도록)

	// 보정할 필요가 없을 경우 (이미 y축과 충분히 평행할 경우) 사진 그대로.
	return revisedImg;

}

	
void rotateImage(Mat image){
	int height = image.rows;								// ex) height = 1200
	float belowCornerY = static_cast<float>(height - 1);	// 1199 = 1200 - 1
	Point2f ptSrc[4];
	Point2f ptDst[4];
	/*	원래 정보
	ptSrc[0] = Point2f(613.0f, 0.0f);	
	ptSrc[1] = Point2f(596.0f, belowCornerY);	
	ptSrc[2] = Point2f(996.0f, belowCornerY);	
	ptSrc[3] = Point2f(1013.0, 0.0f);
	
	/* Mat src = imread("OH\\idx39_camera3.jpg"); 의 경우
	ptSrc[0] = Point2f(595.0f, 0.0f);
	ptSrc[1] = Point2f(596.0f, belowCornerY);
	ptSrc[2] = Point2f(996.0f, belowCornerY);
	ptSrc[3] = Point2f(1013.0, 0.0f);
	
	/*	Mat src = imread("OH\\idx35_camera3.jpg");
	ptSrc[0] = Point2f(593.0f, 0.0f);
	ptSrc[1] = Point2f(600.0f, belowCornerY);
	ptSrc[2] = Point2f(995.0f, belowCornerY);
	ptSrc[3] = Point2f(1013.0f, 0.0f);
	*/
	Size s = image.size();

	ptSrc[0] = Point2f(0.0f, s.height);		// lower left
	ptSrc[1] = Point2f(0.0f, 0.0f);			// upper left
	ptSrc[2] = Point2f(s.width, 0.0f);		// upper right
	ptSrc[3] = Point2f(s.width, s.height);	// lower right

	const float chipWidth = ptSrc[3].x - ptSrc[0].x;
	const float startPointx = (ptSrc[0].x + ptSrc[1].x) / 2;	// start point of dst
	const float rightPointx = startPointx + chipWidth;			// end point of dst

	ptDst[0] = Point2f(startPointx, 0.0f);
	ptDst[1] = Point2f(startPointx, belowCornerY);
	ptDst[2] = Point2f(rightPointx, belowCornerY);
	ptDst[3] = Point2f(rightPointx, 0.0f);
	/*
	// Compute perspective transformation matrix
	Mat warpMat = Mat::eye(3, 3, CV_32F);
	warpMat = getPerspectiveTransform(ptSrc, ptDst);
	
	// Apply perspective matrix to image
	Mat result;
	result.create(image.size(), image.type());
	warpPerspective(image, result, warpMat, image.size(), INTER_CUBIC);
	*/
	Mat result;
	Mat transform = getPerspectiveTransform(ptSrc, ptDst);

	warpPerspective(image, result, transform, s, INTER_CUBIC);

	imshow("Original", image);
	imshow("result", result);
	
}
