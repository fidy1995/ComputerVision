#include <cv.h>
#include <highgui.h>

#include <math.h>
#include <iostream>

using namespace std;

int const xdir[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };
int const ydir[8] = { -1, -1, -1, 0, 1, 1, 1, 0 };

double highThreshold;
double lowThreshold;

void trace(int x, int y, int threshold, bool *judge, int *m, IplImage *dst) {
	int xx, yy;
	for (int i = 0; i < 8; i++) {
		xx = x + xdir[i];
		yy = y + ydir[i];
		if (judge[xx * dst->width + yy] 
			&& m[xx * dst->width + yy] >= threshold
			&& dst->imageData[xx * dst->widthStep + yy] == 0) {
			dst->imageData[xx * dst->widthStep + yy] = 255;
			trace(xx, yy, threshold, judge, m, dst);
		}
	}
}

/*
 * This function is written in my homework "skeleton".
 */
void Otsu(int *m, int size) 
{
	int ret; // 阈值
	int countBG[1500] = { 0 }; // 背景总个数（略去前景，直接使用1-count）
	int countOR[1500] = { 0 };

	double probBG = 0; // 背景概率（占比）
	double probOR = 0; // 前景概率（占比）
	double variance = 0; // 类间方差
	double averageGrey = 0; // 图像的平均灰度
	double maxVariance = 0; // 最大类间方差

	float histogram[1500] = { 0 };
	double averageBGGrey[1500] = { 0.0 };
	double averageORGrey[1500] = { 0.0 };

	for (int i = 0; i < size; i++) {
		histogram[m[i]]++;
	}

	// 获得图像平均灰度
	for (int threshold = 0; threshold < 1500; threshold++) {
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
	for (int i = 0; i < 1500; i++) {
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
	highThreshold = ret;
	lowThreshold = 0.5 * highThreshold;
}

void CannyEdgeDetect(IplImage *src, IplImage *dst)
{
	int width = src->width;
	int widthStep = src->widthStep;
	int height = src->height;
	//int size = src->imageSize;
	
	// this is the beginning of filtering
	// the standard diviation
	double normalSigma = 0.4;
	// the filtering window size
	int normalWindowSize = 1 + 2 * ceil(3 * normalSigma);
	// the center of filtering window
	int normalWindowCenter = normalWindowSize / 2;

	// 1d kernel function
	double *kernel = new double[normalWindowSize];
	double kernelSum = 0.0;
	// a container to save data
	unsigned char *temp = new unsigned char[height * width]();
	// calculate kernel
	for (int i = 0; i < normalWindowSize; i++) {
		double distance = (double)(i - normalWindowCenter);
		kernel[i] = exp(-(0.5) * distance * distance / (normalSigma * normalSigma))
			/ (sqrt(2 * 3.14159) * normalSigma);
		kernelSum += kernel[i];
	}
	for (int i = 0; i < normalWindowSize; i++) {
		kernel[i] /= kernelSum;
	}
	// filtering x and y with kernel
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			double sum = 0.0;
			double filter = 0.0;
			for (int limit = -normalWindowCenter; limit <= normalWindowCenter; limit++) {
				int currentPosition = j + limit;
				if (currentPosition >= 0 && currentPosition < width) {
					filter += (double)src->imageData[i * widthStep + currentPosition]
						* kernel[normalWindowCenter + limit];
					sum += kernel[normalWindowCenter + limit];
				}
			}
			temp[i * width + j] = (unsigned char)(int)filter / sum;
		}
	}
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			double sum = 0.0;
			double filter = 0.0;
			for (int limit = -normalWindowCenter; limit <= normalWindowCenter; limit++) {
				int currentPosition = j + limit;
				if (currentPosition >= 0 && currentPosition < height) {
					filter += (double)temp[i * width + currentPosition]
						* kernel[normalWindowCenter + limit];
					sum += kernel[normalWindowCenter + limit];
				}
			}
			temp[i * width + j] = (unsigned char)(int)filter / sum;
		}
	}
	delete[] kernel;
	// this is the end of filtering

	// this is the beginning of calculating gratitude
	double *p = new double[width * height](); // x partial derivative
	double *q = new double[width * height](); // y partial derivative
	int    *m = new    int[width * height](); // gratitude
	double *t = new double[width * height](); // gratitude direction
	// calculating x and y partial derivative
	// with simple operator 
	/*for (int i = 0; i < height - 1; i++) {
		for (int j = 0; j < width - 1; j++) {
			int index = i * width + j;
			p[index] =
				(double)(temp[i * width + min(j + 1, width - 1)] 
				- temp[i * width + j]
				+ temp[min(i + 1, height - 1) * width + min(j + 1, width - 1)]
				- temp[min(i + 1, height - 1) * width + j]) / 2;
			q[index] =
				(double)(temp[i * width + j]
				- temp[min(i + 1, height - 1) * width + j]
				+ temp[i * width + min(j + 1, width - 1)]
				- temp[min(i + 1, height - 1) * width + min(j + 1, width - 1)]) / 2;
		}
	}*/
	// with Sobel operator
	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < height - 1; j++) {
			double sobel[8] = {
				temp[(i - 1) * width + j - 1],
				temp[(i - 1) * width + j    ],
				temp[(i - 1) * width + j + 1],
				temp[(i    ) * width + j + 1],
				temp[(i + 1) * width + j + 1],
				temp[(i + 1) * width + j    ],
				temp[(i + 1) * width + j - 1],
				temp[(i    ) * width + j - 1],
			};
			p[i * width + j]
				= (sobel[2] + 2 * sobel[3] + sobel[4])
				- (sobel[0] + 2 * sobel[7] + sobel[6]);
			q[i * width + j]
				= (sobel[0] + 2 * sobel[1] + sobel[2])
				- (sobel[6] + 2 * sobel[5] + sobel[4]);
		}
	}
	// calculating gratitude and its direction
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int index = i * width + j;
			m[index] = sqrt(p[index] * p[index] + q[index] * q[index]) + 0.5;
			// from radius to degree
			t[index] = atan2(q[index], p[index]) * 57.3;
			// change to [0, 360)
			if (t[index] < 0) {
				t[index] += 360;
			}  
		}
	}
	delete[] temp;
	// this is the end of calculating gratitude

	// this is the beginning of non maximum suppression
	int g[4] = { 0 }; // interpolation
	double hist1 = 0.0, hist2 = 0.0;
	double weight = 0.0;
	bool *judge = new bool[height * width];
	for (int i = 0; i < height * width; i++) {
		judge[i] = false;
	}

	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			int index = i * width + j;
			/*
			 * The area is divide the plane into 8 areas by angle:
			 *   2  1
			 * 3      0
			 * 4      8
			 *   5  6
			 * draw lines connecting the center and the contract areas:
			 *   2  1
			 * 3      0
			 * 0      3
			 *   1  2
			 */
			int area = ((int)t[index] / 45) % 4;

			// if the gratitude is 0, then it's surely not the maxinum
			if (m[index] == 0) {
				;
			}

			// now checkout the directions:
			else {
				switch (area) {
				case 0:
					/*
					 *             g[0]
					 * g[2] center g[1]
					 * g[3]
					 */
					g[0] = m[index - width + 1];
					g[1] = m[index + 1];
					g[2] = m[index - 1];
					g[3] = m[index + width - 1];
					weight = fabs(q[index]) / fabs(p[index]);
					hist1 = g[0] * weight + g[1] * (1 - weight);
					hist2 = g[3] * weight + g[2] * (1 - weight);
					break;
				case 1:
					/*
					 *       g[0]  g[1]
					 *      center 
					 * g[2]  g[3]
					 */
					g[0] = m[index - width];
					g[1] = m[index - width + 1];
					g[2] = m[index + width - 1];
					g[3] = m[index + width];
					weight = fabs(p[index]) / fabs(q[index]);
					hist1 = g[1] * weight + g[0] * (1 - weight);
					hist2 = g[2] * weight + g[3] * (1 - weight);
					break;
				case 2:
					/*
					 * g[0]  g[1]
					 *      center
					 *       g[2]  g[3]
					 */
					g[0] = m[index - width - 1];
					g[1] = m[index - width];
					g[2] = m[index + width];
					g[3] = m[index + width + 1];
					weight = fabs(p[index]) / fabs(q[index]);
					hist1 = g[0] * weight + g[1] * (1 - weight);
					hist2 = g[3] * weight + g[2] * (1 - weight);
					break;
				case 3:
					/*
					 * g[1]
					 * g[0] center g[3]
					 *             g[2]
					 */
					g[0] = m[index - 1];
					g[1] = m[index - width - 1];
					g[2] = m[index + width + 1];
					g[3] = m[index + 1];
					weight = fabs(q[index]) / fabs(p[index]);
					hist1 = g[1] * weight + g[0] * (1 - weight);
					hist2 = g[2] * weight + g[3] * (1 - weight);
					break;
				}
			}
			// judge if it is the maxinum
			if (m[index] >= hist1 && m[index] >= hist2) {
				judge[index] = true;
			}
		}
	}
	// this is the end of non maximum suppression

	// this is the beginning of double threshold detecting
	/*int hist[1500] = { 0 };
	int edgeCount = 0;
	int maxGrat = 0;
	int highCount = 0;

	double highRate = 0.84;
	double lowRate = 0.5;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int index = i * width + j;
			if (judge[index]) {
				hist[m[index]]++;
			}
		}
	}
	for (int i = 0; i < 400; i++) {
		if (hist[i]) {
			maxGrat = i;
			edgeCount += hist[i];
		}
	}

	highCount = highRate * edgeCount + 0.5;
	int a = 1;
	edgeCount = 0;
	while (a < maxGrat && edgeCount < highCount) {
		edgeCount += hist[a];
		a++;
	}
	highThreshold = (double)a;
	lowThreshold = highThreshold * lowRate + 0.5;*/
	// this is the end of double threshold detecting

	// i found an algorithm just like Otsu
	// it is said to divide the picture into 2 parts
	// and calculate its variance, when it reach the maxinum
	// the threshold would be high threshold.
	// the low threshold should be 1/2 to 1/3 times of the high one.
	// i take 1/2 here.
	Otsu(m, width * height);

	// initialize destination pic
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < widthStep; j++) {
			dst->imageData[i * widthStep + j] = 0;
		}
	}
	// start processing
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int index = i * width + j;
			if (judge[index] && m[index] >= highThreshold) {
				dst->imageData[i * widthStep + j] = 255;
				trace(i, j, lowThreshold, judge, m, dst);
			}
		}
	}
	// ending, release memory
	delete[] p;
	delete[] q;
	delete[] m;
	delete[] t;
	delete[] judge;
}

int main()
{
	IplImage *src = cvLoadImage("horde.jpg", 0);
	IplImage *dst = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage *dst2 = cvCreateImage(cvGetSize(src), 8, 1);

	cvNamedWindow("src", 1);
	cvNamedWindow("cvCanny", 1);
	cvNamedWindow("myCanny", 1);

	CannyEdgeDetect(src, dst);
	cvCanny(src, dst2, lowThreshold, highThreshold);
	cvShowImage("src", src);
	cvShowImage("myCanny", dst);
	cvShowImage("cvCanny", dst2);
	cvWaitKey(-1);

	cvDestroyWindow("src");
	cvDestroyWindow("cvCanny");
	cvDestroyWindow("myCanny");
	cvReleaseImage(&src);
	cvReleaseImage(&dst);
	cvReleaseImage(&dst2);

	return 0;
}