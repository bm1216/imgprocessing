#include "keypointcontrol.h"
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
	Computes SIFT descriptors for each image in the given path and stores them in a file.
	Done using the opencv tutorial (which I cbf finding)
*/

keypointcontrol::keypointcontrol()
{
}

void keypointcontrol::computekeypoints(string path) {
	FileStorage fs("descriptors.csv", FileStorage::WRITE); //create file to store descriptors in
	vector<String> fn;
	glob(path, fn, true); //find all images and store in fn
	printf("Writing descriptors...\n");
	for (size_t k = 0; k<fn.size(); ++k) {
		printf("\r%d/%d", (k + 1), fn.size());
		Mat img_1 = imread(fn[k], 1);
		if (!img_1.data)
		{
			printf(" --(!) Error reading image \n"); continue;
		}
		int minHessian = 400; //need to look into what this does
		SiftFeatureDetector detector(minHessian);
		std::vector<KeyPoint> keypoints_1;
		
		//Get the file's raw name, perhaps this could be avoided
		string filename = fn[k].substr(fn[k].find_last_of("\\") + 1, fn[k].back());
		size_t lastindex = filename.find_last_of("."); //assumes theres a dot in the filename
		string rawname = filename.substr(0, lastindex);

		detector.detect(img_1, keypoints_1); //detect and store keypoints

		SiftDescriptorExtractor extractor;
		Mat descriptors_1;
		extractor.compute(img_1, keypoints_1, descriptors_1); //compute descriptors

		write(fs, rawname, descriptors_1); //requires the filename to start with a letter
		
	}
	printf(" Done!\n");
	fs.release();
}