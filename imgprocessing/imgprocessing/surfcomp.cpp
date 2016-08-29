#include "surfcomp.h"
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
	Loads and compares keypoints between an image and all images and matches them to list images in order of good matches.
	Not as fast as I thought it would be.
	Done using the opencv tutorial.
	Should be merged with keypointcontrol.
*/

surfcomp::surfcomp()
{
}

int surfcomp::genfeatures(string img1, string path) {
	Mat img_1 = imread(img1, 1);

	if (!img_1.data)
	{
		printf(" --(!) Error reading images \n"); return -1;
	}

	int minHessian = 400;

	SurfFeatureDetector detector(minHessian);

	std::vector<KeyPoint> keypoints_1;

	detector.detect(img_1, keypoints_1);

	SurfDescriptorExtractor extractor;
	Mat descriptors_1, descriptors_2;
	extractor.compute(img_1, keypoints_1, descriptors_1);

	FileStorage fs2("descriptors.csv", FileStorage::READ);
	vector<String> fn;
	vector<pair<string, int>> fn2;
	Mat img_4;
	glob(path, fn, true); // recurse

	for (size_t k = 0; k<fn.size(); ++k)
	{
		printf("\r%d/%d", (k + 1), fn.size());
		string filename = fn[k].substr(fn[k].find_last_of("\\") + 1, fn[k].back());
		size_t lastindex = filename.find_last_of("."); //assumes theres a dot in the filename
		string rawname = filename.substr(0, lastindex);

		FileNode kptFileNode = fs2[rawname];
		read(kptFileNode, descriptors_2);

		//-- Step 3: Matching descriptor vectors using FLANN matcher
		FlannBasedMatcher matcher;
		std::vector< DMatch > matches;
		matcher.match(descriptors_1, descriptors_2, matches);

		double max_dist = 0; double min_dist = 100;

		//-- Quick calculation of max and min distances between keypoints
		for (int i = 0; i < descriptors_1.rows; i++)
		{
			double dist = matches[i].distance;
			if (dist < min_dist) min_dist = dist;
			if (dist > max_dist) max_dist = dist;
		}

		//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
		//-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
		//-- small)
		//-- PS.- radiusMatch can also be used here.
		std::vector< DMatch > good_matches;

		for (int i = 0; i < descriptors_1.rows; i++)
		{
			if (matches[i].distance <= max(2 * min_dist, 0.02))
			{
				good_matches.push_back(matches[i]);
			}
		}

		//cout << "Good matches:" << good_matches.size() << endl;
		pair<string, double> temp = { fn[k], good_matches.size() };
		fn2.push_back(temp);

	}
	printf("\r");
	fs2.release();

	std::sort(fn2.begin(), fn2.end(), [](auto &left, auto &right) {
		return left.second < right.second;
	});

	for (size_t k = 0; k < fn2.size(); ++k)
		cout << fn2[k].first << " : " << fn2[k].second << endl;

	return 0;
}