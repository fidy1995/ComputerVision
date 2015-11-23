#include <cv.h>
#include <highgui.h>

#include <vector>
#include <iostream>

using namespace std;

int Otsu(IplImage *src) {
	int ret; // 阈值
	int countBG[256] = { 0 }; // 背景总个数（略去前景，直接使用1-count）
	int countOR[256] = { 0 };

	double probBG = 0; // 背景概率（占比）
	double probOR = 0; // 前景概率（占比）
	double variance = 0; // 类间方差
	double averageGrey = 0; // 图像的平均灰度
	double maxVariance = 0; // 最大类间方差

	float histogram[256] = { 0 };
	double averageBGGrey[256] = { 0.0 };
	double averageORGrey[256] = { 0.0 };
	int height = src->height, width = src->width;
	int size = height * width;

	for (int m = 0; m < height; m++) {
		unsigned char* p = (unsigned char*)src->imageData + src->widthStep * m;
		for (int n = 0; n < width; n++) {
			histogram[int(*p++)]++;
		}
	}

	// 获得图像平均灰度
	for (int threshold = 0; threshold < 256; threshold++) {
		// 获得在阈值为threshold时背景图像的平均灰度
		for (int i = 0; i <= threshold; i++) {
			averageBGGrey[threshold] += i * histogram[i];
			countBG[threshold] += histogram[i];
		}
		averageBGGrey[threshold] = averageBGGrey[threshold] / countBG[threshold];
		// 获得在阈值为threshold时前景图像的平均灰度
		for (int i = threshold + 1; i < 256; i++) {
			averageORGrey[threshold] += i * histogram[i];
			countOR[threshold] += histogram[i];
		}
		averageORGrey[threshold] = averageORGrey[threshold] / countOR[threshold];
	}

	// 枚举所有的threshold值，找出最大类间方差
	for (int i = 0; i < 256; i++) {
		probBG = (double)countBG[i] / size;
		probOR = 1.0 - probBG;
		// 计算图像平均灰度和类间方差
		averageGrey = averageBGGrey[i] * probBG + averageORGrey[i] * probOR;
		variance = probBG * probOR *  (averageBGGrey[i] - averageORGrey[i]) * (averageBGGrey[i] - averageORGrey[i]);

		// 获得最大值
		if (variance > maxVariance) {
			maxVariance = variance;
			ret = i;
		}
	}

	//cout << "The threshold of Otsu is: " << ret << endl;
	return ret;
}

void cvBitNot(IplImage *src, IplImage *dst) {
	//cout << "Entering cvBitNot\n";
	for (int m = 0; m < src->height; m++) {
		int p = m * src->widthStep;
		for (int n = 0; n < src->width; n++) {
			dst->imageData[p] = (char)255 - src->imageData[p];
			p++;
		}
	}
	//cout << "Leaving  cvBitNot\n";
}

void cvBitAnd(IplImage *src1, IplImage *src2, IplImage *dst) {
	//cout << "Entering cvBitAnd\n";
	for (int m = 0; m < src1->height; m++) {
		int p = m * src1->widthStep;
		for (int n = 0; n < src1->width; n++) {
			dst->imageData[p] = src1->imageData[p] & src2->imageData[p];
			p++;
		}
	}
	//cout << "Leaving  cvBitAnd\n";
}

void cvBitOr(IplImage *src1, IplImage *src2, IplImage *dst) {
	//cout << "Entering cvBitOr\n";
	for (int m = 0; m < src1->height; m++) {
		int p = m * src1->widthStep;
		for (int n = 0; n < src1->width; n++) {
			dst->imageData[p] = src1->imageData[p] | src2->imageData[p];
			p++;
		}
	}
	//cout << "Leaving  cvBitOr\n";
}

#define CON(_i) (_i + 8)
vector<int *> st;

void generateSt() {
	st.resize(16);
	for (int i = 0; i < 16; i++) {
		st[i] = new int[9];
	}

	st[0][4] = 1;
	st[0][6] = 1;
	st[0][7] = 1;
	st[0][8] = 1;
	st[CON(0)][0] = 1;
	st[CON(0)][1] = 1;
	st[CON(0)][2] = 1;

	st[1][3] = 1;
	st[1][4] = 1;
	st[1][6] = 1;
	st[1][7] = 1;
	st[CON(1)][1] = 1;
	st[CON(1)][2] = 1;
	st[CON(1)][5] = 1;

	st[2][0] = 1;
	st[2][3] = 1;
	st[2][4] = 1;
	st[2][6] = 1;
	st[CON(2)][2] = 1;
	st[CON(2)][5] = 1;
	st[CON(2)][8] = 1;

	st[3][0] = 1;
	st[3][1] = 1;
	st[3][3] = 1;
	st[3][4] = 1;
	st[CON(3)][5] = 1;
	st[CON(3)][7] = 1;
	st[CON(3)][8] = 1;

	st[4][0] = 1;
	st[4][1] = 1;
	st[4][2] = 1;
	st[4][4] = 1;
	st[CON(4)][6] = 1;
	st[CON(4)][7] = 1;
	st[CON(4)][8] = 1;

	st[5][1] = 1;
	st[5][2] = 1;
	st[5][4] = 1;
	st[5][5] = 1;
	st[CON(5)][3] = 1;
	st[CON(5)][6] = 1;
	st[CON(5)][7] = 1;

	st[6][2] = 1;
	st[6][4] = 1;
	st[6][5] = 1;
	st[6][8] = 1;
	st[CON(6)][0] = 1;
	st[CON(6)][3] = 1;
	st[CON(6)][6] = 1;

	st[7][4] = 1;
	st[7][5] = 1;
	st[7][7] = 1;
	st[7][8] = 1;
	st[CON(7)][0] = 1;
	st[CON(7)][1] = 1;
	st[CON(7)][3] = 1;
}

void erode(IplImage *src, IplImage *dst, int stNo) {
	//cout << "Entering erode\n";
	int h = src->height;
	int w = src->width;
	for (int i = 1; i < h - 1; i++) {
		for (int j = 1; j < w - 1; j++) {
			int p = i * src->widthStep + j;
			dst->imageData[p] = (char)255;
			for (int itemp = -1; itemp < 2; itemp++) {
				for (int jtemp = -1; jtemp < 2; jtemp++) {
					if (st[stNo][(itemp + 1) * 3 + (jtemp + 1)] == 1 
						&& src->imageData[p + itemp * src->widthStep + jtemp] == 0) {
						dst->imageData[p] = 0;
						goto nextLoop;
					}
				}
			}
		nextLoop:
			;
		}
	}
	//cout << "Leaving  erode\n";
}

void hitOrMiss(IplImage *src, IplImage *dst, int stNo) {
	//cout << "Entering hitOrMiss\n";
	IplImage *conSrc = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage *temp1 = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage *temp2 = cvCreateImage(cvGetSize(src), 8, 1);

	cvBitNot(src, conSrc);

	erode(src, temp1, stNo);
	erode(conSrc, temp2, CON(stNo));

	cvBitAnd(temp1, temp2, dst);
	cvReleaseImage(&conSrc);
	cvReleaseImage(&temp1);
	cvReleaseImage(&temp2);
	//cout << "Leaving  hitOrMiss\n";
	return;
}

void thining(IplImage *src, IplImage *dst, int stNo) {
	//cout << "Entering thining\n";
	IplImage *con = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage *temp = cvCreateImage(cvGetSize(src), 8, 1);

	hitOrMiss(src, temp, stNo);
	cvBitNot(temp, con);
	cvBitAnd(src, con, dst);
	cvReleaseImage(&con);
	cvReleaseImage(&temp); 
	//cout << "Leaving  thining\n";
	return;
}

void thickening(IplImage *src, IplImage *dst, int stNo) {
	IplImage *temp = cvCreateImage(cvGetSize(src), 8, 1);

	hitOrMiss(src, temp, stNo);
	cvBitOr(src, temp, dst);
	cvReleaseImage(&temp);
	cvNamedWindow("img", 1);
	cvShowImage("img", dst);
	cvWaitKey(-1);
	cvDestroyWindow("img");
	return;
}

int main() {
	IplImage *src0 = cvLoadImage("potato.bmp", 0);
	IplImage *src = cvCreateImage(cvGetSize(src0), 8, 1);
	IplImage *dst = cvCreateImage(cvGetSize(src0), 8, 1);
	int threshold = Otsu(src0);
	cvThreshold(src0, src, threshold, 255, CV_THRESH_BINARY);

	cvNamedWindow("img", 1);
	cvShowImage("img", src);
	cvWaitKey(10);

	generateSt();

	for (int i = 1; i < 46; i++) {
		for (int j = 0; j < 8; j++) {
			thining(src, dst, j);
			cvReleaseImage(&src);
			src = cvCreateImage(cvGetSize(src0), 8, 1);
			cvCopy(dst, src);
			cvReleaseImage(&dst);
			dst = cvCreateImage(cvGetSize(src), 8, 1);
			cout << "round:" << i << ",step: " << j + 1 << endl;
			cvShowImage("img", src);
			cvWaitKey(10);
		}
	}
	cvShowImage("img", src);
	cvWaitKey(-1);
	cvDestroyWindow("img");

	for (int i = st.size() - 1; i >= 0; i--) {
		delete[] st[i];
		st.pop_back();
	}

	cvReleaseImage(&src);
	cvReleaseImage(&dst);
	
	return 0;
}