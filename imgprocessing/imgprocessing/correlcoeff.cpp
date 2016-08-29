#include "correlcoeff.h"
#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/opencv_modules.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace std;
using namespace cv;
using namespace std::tr1;

correlcoeff::correlcoeff()
{
}

int correlcoeff::calculateccf(string img, string imgpath) {
	Mat img_1 = imread(img, 1);
	if (!img_1.data)
	{
		printf(" --(!) Error reading images \n"); return -1;
	}

	Size size(200, 200);
	Mat img_3;
	resize(img_1, img_3, size);
	vector<pair<string, double>> fn2;
	String path(imgpath);
	vector<String> fn;
	Mat img_4;
	glob(path, fn, true); // recurse
	for (size_t k = 0; k<fn.size(); ++k)
	{
		printf("\r%d/%d", (k+1), fn.size());
		img_4 = imread(fn[k]);
		if (img_4.empty()) continue; //only proceed if sucsessful
		resize(img_4, img_4, size);

		// convert data-type to "float"
		Mat im_float_1;
		img_3.convertTo(im_float_1, CV_32F);
		Mat im_float_2;
		img_4.convertTo(im_float_2, CV_32F);

		int n_pixels = im_float_1.rows * im_float_1.cols;

		// Compute mean and standard deviation of both images
		Scalar im1_Mean, im1_Std, im2_Mean, im2_Std;
		meanStdDev(im_float_1, im1_Mean, im1_Std);
		meanStdDev(im_float_2, im2_Mean, im2_Std);

		// Compute covariance and correlation coefficient
		double covar = (im_float_1 - im1_Mean).dot(im_float_2 - im2_Mean) / n_pixels;
		double correl = covar / (im1_Std[0] * im2_Std[0]);

		pair<string, double> temp = { fn[k], correl };
		fn2.push_back(temp);
	}
	printf("\r");
	//imshow("Compared image", img_3);

	std::sort(fn2.begin(), fn2.end(), [](auto &left, auto &right) {
		return left.second < right.second;
	});

	for (size_t k = 0; k < fn2.size(); ++k)
		cout << fn2[k].first << " : " << fn2[k].second << endl;

	return 0;
};
