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
	Done using the opencv tutorial.
*/

surfcomp::surfcomp()
{
}

int surfcomp::genfeatures(string img1, string path) {
	Mat img_1 = imread(img1, 1);

	if (!img_1.data) {
		printf(" --(!) Error reading image \n");
		cout << img1 << endl;
		return -1;
	}
	//Compute keypoints and descriptors for added image
	int minHessian = 400;

	SiftFeatureDetector detector(minHessian);

	std::vector<KeyPoint> keypoints_1;
	detector.detect(img_1, keypoints_1);

	int keypointsize = keypoints_1.size(); //A 'perfect' match would match all of these keypoints
	printf("Number of keypoints to match: %d\n", keypointsize);

	SiftDescriptorExtractor extractor;
	Mat descriptors_1, descriptors_2;
	extractor.compute(img_1, keypoints_1, descriptors_1);
	cout << "Reading descriptor file..." << endl;
	FileStorage fs2("descriptors.csv", FileStorage::READ);
	vector<String> fn;
	vector<pair<string, int>> fn2;
	Mat img_4;
	glob(path, fn, true); 
	cout << "Reading descriptors..." << endl;

	for (size_t k = 0; k<fn.size(); ++k) {
		printf("\r%d/%d", (k + 1), fn.size());
		//Uses the filenames in the given directory rather than reading the file, I should realy change this
		string filename = fn[k].substr(fn[k].find_last_of("\\") + 1, fn[k].back());
		size_t lastindex = filename.find_last_of("."); //assumes theres a dot in the filename
		string rawname = filename.substr(0, lastindex);

		FileNode kptFileNode = fs2[rawname];
		read(kptFileNode, descriptors_2);

		//Use FLANN to compare descriptors
		descriptors_1.convertTo(descriptors_1, CV_32F);
		descriptors_2.convertTo(descriptors_2, CV_32F);
		FlannBasedMatcher matcher;
		std::vector< DMatch > matches;
		matcher.match(descriptors_1, descriptors_2, matches);

		double max_dist = 0; double min_dist = 75; //small dist is stricter, 75 and below seems fine

		// Quick calculation of max and min distances between keypoints
		for (int i = 0; i < descriptors_1.rows; i++) {
			double dist = matches[i].distance;
			if (dist < min_dist) min_dist = dist;
			if (dist > max_dist) max_dist = dist;
		}

		// Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
		// or a small arbitary value ( 0.02 ) in the event that min_dist is very small)
		std::vector< DMatch > good_matches;

		for (int i = 0; i < descriptors_1.rows; i++) {
			if (matches[i].distance <= max(2 * min_dist, 0.02)) {
				good_matches.push_back(matches[i]);
			}
		}

		pair<string, double> temp = { fn[k], good_matches.size() };
		fn2.push_back(temp);

	}
	printf(" Done!\n");
	fs2.release();

	std::sort(fn2.begin(), fn2.end(), [](auto &left, auto &right) {
		return left.second > right.second;
	});
	printf("Displaying best 10 matches:\n");
	for (size_t k = 0; k < 10 ; ++k)
		cout << fn2[k].first << " : " << fn2[k].second/(float)keypointsize<< endl;

	return 0;
}