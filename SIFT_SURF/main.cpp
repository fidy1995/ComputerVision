#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

using namespace cv;

int main()
{
	Mat img_1 = imread("mao.jpg");
	Mat img_2 = imread("mao2.jpg");

	cv::Ptr<Feature2D> f2d = xfeatures2d::SIFT::create();

	//-- Step 1: Detect the keypoints:
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	f2d->detect(img_1, keypoints_1);
	f2d->detect(img_2, keypoints_2);

	//-- Step 2: Calculate descriptors (feature vectors)    
	Mat descriptors_1, descriptors_2;
	f2d->compute(img_1, keypoints_1, descriptors_1);
	f2d->compute(img_2, keypoints_2, descriptors_2);

	//-- Step 3: Matching descriptor vectors using BFMatcher :
	BFMatcher matcher;
	std::vector<DMatch> matches;
	matcher.match(descriptors_1, descriptors_2, matches);

	Mat out;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, out);

	imshow("result", out);
	cvWaitKey(-1);

	return 0;
}