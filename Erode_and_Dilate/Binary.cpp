#include <vector>
#include <iostream>

using namespace std;

void erosion(int **image, int **standard, int *size)
{
	for (int i = 0; i < size[0]; i++) {
		for (int j = 0; j < size[1]; j++) {
			// find target point
			if (image[i][j] == 1) {
				// do erosion
				for (int itemp = i; itemp < size[0] && (itemp - i < size[2]); itemp++) {
					for (int jtemp = j; jtemp < size[1] && (jtemp - j < size[3]); jtemp++) {
						// if standard not belongs to image
						if ((!image[itemp][jtemp]) & standard[itemp - i][jtemp - j]) { // if image == 0 && stardard == 1
							image[i][j] = 0;
							goto nextLoop;
						}
					}
				}
			nextLoop:
				; // here we go to the next loop
			}
		}
	}
}

void dilation(int **image, int **standard, int *size)
{
	for (int i = 0; i < size[0]; i++) {
		for (int j = 0; j < size[1]; j++) {
			// find target point
			if (image[i][j] == 0) {
				// do dilation
				for (int itemp = i; itemp < size[0] && (itemp - i < size[2]); itemp++) {
					for (int jtemp = j; jtemp < size[1] && (jtemp - j < size[3]); jtemp++) {
						// if standard intersects image
						if (image[itemp][jtemp] & standard[itemp - i][jtemp - j]) { // if image == 1 && standard == 1
							image[i][j] = 1;
							goto nextLoop;
						}
					}
				}
			nextLoop:
				; // here we go to the next loop
			}
		}
	}
}

int main() {
	/*********************************************************
	 Here we set a 2d-int array as the input image:
	 Although it takes much more space than the char ** form,
	 but it saves the running time by bit operations.
	 The structure is a 2d-int array too.
	 The following code is a simple test case.
	*********************************************************/
	int **image = new int *[6];
	int **image2 = new int *[6];
	for (int i = 0; i < 6; i++) {
		image[i] = new int[6];
		image2[i] = new int[6];
	}
	int im[36] = { 0, 1, 0, 1, 0, 0,
				   0, 1, 1, 0, 1, 0,
				   0, 0, 1, 0, 0, 0,
				   0, 0, 1, 1, 0, 0,
				   0, 1, 0, 1, 0, 0,
				   0, 0, 0, 0, 0, 0};
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			image[i][j] = im[i * 6 + j];
		}
	}
	int im2[36] = { 0, 1, 0, 1, 0, 0,
					0, 1, 1, 0, 1, 0,
					0, 0, 1, 0, 1, 0,
					0, 0, 1, 1, 0, 0,
					0, 1, 0, 1, 0, 0,
					0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			image2[i][j] = im2[i * 6 + j];
		}
	}
	int **stand = new int *[2];
	stand[0] = new int[2];
	stand[1] = new int[2];
	stand[0][0] = 1;
	stand[0][1] = 0;
	stand[1][0] = 1;
	stand[1][1] = 1;
	int size[4] = { 6, 6, 2, 2 };
	erosion(image, stand, size);
	dilation(image2, stand, size);
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			cout << image[i][j] << ' ';
		}
		cout << endl;
	}
	cout << endl;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			cout << image2[i][j] << ' ';
		}
		cout << endl;
	}
	system("pause");
	return 0;
}
