#include <opencv.hpp>
#include <opencv2\opencv.hpp>

#define arraySize 10000

using namespace cv;
using namespace std;

void hough_transform(Mat& im, Mat& orig, Mat gray, double* skew);
Mat preprocess1(Mat& im);
Mat preprocess2(Mat& im);
Mat rot(Mat& im, double thetaRad);
string src = "OH\\idx43_camera3.jpg";

void hough_transform(Mat& im, Mat& orig, Mat gray, double* skew)
{
	double max_r = sqrt(pow(.5*im.cols, 2) + pow(.5*im.rows, 2));
	int angleBins = 180;
	Mat acc = Mat::zeros(Size(2 * max_r, angleBins), CV_32SC1);
	int cenx = im.cols / 2;
	int ceny = im.rows / 2;
	Mat vert, hori;
	orig.copyTo(vert);
	orig.copyTo(hori);

	*skew = 0.;

	for (int x = 1; x < im.cols - 1; x++)
	{
		for (int y = 1; y < im.rows - 1; y++)
		{
			if (im.at<uchar>(y, x) == 255)		// edge 검출
			{
				for (int t = 0; t < angleBins; t++)
				{
					double r = (x - cenx)*cos((double)t / angleBins*CV_PI) + (y - ceny)*sin((double)t / angleBins*CV_PI);
					r += max_r;
					acc.at<int>(t, int(r))++;
				}
			}
		}
	}
	Mat thresh;
	normalize(acc, acc, 255, 0, NORM_MINMAX);
	convertScaleAbs(acc, acc);

	Point maxLoc, minLoc;
	minMaxLoc(acc, 0, 0, &minLoc, &maxLoc);	// 가장 뚜렷한 직선값

	cout << "minLoc(x, y) : " << minLoc.x << ", " << minLoc.y << endl;
	cout << "maxLoc(x, y) : " << maxLoc.x << ", " << maxLoc.y << endl;
	double theta = (double)maxLoc.y / angleBins*CV_PI;	// 영상 센터 좌표의 y축을 기준으로 한 각도
	cout << "theta : " << theta << endl;
	//double rho = maxLoc.x - max_r;		// 원점~영상 센터 좌표까지의 거리
	double rho = -maxLoc.x + max_r;		// 원점~영상 센터 좌표까지의 거리

	cout << "rho : " << rho << endl;
	printf("abs(sin(theta)) : %f\n", abs(sin(theta)));
	printf("abs(cos(theta)) : %f\n", abs(cos(theta)));

	cout << "abs(sin(theta)) : " << abs(sin(theta)) << endl;
	cout << "abs(cos(theta)) : " << abs(cos(theta)) << endl;
	Mat revisedImg, w_thres, b_thres, add, canny, rgb_canny, dst;
	vector<Vec4i> lines;
	Point center;

	double vertGrad[arraySize] = { 0.0 }, horiGrad[arraySize] = { 0.0 };
	double vertFrag[arraySize] = { 0.0 }, horiFrag[arraySize] = { 0.0 };
	int vertCount = 0, horiCount = 0;
	double vertGAVG = 0., vertFAVG = 0., horiGAVG = 0., horiFAVG = 0.;
	double vertSkewangle = 0., horiSkewangle = 0.;


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
		center = Point(I[2], I[3]) - Point(I[0], I[1]);
		double rad = atan2(center.x, center.y);
		double degree = (rad * 180) / CV_PI;
		//printf("degree:%f\n", degree);
		double a, b;

		if (degree >0 && degree < 10){	// 수직(y축)에 가까운 선 검출
			// 검출되는 모든 직선의 기울기를 구한후, 평균내기
			//cout << "detecting vertical" << endl;
			//기울기(a) 구하기
			//a = (I[3] - I[1]) / (I[2] - I[0]);
			//y절편(b) 구하기	y = ax + b
			b = I[1] - I[0] * degree;
			vertGrad[vertCount] = degree;
			//if (b < -225 && b > -355)
			vertFrag[vertCount++] = b;
			line(rgb_canny, Point(I[0], I[1]), Point(I[2], I[3]), Scalar(0, 0, 255), 1);

		}
		else if (degree>170 && degree < 180){
			degree = degree - 180;
			b = I[1] - I[0] * degree;
			if (degree >-10 && degree < 0) vertGrad[vertCount] = degree;
			//if (b < -225 && b > -355)
			vertFrag[vertCount++] = b;
			line(rgb_canny, Point(I[0], I[1]), Point(I[2], I[3]), Scalar(0, 0, 255), 1);

		}
		else if (degree > 80 && degree < 90) // 수평 (x축)에 가까운 선 검출
		{
			//cout << "detecting horizontal" << endl;
			//기울기(a) 구하기
			//a = (I[3] - I[1]) / (I[2] - I[0]);
			//y절편(b) 구하기	y = ax + b
			b = I[1] - I[0] * degree;
			horiGrad[horiCount] = degree;
			horiFrag[horiCount++] = b;
			line(rgb_canny, Point(I[0], I[1]), Point(I[2], I[3]), Scalar(0, 0, 255), 1);

		}
		else if (degree >90 && degree < 110){
			degree = degree - 180;
			b = I[1] - I[0] * degree;
			if (degree > -90 && degree < -80) horiGrad[horiCount] = degree;
			horiFrag[horiCount++] = b;
			line(rgb_canny, Point(I[0], I[1]), Point(I[2], I[3]), Scalar(0, 0, 255), 1);

		}
	}


	for (int j = 0; j < vertCount; j++)
	{
		vertGAVG += vertGrad[j];
		vertFAVG += vertFrag[j];
	}
	for (int j = 0; j < horiCount; j++)
	{
		horiGAVG += horiGrad[j];
		horiFAVG += horiFrag[j];
	}
	if (vertCount != 0.){
		vertGAVG /= vertCount;
		vertFAVG /= vertCount;
		
	}
	if (horiCount != 0.){
		horiGAVG /= horiCount;
		horiFAVG /= horiCount;
	}
	cout << "수직선 평균 기울기 : " << vertGAVG << endl;
	cout << "수평선 평균 기울기 : " << horiGAVG << endl;

	if (abs(sin(theta)) < 0.000001)//check vertical    <0.000001
	{
		cout << "complete vertical line" << endl;
		cout << abs(sin(theta)) << endl;
		cout << abs(cos(theta)) << endl;
		//when vertical, line equation becomes
		//x = rho
		double m = -cos(theta) / sin(theta);
		double b = rho / sin(theta) + im.rows / 2. - m*im.cols / 2.;
		Point2d p1 = Point2d(rho + im.cols / 2, 0);
		Point2d p2 = Point2d(rho + im.cols / 2, im.rows);
		line(orig, p1, p2, Scalar(0, 0, 255), 1);
		double skewangle;
		skewangle = p1.x - p2.x > 0 ? (atan2(p1.y - p2.y, p1.x - p2.x)*180. / CV_PI) : (atan2(p2.y - p1.y, p2.x - p1.x)*180. / CV_PI);
		*skew = skewangle;
		cout << "skew angle " << skewangle << endl;
		/*
		double m = -cos(theta) / sin(theta);
		Point2d p1 = Point2d(rho + im.cols / 2, 0);
		Point2d p2 = Point2d(rho + im.cols / 2, im.rows);
		line(orig, p1, p2, Scalar(0, 0, 255), 1);
		*skew = 90;
		cout << "skew angle " << " 90" << endl;
		*/
	}
	//if ((vertGAVG > 0 && vertGAVG < 20) || (vertGAVG>160 && vertGAVG < 180)){


	if ((vertGAVG > -10 && vertGAVG < 0) || (vertGAVG> 0 && vertGAVG < 10)){

		cout << "detecting vertical : 기울기 = " << vertGAVG << endl;
		Point2d p1 = Point2d(0, vertFAVG);
		Point2d p2 = Point2d(im.cols, im.cols*vertGAVG + vertFAVG);
		line(vert, p1, p2, Scalar(0, 0, 255), 1);

		vertSkewangle = p1.y - p2.y>0 ? (atan2(p1.y - p2.y, p1.x - p2.x)*180. / CV_PI) : (atan2(p2.y - p1.y, p2.x - p1.x)*180. / CV_PI);	// 세로선
		*skew = -vertGAVG;
		cout << "vertical skew angle " << vertSkewangle << endl;
		//imshow("vertical line", vert);
	}

	else{
		*skew = 0;
	}

	//if ((horiGAVG > 70 && horiGAVG < 90) || (horiGAVG >90 && horiGAVG < 110)) // 수평 (x축)에 가까운 선 검출
	if (((horiGAVG > 40 && horiGAVG < 90) || (horiGAVG >-90 && horiGAVG < -40)) || ((horiGAVG > 0 && horiGAVG < 20) || (horiGAVG >-20 && horiGAVG < 0))) // 수평 (x축)에 가까운 선 검출
	{

		if (horiGAVG > 0) horiGAVG = 90 - horiGAVG;
		else horiGAVG = 90 + horiGAVG;
		cout << "detecting horizontal : 기울기 = " << horiGAVG << endl;
		Point2d p1 = Point2d(0, horiFAVG);
		Point2d p2 = Point2d(im.cols, im.cols*horiGAVG + horiFAVG);
		line(hori, p1, p2, Scalar(0, 0, 255), 1);
		horiSkewangle = p1.x - p2.x > 0 ? (atan2(p1.y - p2.y, p1.x - p2.x)*180. / CV_PI) : (atan2(p2.y - p1.y, p2.x - p1.x)*180. / CV_PI);	// 세로선
		//*skew = -horiGAVG;
		horiSkewangle = 90 - horiSkewangle;
		horiSkewangle = abs(horiSkewangle) < horiGAVG ? horiSkewangle : horiGAVG;
		*skew = horiSkewangle;
		cout << "horizontal skew angle " << horiSkewangle << endl;
		//imshow("horizontal line", hori);

	}
	else{
		*skew = 0;
	}
	/*
	if (horiSkewangle >= 90 && horiSkewangle < 180)
	{
	//convert normal form back to slope intercept form
	//y = mx + b

	double skewangle;
	//skewangle = *skew - 90.0f;
	skewangle = 180.0f - horiSkewangle;
	*skew = skewangle;
	cout << "revised skew angle " << skewangle << endl;
	}
	else if (horiSkewangle >= 180 || vertSkewangle >= 180)
	{
	*skew = 0;
	}
	*/

	
	if (horiSkewangle != 0. && vertGAVG != 0. && (horiSkewangle <10. && horiSkewangle >1.)) {
		if (vertGAVG > -2. && vertGAVG < 2.)
			*skew = -vertGAVG;
		else
			*skew = abs(vertGAVG) > abs(horiSkewangle) ? -vertGAVG : horiSkewangle;
	}
	else if (horiSkewangle != 0. && vertGAVG != 0. && (horiSkewangle <1. && horiSkewangle >0.)) *skew = abs(vertGAVG) < abs(horiSkewangle) ? -vertGAVG : horiSkewangle;
	else if (horiSkewangle != 0. && vertGAVG != 0. && (horiSkewangle >10.)) *skew = abs(vertGAVG) > abs(horiGAVG) ? -vertGAVG : horiSkewangle;
	else if (horiSkewangle == 0)	*skew = -vertGAVG;
	else if (vertGAVG == 0) *skew = horiSkewangle;
	else if (horiSkewangle == 0 && vertGAVG == 0)	*skew = 0.;

	if ((vertSkewangle < 10. && vertSkewangle >0.) && (horiSkewangle <10. && horiSkewangle >0.))
		*skew = vertSkewangle > horiSkewangle ? -vertSkewangle : horiSkewangle;

	//*skew = vertGAVG;
	cout << "skew angle" << *skew << endl;

	imshow("orig", orig);
	imshow("canny", rgb_canny);
}

Mat preprocess1(Mat& im)
{
	Mat ret = Mat::zeros(im.size(), CV_32SC1);

	for (int x = 1; x < im.cols - 1; x++)
	{
		for (int y = 1; y < im.rows - 1; y++)
		{

			int gy = (im.at<uchar>(y - 1, x + 1) - im.at<uchar>(y - 1, x - 1))
				+ 2 * (im.at<uchar>(y, x + 1) - im.at<uchar>(y, x - 1))
				+ (im.at<uchar>(y + 1, x + 1) - im.at<uchar>(y + 1, x - 1));
			int gx = (im.at<uchar>(y + 1, x - 1) - im.at<uchar>(y - 1, x - 1))
				+ 2 * (im.at<uchar>(y + 1, x) - im.at<uchar>(y - 1, x))
				+ (im.at<uchar>(y + 1, x + 1) - im.at<uchar>(y - 1, x + 1));
			int g2 = (gy*gy + gx*gx);
			ret.at<int>(y, x) = g2;
		}
	}
	normalize(ret, ret, 255, 0, NORM_MINMAX);
	ret.convertTo(ret, CV_8UC1);
	threshold(ret, ret, 50, 255, THRESH_BINARY);
	return ret;
}

Mat preprocess2(Mat& im)
{
	// 1) assume white on black and does local thresholding
	// 2) only allow voting top is white and buttom is black(buttom text line)
	Mat thresh;
	//thresh=255-im;
	thresh = im.clone();
	adaptiveThreshold(thresh, thresh, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 15, -2);
	Mat ret = Mat::zeros(im.size(), CV_8UC1);
	for (int x = 1; x < thresh.cols - 1; x++)
	{
		for (int y = 1; y < thresh.rows - 1; y++)
		{
			bool toprowblack = thresh.at<uchar>(y - 1, x) == 0 || thresh.at<uchar>(y - 1, x - 1) == 0 || thresh.at<uchar>(y - 1, x + 1) == 0;
			bool belowrowblack = thresh.at<uchar>(y + 1, x) == 0 || thresh.at<uchar>(y + 1, x - 1) == 0 || thresh.at<uchar>(y + 1, x + 1) == 0;

			uchar pix = thresh.at<uchar>(y, x);
			if ((!toprowblack && pix == 255 && belowrowblack))
			{
				ret.at<uchar>(y, x) = 255;
			}
		}
	}
	return ret;
}
Mat rot(Mat& im, double thetaRad)
{
	cv::Mat rotated;
	double rskew = thetaRad* CV_PI / 180;
	double nw = abs(sin(thetaRad))*im.rows + abs(cos(thetaRad))*im.cols;
	double nh = abs(cos(thetaRad))*im.rows + abs(sin(thetaRad))*im.cols;
	cv::Mat rot_mat = cv::getRotationMatrix2D(Point2d(nw*.5, nh*.5), thetaRad * 180 / CV_PI, 1);
	Mat pos = Mat::zeros(Size(1, 3), CV_64FC1);
	pos.at<double>(0) = (nw - im.cols)*.5;
	pos.at<double>(1) = (nh - im.rows)*.5;
	Mat res = rot_mat*pos;
	rot_mat.at<double>(0, 2) += res.at<double>(0);
	rot_mat.at<double>(1, 2) += res.at<double>(1);
	cv::warpAffine(im, rotated, rot_mat, Size(nw, nh), cv::INTER_LANCZOS4);
	return rotated;
}

int main(int argc, char** argv)
{

	Mat im = imread(src);
	Mat gray;
	cvtColor(im, gray, CV_BGR2GRAY);

	Mat preprocessed = preprocess2(gray);
	//imshow("preprocessed2", preprocessed);
	double skew;
	hough_transform(preprocessed, im, gray, &skew);
	Mat rotated = rot(im, skew* CV_PI / 180);
	imshow("corrected", rotated);

	waitKey(0);
	return 0;
}