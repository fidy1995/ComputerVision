#pragma once

struct Pack {
	int s1;
	int s2;
	int s3;
	int s4;
};

class Worker
{
	int s1[32];
	int s2[32];
	int s3[32];
	int s4[32];
	
	int total;

	double p1[32], p2[32], p3[32], p4[32];
	double p;

	int mark;
public:
	Worker(int m);
	~Worker();

	void add(Pack p);
	void done(int alt);

	double determine(Pack p);
};

