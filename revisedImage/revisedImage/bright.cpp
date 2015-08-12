#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace cv;
using namespace std;

int judgeBright(Mat in_src);
void reviseBright(Mat in_src, Mat out_src);

int main(){

	Mat image = imread("파일이름.jpg");
	Mat out = image.clone();

	reviseBright(image, out);

	namedWindow("original", CV_WINDOW_AUTOSIZE);
	imshow("original", image);
	namedWindow("revised", CV_WINDOW_AUTOSIZE);
	imshow("revised", out);

	waitKey(0);
	destroyAllWindows();
	return 0;
}

//밝고 어두움 판정
//output 적당함 = 0 , 너무밝음 = 1 , 너무어두움 = 2
int judgeBright(Mat in_src){
	Mat hsvImage;
	vector<Mat> channel;
	cvtColor(in_src, hsvImage, CV_BGR2HSV);
	split(hsvImage, channel);
	int histSize = 256;
	float range[] = { 0, 256 };
	const float* histRange = { range };
	bool uniform = true;
	bool accumulate = false;
	Mat hist;
	calcHist(&channel[2], 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);

	int darkness = 0; int brightness = 0; int middle = 0;
	for (int i = 0; i < 256; i++){
		//cout << "Value" << i << "=" << hist.at<float>(i) << endl;
		if (i < 50){
			darkness += hist.at<float>(i);
		}
		else if (i >= 200){
			brightness += hist.at<float>(i);
		}
		else{
			middle += hist.at<float>(i);
		}
	}

	double darkRatio = (float)darkness / (darkness + brightness + middle) * 100;
	double brightRatio = (float)brightness / (darkness + brightness + middle) * 100;
	cout << endl << "dark = " << darkness << endl;
	cout << "bright = " << brightness << endl;
	cout << "dark비율 = " << darkRatio << endl;
	cout << "bright비율 = " << brightRatio << endl;

	if (darkRatio > 30){
		cout << "어두움" << endl;
		return 2;
	}
	else if (brightRatio > 30){
		cout << "밝음" << endl;
		return 1;
	}
	else{
		cout << "적당함" << endl;
		return 0;
	}
}

void reviseBright(Mat in_src, Mat out_src){
	int judge = judgeBright(in_src); // 적당함 = 0 , 너무밝음 = 1 , 너무어두움 = 2

	if (judge == 0){
		return;
	}

	cout << endl << "######################" << endl;
	cout << endl << "밝기 보정" << endl;

	Mat hsvImage, revised;
	vector<Mat> channel;
	cvtColor(in_src, hsvImage, CV_BGR2HSV);
	split(hsvImage, channel);
	int x = 0, y = 0;
	float c;
	double tmp;

	if (judge == 1){
		c = 1.4;
	}
	else if (judge == 2){
		c = 0.6;
	}

	for (int y = 0; y < channel[2].rows; y++){
		for (int x = 0; x < channel[2].cols; x++){
			tmp = channel[2].at<uchar>(y, x) / 255.0; // r
			tmp = 1 * pow(tmp, c);
			channel[2].at<uchar>(y, x) = tmp * 255.0;
		}
	}

	merge(channel, out_src);
	cvtColor(out_src, out_src, CV_HSV2BGR);

}
