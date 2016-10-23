#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "sumsquaredistance.h"
#include "opencv2/opencv_modules.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace std;
using namespace cv;
using namespace std::tr1;

/*
	Works out the sum of squared difference between an image and all images in a given path.
	Not very fast.
*/

sumsquaredistance::sumsquaredistance()
{
}

int sumsquaredistance::calculatessd(string img, string imgpath) {
	Mat img_1 = imread(img, 1);
	if (!img_1.data)
	{
		printf(" --(!) Error reading images \n"); return -1;
	}

	vector<pair<string, double>> fn2;
	String path(imgpath); 
	vector<String> fn;
	Mat img_4;
	glob(path, fn, true); // recurse
	for (size_t k = 0; k<fn.size(); ++k)
	{
		printf("\r%d/%d", (k + 1), fn.size());
		img_4 = imread(fn[k]);
		if (img_4.empty()) continue; //only proceed if sucsessful
		double ssd = 0;
		double ssd1 = 0;
		double ssd2 = 0;
		for (int i = 0; i < img_1.cols; i++)
		{
			for (int j = 0; j < img_1.rows; j++)
			{
				Vec3b colour1 = img_1.at<Vec3b>(Point(i, j));
				Vec3b colour2 = img_4.at<Vec3b>(Point(i, j));
				double diff0 = (colour1[0] - colour2[0]);
				double diff1 = (colour1[1] - colour2[1]);
				double diff2 = (colour1[2] - colour2[2]);
				ssd += (diff0 * diff0);
				ssd1 += (diff1 * diff1);
				ssd2 += (diff2 * diff2);
			}
		}
		pair<string, double> temp = { fn[k], (ssd + ssd1 + ssd2) };
		fn2.push_back(temp);		
	}
	printf(" Done! \n");

	//imshow("Compared image", img_3);
	
	std::sort(fn2.begin(), fn2.end(), [](auto &left, auto &right) {
		return left.second < right.second;
	});

	for (size_t k = 0; k < fn2.size(); ++k)
		cout << fn2[k].first << " : " << fn2[k].second << endl;
	
	waitKey(0);
	return 0;
}

