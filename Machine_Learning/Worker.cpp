#include "Worker.h"

#include <fstream>

using namespace std;

Worker::Worker(int m)
{
	mark = m;
	for (int i = 0; i < 32; i++) {
		s1[i] = 1;
		s2[i] = 1;
		s3[i] = 1;
		s4[i] = 1;
	}
	total = 0;
}


Worker::~Worker()
{
}

void Worker::add(Pack pa) {
	s1[pa.s1]++;
	s2[pa.s2]++;
	s3[pa.s3]++;
	s4[pa.s4]++;
	total++;
}

void Worker::done(int count) {
	int t1 = 0, t2 = 0, t3 = 0, t4 = 0;
	for (int i = 0; i < 32; i++) {
		t1 += s1[i];
		t2 += s2[i];
		t3 += s3[i];
		t4 += s4[i];
	}
	for (int i = 0; i < 32; i++) {
		p1[i] = (double)s1[i] / t1;
		p2[i] = (double)s2[i] / t2;
		p3[i] = (double)s3[i] / t3;
		p4[i] = (double)s4[i] / t4;
	}
	p = (double)total / count;
}

double Worker::determine(Pack pa) {
	return 10000000.0 * p1[pa.s1] * p2[pa.s2] * p3[pa.s3] * p4[pa.s4] * p;
}
