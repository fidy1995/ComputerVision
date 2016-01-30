/*
 * Warnings: I can't change the indent block to 4 spaces.
 * Please change it manually to receive a better view.
 * 
 * Author: Fred Jiang, SE Inst., Shanghai Jiaotong Univ.
 */

#include <iostream>
#include <cv.h>
#include <highgui.h>  
#include <math.h>
#include <vector>

#define _BG 0
#define _OR 255

float histogram[256] = { 0 };
double averageBGGrey[256] = { 0.0 };
double averageORGrey[256] = { 0.0 };
int height, width, size;
int greyMax = 0;
int greyMin = 256;
double c;

// 统计灰度图
void generateHistoGram();
// 统计平均灰度
void generateAverageAndC();
int Otsu();
int minDivergence();
int linearIndex();
int quardicIndex();
int maxSimilarity();
int geometry();

using namespace std;
using namespace cv;

IplImage *src;
IplImage *dst;
IplImage *tempd[6];

int main(int argc, char** argv)
{
	src = cvLoadImage("test.bmp", 0);
	dst = cvCreateImage(cvGetSize(src), 8, 1);
	height = src->height;
	width = src->width;
	size = height * width;
	generateHistoGram();
	generateAverageAndC();

	for (int i = 0; i < 6; i++) {
		int threshold;
		switch (i) {
		case 0:
			threshold = Otsu();
			break;
		case 1:
			threshold = minDivergence();
			break;
		case 2:
			threshold = linearIndex();
			break;
		case 3:
			threshold = quardicIndex();
			break;
		case 4:
			threshold = maxSimilarity();
			break;
		case 5:
			threshold = geometry();
			break;
		}
		cvThreshold(src, dst, threshold, 255, CV_THRESH_BINARY);
		// 为了在一个窗口中显示六张图片，把图片缩小
		tempd[i] = cvCreateImage(CvSize(width * 0.5, height * 0.5), 8, 1);
		cvResize(dst, tempd[i], CV_INTER_LINEAR);

		cvReleaseImage(&dst);
		dst = cvCreateImage(cvGetSize(src), 8, 1);
	}
	cvReleaseImage(&src);
	cvReleaseImage(&dst);

	// 在一个窗口中显示6张图片
	CvSize dstSize;
	dstSize.width = width * 1.5;
	dstSize.height = height * 1.0;
	dst = cvCreateImage(dstSize, 8, 1);
	cvZero(dst);
	for (int i = 0; i < 6; i++) {
		cvSetImageROI(dst, cvRect(width * (i % 3) / 2, height * (i / 3) / 2, width / 2, height / 2));
		if (i > 0) dst->nChannels = 1;
		cvCopy(tempd[i], dst);
		cvResetImageROI(dst);
	}
	cvNamedWindow("img", 1);
	cvShowImage("img", dst);
	cvWaitKey(-1);
	cvDestroyWindow("dst");
	system("pause");
	return 0;
}

void generateHistoGram() {
	// 获得图像灰度
	for (int m = 0; m < height; m++)
	{
		unsigned char* p = (unsigned char*)src->imageData + src->widthStep * m;
		for (int n = 0; n < width; n++)
		{
			histogram[int(*p++)]++;
		}
	}
}

void generateAverageAndC() {
	// 获得图像平均灰度
	for (int threshold = 0; threshold < 256; threshold++) {
		int countBG = 0, countOR = 0;
		// 获得在阈值为threshold时背景图像的平均灰度
		for (int i = 0; i <= threshold; i++) {
			averageBGGrey[threshold] += i * histogram[i];
			countBG += histogram[i];
			// 获得图像中灰度的最小值和最大值
			if (greyMin > i && histogram[i] > 0) {
				greyMin = i;
			}
			if (greyMax < i && histogram[i] > 0) {
				greyMax = i;
			}
		}
		averageBGGrey[threshold] = averageBGGrey[threshold] / countBG;
		// 获得在阈值为threshold时前景图像的平均灰度
		for (int i = threshold + 1; i < 256; i++) {
			averageORGrey[threshold] += i * histogram[i];
			countOR += histogram[i];
			// 获得图像中灰度的最小值和最大值
			if (greyMin > i && histogram[i] > 0) {
				greyMin = i;
			}
			if (greyMax < i && histogram[i] > 0) {
				greyMax = i;
			}
		}
		averageORGrey[threshold] = averageORGrey[threshold] / countOR;
	}
	c = (double)1.0 / (greyMax - greyMin);
}

int Otsu() {
	int threshold; // 阈值
	int countBG = 0; // 背景总个数（略去前景，直接使用1-count）

	double probBG = 0; // 背景概率（占比）
	double probOR = 0; // 前景概率（占比）
	double variance = 0; // 类间方差
	double averageGrey = 0; // 图像的平均灰度
	double maxVariance = 0; // 最大类间方差
	
	// 枚举所有的threshold值，找出最大类间方差
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j <= i; j++) {
			countBG += averageBGGrey[i];
		}
		probBG = (double)countBG / size;
		probOR = 1.0 - probBG;
		// 计算图像平均灰度和类间方差
		averageGrey = averageBGGrey[i] * probBG + averageORGrey[i] * probOR; 
		variance = probBG * probOR *  (averageBGGrey[i] - averageORGrey[i]) * (averageBGGrey[i] - averageORGrey[i]);

		// 获得最大值
		if (variance > maxVariance) {
			maxVariance = variance;
			threshold = i;
		}
	}

	cout << "The threshold of Otsu is: " << threshold << endl;
	return threshold;
}

double miu(int a, int threshold) {
	// 在模糊集理论中的membership function
	if (a > threshold) {
		return exp(-c * abs(a - averageORGrey[threshold]));
	}
	else {
		return exp(-c * abs(a - averageBGGrey[threshold]));
	}
}

int minDivergence() {
	int threshold;
	int result;
	double divergence;
	double minDiver = 1000000000.0;
	// 枚举所有
	for (threshold = 0; threshold < 256; threshold++) {
		divergence = 0.0;
		// 统计所有点的区分度
		for (int m = 0; m < height; m++) {
			unsigned char* p = (unsigned char*)src->imageData + src->widthStep * m;
			for (int n = 0; n < width; n++) {
				int grey = int(*p++); // aij, 图像在(i, j)处的灰度
				double miuA = miu(grey, threshold); // miua(aij)
				double miuB = 1.0; // 默认值取1
				divergence += (2.0 - (1.0 - miuA + miuB) * exp(miuA - miuB)
					- (1.0 - miuB + miuA) * exp(miuB - miuA));
			}
		}
		if (divergence < minDiver) {
			result = threshold;
			minDiver = divergence;
		}
	}
	cout << "The threshold of min divergence is: " << result << endl;
	return result;
}

int linearIndex() {
	int threshold;
	int result;
	double index;
	double minIndex = 100000000.0;
	for (threshold = 0; threshold < 256; threshold++) {
		index = 0;
		// 统计线性模糊指数
		for (int m = 0; m < height; m++) {
			unsigned char* p = (unsigned char*)src->imageData + src->widthStep * m;
			for (int n = 0; n < width; n++) {
				int grey = int(*p++); // aij, 图像在(i, j)处的灰度
				double miuA = miu(grey, threshold);
				index += min(miuA, 1.0 - miuA);
			}
		}
		// 在这里对公式进行了简化，因为所有的比较都有乘以2/n，所以此处相当于消去
		if (index < minIndex) {
			result = threshold;
			minIndex = index;
		}
	}
	cout << "The threshold of linear index is: " << result << endl;
	return result;
}

int quardicIndex() {
	int threshold;
	int result;
	double index;
	double minIndex = 100000000.0;
	for (threshold = 0; threshold < 256; threshold++) {
		index = 0;
		// 统计平方模糊指数
		for (int m = 0; m < height; m++) {
			unsigned char* p = (unsigned char*)src->imageData + src->widthStep * m;
			for (int n = 0; n < width; n++) {
				int grey = int(*p++); // aij, 图像在(i, j)处的灰度
				double miuA = miu(grey, threshold);
				index += pow(min(miuA, 1.0 - miuA), 2);
			}
		}
		// 在这里对公式进行了简化，因为所有的比较都有先平方再乘以2/sqrt(n)，所以此处相当于消去
		if (index < minIndex) {
			result = threshold;
			minIndex = index;
		}
	}

	cout << "The threshold of quardic index is: " << result << endl;
	return result;
}

int maxSimilarity() {
	int threshold;
	int result;
	double similarity;
	double maxSim = 0.0;
	for (threshold = 0; threshold < 256; threshold++) {
		similarity = 0;
		// 统计相似度
		for (int m = 0; m < height; m++) {
			unsigned char* p = (unsigned char*)src->imageData + src->widthStep * m;
			for (int n = 0; n < width; n++) {
				int grey = int(*p++); // aij, 图像在(i, j)处的灰度
				double miuA = miu(grey, threshold);
				similarity += miuA;
			}
		}
		// 计算最大的相似度
		if (similarity > maxSim) {
			result = threshold;
			maxSim = similarity;
		}
	}

	cout << "The threshold of max similarity is: " << result << endl;
	return result;
}

int geometry() {
	int threshold;
	int result;
	double aMiu, pMiu;
	double comp;
	double maxComp = 0.0;
	for (threshold = 0; threshold < 256; threshold++) {
		aMiu = 0;
		pMiu = 0;
		// 统计所有点的区分度
		for (int m = 0; m < height; m++) {
			unsigned char* p = (unsigned char*)src->imageData + src->widthStep * m;
			for (int n = 0; n < width; n++) {
				int grey = int(*p++); // aij, 图像在(i, j)处的灰度
				double miuA = miu(grey, threshold);
				aMiu += miuA;
			}
		}
		// Pmiu的第一项和
		for (int m = 0; m < height - 1; m++) {
			unsigned char* p = (unsigned char*)src->imageData + src->widthStep * m;
			unsigned char* p1 = (unsigned char*)src->imageData + src->widthStep * (m + 1);
			for (int n = 0; n < width; n++) {
				int grey = int(*p++);
				double miuP = miu(grey, threshold);
				int grey1 = int(*p1++);
				double miuP2 = miu(grey1, threshold);
				pMiu += abs(miuP - miuP2);
			}
		}
		// Pmiu的第二项和
		for (int m = 0; m < height; m++) {
			unsigned char* p = (unsigned char*)src->imageData + src->widthStep * m;
			unsigned char* p1 = (unsigned char*)src->imageData + src->widthStep * m + 1;
			for (int n = 0; n < width - 1; n++) {
				int grey = int(*p++);
				double miuP = miu(grey, threshold);
				int grey1 = int(*p1++);
				double miuP2 = miu(grey1, threshold);
				pMiu += abs(miuP - miuP2);
			}
		}
		// 原公式
		comp = aMiu / (pMiu * pMiu);
		if (comp > maxComp) {
			result = threshold;
			maxComp = comp;
		}
	}

	cout << "The threshold of max compactness is: " << result << endl;
	return result;
}
