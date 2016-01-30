#pragma once

#include "Worker.h"


#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/utility.hpp>

using namespace cv;
using namespace std;

const string prefixf = "BRATS_HG0005_FLAIR";
const string prefixt = "BRATS_HG0005_T1";
const string prefixc = "BRATS_HG0005_T1C";
const string prefixd = "BRATS_HG0005_T2";
const string prefixr = "BRATS_HG0005_truth";
const string suffix = ".png";
const string root = "./hg0005/";

string toString(int i);

class Master
{
	Worker *w[2];
public:
	Master();
	~Master();

	void init();
	void run();
	void determine(IplImage *src1, IplImage *src2, IplImage *src3, IplImage *src4,
		IplImage *trut, IplImage *dest);
};

