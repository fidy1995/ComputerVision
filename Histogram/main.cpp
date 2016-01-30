#include <cv.h>
#include <highgui.h>

using namespace std;

void histogramEqualization(IplImage *src, IplImage *dst) {
	int height = src->height;
	int width = src->width;
	int size = height * width;
	
	// calculate the occurred times of each grey level
	double histogram[256] = { 0.0 };
	int max = 0, min = 255;
	for (int m = 0; m < height; m++) {
		unsigned char* p = (unsigned char*)src->imageData + src->widthStep * m;
		for (int n = 0; n < src->widthStep; n++) {
			histogram[int(*p)] = histogram[int(*p)] + 1;
			if (int(*p) > max) {
				max = int(*p);
			}
			if (int(*p) < min) {
				min = int(*p);
			}
			p++;
		}
	}

	// generate density of each grey level
	for (int i = 0; i < 256; i++) {
		histogram[i] = (double)histogram[i] / size;
	}

	// generate add-ups for each grey level
	for (int i = 1; i < 256; i++) {
		histogram[i] = histogram[i] + histogram[i - 1];
	}

	// generate new histogram
	for (int i = 0; i < 256; i++) {
		histogram[i] = histogram[i] * (max - min) + min;
	}

	/*
	 * Maybe I can add up like this to decrese cycles:
	 *
	 * histogram[0] /= (float)size;
	 * histogram[0] *= 255;
	 * histogram[0] += min;
	 * for (int i = 1; i < 256; i++) {
	 *     histogram[i] /= (float)size;
	 *     histogram[i] *= (max - min);
	 *     histogram[i] += histogram[i - 1];
	 * }
	 * 
	 * But I think when learning a new technique,
	 * I should follow the steps to get better knowing of it.
	 */

	for (int m = 0; m < height; m++) {
		unsigned char* p = (unsigned char*)src->imageData + src->widthStep * m;
		for (int n = 0; n < width; n++) {
			dst->imageData[m * width + n] 
				= (unsigned int)histogram[int(*p++)];
		}
	}
}

int main(int argc, char *argv[])
{
	IplImage *src = cvLoadImage("mao.jpg", 0);
	IplImage *dst = cvCreateImage(cvGetSize(src), 8, 1);
	histogramEqualization(src, dst);

	cvNamedWindow("Origin", 1);
	cvShowImage("Origin", src);

	cvNamedWindow("After", 1);
	cvShowImage("After", dst);
	cvWaitKey(-1);

	cvDestroyAllWindows();
	cvReleaseImage(&src);
	cvReleaseImage(&dst);

	return 0;
}