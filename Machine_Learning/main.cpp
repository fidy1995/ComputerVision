#include "Master.h"

#include <cv.h>
#include <highgui.h>

int main() {
	Master *m = new Master();
	m->run();

	return 0;
	/*string filename = root + prefixr + "/" + prefixr + "_" + toString(75) + suffix;*/
	/*IplImage *test = cvLoadImage(filename.c_str(), 0);
	cvNamedWindow("img");
	cvShowImage("img", test);
	cvWaitKey(-1);*/
	/*Mat m = imread(filename);
	for (int i = 0; i < 65536; i++) {
		int p = (int)m.at<unsigned char>(i / 256, i % 256);
		if (p != 0)
			cout << p << endl;
	}
	imshow("img", m);
	cvWaitKey(-1);
	system("pause");*/
}