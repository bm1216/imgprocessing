#include "histcompare.h"
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

/*
	Computes histograms for a given directory of images and allows an image to be compared against them.
	computegrams must run once before running comparegrams
	Fast af, histograms can be computed once and read fast and as often as needed.
	Done using http://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/histogram_comparison/histogram_comparison.html

	TODO:
	 - Iterate through the file to find histogram entries rather than looking for images. This prevents the case 
	   where a new image causes an error if it hasnt been included in the file.
	    - I want to store it as a red-black tree, not sure if it's possible. Easier to store as a vector instead.
*/
histcompare::histcompare()
{
}

int histcompare::comparegrams(string img, string imgpath) {
	Mat img_1 = imread(img, 1);
	if (!img_1.data)
	{
		printf(" --(!) Error reading images \n"); return -1;
	}

	cvtColor(img_1, img_1, COLOR_BGR2HSV);

	// Using 50 bins for hue and 60 for saturation
	int h_bins = 50; int s_bins = 60;
	int histSize[] = { h_bins, s_bins };

	// hue varies from 0 to 179, saturation from 0 to 255
	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 256 };

	const float* ranges[] = { h_ranges, s_ranges };

	// Use the o-th and 1-st channels
	int channels[] = { 0, 1 };

	Mat hist_base;

	calcHist(&img_1, 1, channels, Mat(), hist_base, 2, histSize, ranges, true, false);
	normalize(hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat());

	FileStorage fs2("histograms.csv", FileStorage::READ);
	vector<pair<string, double>> fn2;
	String path(imgpath);
	vector<String> fn;
	Mat img_4;
	glob(path, fn, true); // recurse

	for (size_t k = 0; k<fn.size(); ++k)
	{
		//printf("\r%d/%d", (k + 1), fn.size());
		string filename = fn[k].substr(fn[k].find_last_of("\\") + 1, fn[k].back());
		size_t lastindex = filename.find_last_of("."); //assumes theres a dot in the filename
		string rawname = filename.substr(0, lastindex);
		Mat hist_test1;

		FileNode kptFileNode = fs2["i"+rawname];
		read(kptFileNode, hist_test1);

		double result = compareHist(hist_base, hist_test1, 0);

		pair<string, double> temp = { fn[k], result };
		fn2.push_back(temp);
		
	}
	//printf("\r");

	//imshow("Compared image", img_3);

	sort(fn2.begin(), fn2.end(), [](auto &left, auto &right) {
		return left.second < right.second;
	});

	for (size_t k = fn2.size() - 5; k < fn2.size(); ++k)
		cout << fn2[k].first << " : " << fn2[k].second << endl;

	waitKey(0);
	return 0;
}

void histcompare::computegrams(string path) {

	//increasing histogram size increases execution time and file size but it improves results (I think)

	/// 8x12 seems to work okay, 50x60 is better tho
	int h_bins = 50; int s_bins = 60;
	int histSize[] = { h_bins, s_bins };

	// hue varies from 0 to 179, saturation from 0 to 255
	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 256 };

	const float* ranges[] = { h_ranges, s_ranges };

	// Use the o-th and 1-st channels
	int channels[] = { 0, 1 };

	FileStorage fs("histograms.csv", FileStorage::WRITE);
	vector<String> fn;
	glob(path, fn, true); // recurse
	for (size_t k = 0; k<fn.size(); ++k)
	{
		printf("\r%d/%d", (k + 1), fn.size());
		Mat img_1 = imread(fn[k], 1);
		if (!img_1.data)
		{
			printf(" --(!) Error reading image \n"); continue;
		}

		cvtColor(img_1, img_1, COLOR_BGR2HSV);
		Mat hist_test1;

		calcHist(&img_1, 1, channels, Mat(), hist_test1, 2, histSize, ranges, true, false);
		normalize(hist_test1, hist_test1, 0, 1, NORM_MINMAX, -1, Mat());

		string filename = fn[k].substr(fn[k].find_last_of("\\") + 1, fn[k].back());
		size_t lastindex = filename.find_last_of("."); //assumes theres a dot in the filename
		string rawname = filename.substr(0, lastindex);
		
		write(fs, "i"+rawname, hist_test1); //requires the filename to start with a letter

	}
	printf("\r");
	fs.release();
}

