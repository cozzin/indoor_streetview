#include <iostream>
#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include <math.h>

using namespace cv;
using namespace std;

void detectLines(Mat in_src);
void colorDiversity(Mat in_src);
void findVanishingPoint(Mat in_src);
void findVanishingPoint_roi(Mat in_src);

double F_ang(double x1, double y1, double x2, double y2);

int main(int argc, char**argv){

	system("cls");
	Mat src;
	src = imread("�����̸�.jpg");
	if (!src.data){ // ���� �ҷ����� ����
		return -1;
	}

	detectLines(src); // �������� ����
	colorDiversity(src); // ���� �پ缺 �ľ�
	findVanishingPoint(src);
	//findVanishingPoint_roi(src);

	waitKey(0);
	destroyAllWindows();
	return 0;
}

//���� ���� ����
void detectLines(Mat in_src){

	cout << endl << "##### ���� ���� ���� �� #####" << endl;
	Mat dst, color_dst;
	Canny(in_src, dst, 50, 250, 3);
	cvtColor(dst, color_dst, COLOR_GRAY2BGR);

	vector<Vec4i> lines;
	HoughLinesP(dst, lines, 1, CV_PI / 180, 80, 30, 10);//default

	for (size_t i = 0; i < lines.size(); i++){
		line(color_dst, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 1, 8);
	}

	if (lines.size() == 0){
		cout << "FAIL : ���� ������ �����ϴ�" << endl;
	}
	else{
		cout << "PASS : ���� ������ " << lines.size() << "�� �����մϴ�" << endl;
	}



	namedWindow("Detected LInes", CV_WINDOW_AUTOSIZE);
	imshow("Detected LInes", color_dst);
}

//���� �پ缺 �ľ�
void colorDiversity(Mat in_src){
	cout << endl << "###### ���� �پ缺 �� ######" << endl;
	//HSV���� ������ �پ缺�� �ľ��ϱ����� hue ���� �̿���
	//HSV�� ��ȯ�� �� ä�κ��� ����
	Mat hsv_image;
	vector<Mat> hsv_channels(3);
	cvtColor(in_src, hsv_image, CV_BGR2HSV);
	split(hsv_image, hsv_channels);

	//histogram ��� �κ�
	int histSize = 256; //bin�� ���� ����
	float range[] = { 0, 256 }; // ������ ����
	const float* histRange = { range }; // ���ϴ��� ������ ��
	bool uniform = true; bool accumulate = false;
	Mat h_hist;

	calcHist(&hsv_channels[2], 1, 0, Mat(), h_hist, 1, &histSize, &histRange, uniform, accumulate);

	double addHist = 0;
	for (int i = 1; i < histSize; i++){
		//cout << i << " = " << h_hist.at<float>(i) << endl;
		addHist += h_hist.at<float>(i);
	}
	float hist_equal = (float)addHist / histSize;
	cout << "��� = " << hist_equal << endl;

	double hist_square_Sum = 0;
	for (int i = 1; i < histSize; i++){
		hist_square_Sum += pow((float)(h_hist.at<float>(i) -hist_equal), 2);
	}
	double variance = hist_square_Sum / histSize;
	double standardDeviation = sqrt(variance);
	printf("�л� = %f\n", variance);
	printf("ǥ������ = %f\n", standardDeviation);

	if (standardDeviation >= 2500){
		cout << "FAIL : ���� �پ��� : �����ο�" << endl;
	}
	else if ((standardDeviation<2500) && (standardDeviation >= 1200)){
		cout << "PASS : ���� �پ��� : ����" << endl;
	}
	else if (standardDeviation<1200){
		cout << "PASS : ���� �پ��� : ��ä�ο�" << endl;
	}

	//imshow("HSV_H_CHANNEL", hsv_channels[0]);
	//imshow("HSV_S_CHANNEL", hsv_channels[1]);
	//imshow("HSV_V_CHANNEL", hsv_channels[2]);
}

void findVanishingPoint(Mat in_src){
	cout << endl << "########## �ҽ��� �ľ� ########" << endl;

	Mat dst, color_dst, bgr_dst;
	bgr_dst = in_src.clone();
	Canny(in_src, dst, 40, 100, 3);
	cvtColor(dst, color_dst, COLOR_GRAY2BGR);

	//Rect roi(0, dst.rows / 2, dst.cols, dst.rows / 2);
	//namedWindow("roi", CV_WINDOW_AUTOSIZE);
	//imshow("roi", dst(roi));

	vector<vector<double>> linearEquationCollection_left;
	vector<vector<double>> linearEquationCollection_right;

	vector<Vec4i> lines;
	HoughLinesP(dst, lines, 1, CV_PI / 180, 90, 20, 10);
	for (size_t i = 0; i < lines.size(); i++)
	{
		Point pt1(lines[i][0], lines[i][1]);
		Point pt2(lines[i][2], lines[i][3]);

		double pts_angle = (atan2(pt1.y - pt2.y, pt1.x - pt2.x) * 180) / CV_PI;

		if (((pts_angle>120) && (pts_angle < 150)) || ((pts_angle<-30) && (pts_angle>-60))){

			line(color_dst, pt1, pt2, Scalar(0, 255, 0), 1, 8);

			double linear_slope = (double)(pt1.y - pt2.y) / (pt1.x - pt2.x); // ����
			double linear_intercept = -linear_slope * pt1.x + pt1.y; // ����

			vector<double> linearEquation_left;
			linearEquation_left.push_back(linear_slope);
			linearEquation_left.push_back(linear_intercept);
			linearEquationCollection_left.push_back(linearEquation_left);
		}
		else if (((pts_angle > 30) && (pts_angle < 60)) || ((pts_angle<-120) && (pts_angle>-150))){

			line(color_dst, pt1, pt2, Scalar(255, 0, 0), 1, 8);
			double linear_slope = (double)(pt1.y - pt2.y) / (pt1.x - pt2.x); // ����
			double linear_intercept = -linear_slope * pt1.x + pt1.y; // ����

			vector<double> linearEquation_right;
			linearEquation_right.push_back(linear_slope);
			linearEquation_right.push_back(linear_intercept);
			linearEquationCollection_right.push_back(linearEquation_right);
		}
	}

	//����� ���� ������ ������ �������� ���� �ľ�
	vector<Point> intersectionCollection;
	for (size_t j = 0; j < linearEquationCollection_left.size(); j++){

		double slope_a = linearEquationCollection_left[j][0];
		double intercept_a = linearEquationCollection_left[j][1];

		for (size_t k = 0; k < linearEquationCollection_right.size(); k++){

			double slope_b = linearEquationCollection_right[k][0];
			double intercept_b = linearEquationCollection_right[k][1];


			double intersection_x = (-intercept_a + intercept_b) / (slope_a - slope_b);
			double intersection_y = slope_a*intersection_x + intercept_a;

			Point intersection(intersection_x, intersection_y);
			intersectionCollection.push_back(intersection);
		}
	}

	double sum_x = 0;
	double sum_y = 0;
	for (size_t i = 0; i < intersectionCollection.size(); i++){
		sum_x += intersectionCollection[i].x;
		sum_y += intersectionCollection[i].y;
		circle(color_dst, intersectionCollection[i], 7, Scalar(0, 255, 255), 1);
	}

	cout << "�ҽ��� �ĺ� ���� : " << intersectionCollection.size() << endl;
	// �ҽ��� �ĺ����� ��ճ� ��ǥ
	Point averagePoint(sum_x / intersectionCollection.size(), sum_y / intersectionCollection.size());

	// ������ �ʹ� ���� ����Ǹ� �ҽ����� ���� �����. �׷���� �ҽ����� ��ȣ�� ������ ���
	// ȭ��� �ҽ����� ����Ǵ� ��츸 Ȯ����
	if ((averagePoint.x>0) && (averagePoint.x < color_dst.cols) && (averagePoint.y>0) && (averagePoint.y < color_dst.rows)){

		if (intersectionCollection.size() < 1000){ //1000�̶�� ���ڴ� ���Ƿ� ����
			circle(color_dst, averagePoint, 7, Scalar(0, 0, 255), 3);
			circle(bgr_dst, averagePoint, 7, Scalar(0, 0, 255), 3);
			cout << "PASS : �ҽ����� �ֽ��ϴ�" << endl;
		}
		else{
			cout << "FAIL : �ҽ����� ��ȣ�մϴ�" << endl;
		}

	}
	else{
		cout << "FAIL : �ҽ����� �����ϴ�" << endl;
	}

	namedWindow("findVanishingPoint_bgr", WINDOW_NORMAL);
	imshow("findVanishingPoint_bgr", bgr_dst);
	resizeWindow("findVanishingPoint_bgr", 640, 480);

	namedWindow("findVanishingPoint", WINDOW_NORMAL);
	imshow("findVanishingPoint", color_dst);
	resizeWindow("findVanishingPoint", 640, 480);

}

void findVanishingPoint_roi(Mat in_src){
	cout << "########## �ҽ��� �ľ� ########" << endl;

	Mat dst, color_dst, bgr_dst;
	bgr_dst = in_src.clone();
	Canny(in_src, dst, 40, 100, 3);
	cvtColor(dst, color_dst, COLOR_GRAY2BGR);

	Rect right_roi(dst.cols / 2, 0, dst.cols / 2, dst.rows);
	Rect left_roi(0, 0, dst.cols / 2, dst.rows);
	namedWindow("right roi", CV_WINDOW_AUTOSIZE);
	namedWindow("left roi", CV_WINDOW_AUTOSIZE);
	imshow("right roi", dst(right_roi));
	imshow("left roi", dst(left_roi));

	vector<vector<double>> linearEquationCollection_left;
	vector<vector<double>> linearEquationCollection_right;

	vector<Vec4i> left_lines;
	vector<Vec4i> right_lines;
	HoughLinesP(dst(left_roi), left_lines, 1, CV_PI / 180, 90, 20, 10);
	HoughLinesP(dst(right_roi), right_lines, 1, CV_PI / 180, 90, 20, 10);

	for (size_t i = 0; i < left_lines.size(); i++)
	{
		Point pt1(left_lines[i][0], left_lines[i][1]);
		Point pt2(left_lines[i][2], left_lines[i][3]);

		double pts_angle = (atan2(pt1.y - pt2.y, pt1.x - pt2.x) * 180) / CV_PI;

		if (((pts_angle>110) && (pts_angle < 160)) || ((pts_angle<-20) && (pts_angle>-70))){

			line(color_dst, pt1, pt2, Scalar(0, 255, 0), 1, 8);

			double linear_slope = (double)(pt1.y - pt2.y) / (pt1.x - pt2.x); // ����
			double linear_intercept = -linear_slope * pt1.x + pt1.y; // ����

			vector<double> linearEquation_left;
			linearEquation_left.push_back(linear_slope);
			linearEquation_left.push_back(linear_intercept);
			linearEquationCollection_left.push_back(linearEquation_left);
		}

	}

	for (size_t i = 0; i < right_lines.size(); i++)
	{
		Point pt1(right_lines[i][0] + dst.cols / 2, right_lines[i][1]);
		Point pt2(right_lines[i][2] + dst.cols / 2, right_lines[i][3]);

		double pts_angle = (atan2(pt1.y - pt2.y, pt1.x - pt2.x) * 180) / CV_PI;

		if (((pts_angle > 20) && (pts_angle < 70)) || ((pts_angle<-110) && (pts_angle>-160))){

			line(color_dst, pt1, pt2, Scalar(255, 0, 0), 1, 8);
			double linear_slope = (double)(pt1.y - pt2.y) / (pt1.x - pt2.x); // ����
			double linear_intercept = -linear_slope * pt1.x + pt1.y; // ����

			vector<double> linearEquation_right;
			linearEquation_right.push_back(linear_slope);
			linearEquation_right.push_back(linear_intercept);
			linearEquationCollection_right.push_back(linearEquation_right);
		}
	}

	//����� ���� ������ ������ �������� ���� �ľ�
	vector<Point> intersectionCollection;
	for (size_t j = 0; j < linearEquationCollection_left.size(); j++){

		double slope_a = linearEquationCollection_left[j][0];
		double intercept_a = linearEquationCollection_left[j][1];

		for (size_t k = 0; k < linearEquationCollection_right.size(); k++){

			double slope_b = linearEquationCollection_right[k][0];
			double intercept_b = linearEquationCollection_right[k][1];


			double intersection_x = (-intercept_a + intercept_b) / (slope_a - slope_b);
			double intersection_y = slope_a*intersection_x + intercept_a;

			Point intersection(intersection_x, intersection_y);
			intersectionCollection.push_back(intersection);
		}
	}

	double sum_x = 0;
	double sum_y = 0;
	for (size_t i = 0; i < intersectionCollection.size(); i++){
		sum_x += intersectionCollection[i].x;
		sum_y += intersectionCollection[i].y;
		circle(color_dst, intersectionCollection[i], 7, Scalar(0, 255, 255), 1);
	}

	cout << "�ҽ��� �ĺ� ���� : " << intersectionCollection.size() << endl;
	// �ҽ��� �ĺ����� ��ճ� ��ǥ
	Point averagePoint(sum_x / intersectionCollection.size(), sum_y / intersectionCollection.size());

	// ������ �ʹ� ���� ����Ǹ� �ҽ����� ���� �����. �׷���� �ҽ����� ��ȣ�� ������ ���
	// ȭ��� �ҽ����� ����Ǵ� ��츸 Ȯ����
	if ((averagePoint.x>0) && (averagePoint.x < color_dst.cols) && (averagePoint.y>0) && (averagePoint.y < color_dst.rows)){

		if (intersectionCollection.size() < 500){ //500�̶�� ���ڴ� ���Ƿ� ����
			circle(color_dst, averagePoint, 7, Scalar(0, 0, 255), 3);
			circle(bgr_dst, averagePoint, 7, Scalar(0, 0, 255), 3);
			cout << "GOOD : �ҽ����� �ֽ��ϴ�" << endl;
		}
		else{
			cout << "BAD : �ҽ����� ��ȣ�մϴ�" << endl;
		}

	}
	else{
		cout << "BAD : �ҽ����� �����ϴ�" << endl;
	}

	namedWindow("findVanishingPoint_bgr", WINDOW_NORMAL);
	imshow("findVanishingPoint_bgr", bgr_dst);
	resizeWindow("findVanishingPoint_bgr", 640, 480);

	namedWindow("findVanishingPoint", WINDOW_NORMAL);
	imshow("findVanishingPoint", color_dst);
	resizeWindow("findVanishingPoint", 640, 480);

}