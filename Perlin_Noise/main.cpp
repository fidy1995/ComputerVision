#include "perlin.h"

#include <cv.h>
#include <highgui.h>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	/*IplImage *src = cvCreateImage(cvSize(256, 256), IPL_DEPTH_32S, 3);
	Perlin *per = new Perlin();
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			double x = per->getValue(i, j) - floor(per->getValue(i, j));
			double r = 255.0 * x;
			double g = 215.0 * x;
			double b = 0;
			cvSet2D(src, i, j, CV_RGB(r, g, b));
		}
	}*/
	IplImage *src = cvCreateImage(cvSize(256, 256), IPL_DEPTH_8U, 1);
	Perlin *per = new Perlin();
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			unsigned char *p = (unsigned char *)src->imageData + i * 256 + j;
			*p = 255 * per->getValue(i, j);
		}
	}
	/*dst = cvCreateImage(cvSize(256, 256), IPL_DEPTH_8U, 1);
	cvSmooth(src, dst, 2, 3, 3);*/
	cvSaveImage("ctexture.bmp", src);
}

