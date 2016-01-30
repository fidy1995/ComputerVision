#include "Master.h"

string toString(int i) {
	char ch[80];
	_itoa_s(i, ch, 10);
	string s(ch);
	return s;
}

vector<string> split(string s) {
	stringstream ss(s);
	vector<string> ret;
	while (ss >> s) {
		ret.push_back(s);
	}
	return ret;
}

void gaussianFilter(IplImage *src) {
	int height = src->height;
	int width = src->width;
	int widthStep = src->widthStep;
	// this is the beginning of filtering
	// the standard diviation
	double normalSigma = 0.7;
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
	
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			src->imageData[i * widthStep + j] = temp[i * width + j];
		}
	}
	delete[] kernel;
	delete[] temp;
	// this is the end of filtering
}

Master::Master()
{
	init();
}


Master::~Master()
{
}

void Master::init() {
	w[0] = new Worker(0);
	w[1] = new Worker(1);

	string fileName, num;
	int count = 0;
	
	for (int i = 67; i <= 132; i += 2) {
		num = toString(i);
		fileName = root + prefixf + "/" + prefixf + "_" + num + suffix;
		IplImage *src1 = cvLoadImage(fileName.c_str(), 0);
		fileName = root + prefixt + "/" + prefixt + "_" + num + suffix;
		IplImage *src2 = cvLoadImage(fileName.c_str(), 0);
		fileName = root + prefixc + "/" + prefixc + "_" + num + suffix;
		IplImage *src3 = cvLoadImage(fileName.c_str(), 0);
		fileName = root + prefixd + "/" + prefixd + "_" + num + suffix;
		IplImage *src4 = cvLoadImage(fileName.c_str(), 0);
		fileName = root + prefixr + "/" + prefixr + "_" + num + suffix;
		IplImage *resu = cvLoadImage(fileName.c_str(), 0);

		/*gaussianFilter(src1);
		gaussianFilter(src2);
		gaussianFilter(src3);
		gaussianFilter(src4);*/
		cvSmooth(src1, src1, CV_GAUSSIAN, 3, 5);
		cvSmooth(src2, src2, CV_GAUSSIAN, 3, 5);
		cvSmooth(src3, src3, CV_GAUSSIAN, 3, 5);
		cvSmooth(src4, src4, CV_GAUSSIAN, 3, 5);
		
		int height = resu->height;
		int width = resu->width;
		int widthStep = resu->widthStep;

		for (int m = 0; m < height; m++) {			
			for (int n = 0; n < width; n++) {
				Pack p;
				unsigned char *p1 = (unsigned char *)src1->imageData + m * widthStep + n;
				unsigned char *p2 = (unsigned char *)src2->imageData + m * widthStep + n;
				unsigned char *p3 = (unsigned char *)src3->imageData + m * widthStep + n;
				unsigned char *p4 = (unsigned char *)src4->imageData + m * widthStep + n;
				unsigned char *p5 = (unsigned char *)resu->imageData + m * widthStep + n;
				int i1 = int(*p1), i2 = int(*p2), i3 = int(*p3), i4 = int(*p4), i5 = int(*p5);
				if (i1 || i2 || i3 || i4) {
					p.s1 = i1 / 8;
					p.s2 = i2 / 8;
					p.s3 = i3 / 8;
					p.s4 = i4 / 8;
					if (i5 > 0) {
						w[1]->add(p);
					}
					else {
						w[0]->add(p);
					}
					count++;
				}
			}
		}
		cvReleaseImage(&src1);
		cvReleaseImage(&src2);
		cvReleaseImage(&src3);
		cvReleaseImage(&src4);
		cvReleaseImage(&resu);
	}
	w[0]->done(count);
	w[1]->done(count);
}

void Master::determine(IplImage *src1, IplImage *src2, IplImage *src3, IplImage *src4,
	IplImage *trut, IplImage *dest) {
	int height = trut->height;
	int width = trut->width;
	int widthStep = trut->widthStep;
	for (int m = 0; m < height; m++) {
		for (int n = 0; n < width; n++) {
			Pack p; 
			unsigned char *p1 = (unsigned char *)src1->imageData + m * widthStep + n;
			unsigned char *p2 = (unsigned char *)src2->imageData + m * widthStep + n;
			unsigned char *p3 = (unsigned char *)src3->imageData + m * widthStep + n;
			unsigned char *p4 = (unsigned char *)src4->imageData + m * widthStep + n;
			int i1 = int(*p1), i2 = int(*p2), i3 = int(*p3), i4 = int(*p4);
			if (i1 || i2 || i3 || i4) {
				p.s1 = i1 / 8;
				p.s2 = i2 / 8;
				p.s3 = i3 / 8;
				p.s4 = i4 / 8;
				int pb = w[0]->determine(p);
				int pw = w[1]->determine(p);
				if (pw > pb) {
					dest->imageData[m * widthStep + n] = 255;
				}
				else {
					dest->imageData[m * widthStep + n] = 0;
				}
			}
			else {
				dest->imageData[m * widthStep + n] = 0;
			}
		}
	}
	int cnt = 0;
	for (int m = 0; m < height; m++) {
		unsigned char *pt = (unsigned char *)trut->imageData + m * widthStep;
		unsigned char *pd = (unsigned char *)dest->imageData + m * widthStep;
		for (int n = 0; n < width; n++) {
			int it = (int)*pt++ > 0 ? 255 : 0;
			int id = (int)*pd++;
			if (it != id) {
				cnt++;
			}
		}
	}
	double size = (double)width * height;
	cout << (size - cnt) / size << endl;
}

void Master::run() {
	while (1) {
		string temp;
		getline(cin, temp);
		vector<string> command = split(temp);
		size_t size = command.size();
		if (command[0] == "exit") {
			return;
		}
		if (command[0] == "det" && size == 2) {
			int num = atoi(command[1].c_str());
			if (num >= 67 && num <= 132) {
				string fileName = root + prefixf + "/" + prefixf + "_" + command[1] + suffix;
				IplImage *src1 = cvLoadImage(fileName.c_str(), 0);
				fileName = root + prefixt + "/" + prefixt + "_" + command[1] + suffix;
				IplImage *src2 = cvLoadImage(fileName.c_str(), 0);
				fileName = root + prefixc + "/" + prefixc + "_" + command[1] + suffix;
				IplImage *src3 = cvLoadImage(fileName.c_str(), 0);
				fileName = root + prefixd + "/" + prefixd + "_" + command[1] + suffix;
				IplImage *src4 = cvLoadImage(fileName.c_str(), 0);
				fileName = root + prefixr + "/" + prefixr + "_" + command[1] + suffix;
				IplImage *resu = cvLoadImage(fileName.c_str(), 0);
				IplImage *dest = cvCreateImage(cvGetSize(resu), 8, 1);
				/*gaussianFilter(src1);
				gaussianFilter(src2);
				gaussianFilter(src3);
				gaussianFilter(src4);*/
				cvSmooth(src1, src1, CV_GAUSSIAN, 3, 5);
				cvSmooth(src2, src2, CV_GAUSSIAN, 3, 5);
				cvSmooth(src3, src3, CV_GAUSSIAN, 3, 5);
				cvSmooth(src4, src4, CV_GAUSSIAN, 3, 5);
				determine(src1, src2, src3, src4, resu, dest);

				Mat mt = cvarrToMat(resu);
				Mat md = cvarrToMat(dest);
				imshow("truth", mt);
				imshow("my", md);

				waitKey(-1);
			}
		}
	}
}