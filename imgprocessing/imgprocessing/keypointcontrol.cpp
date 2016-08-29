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

keypointcontrol::keypointcontrol()
{
}

void keypointcontrol::computekeypoints(string path) {
	FileStorage fs("descriptors.csv", FileStorage::WRITE);
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
		int minHessian = 400;
		SurfFeatureDetector detector(minHessian);
		std::vector<KeyPoint> keypoints_1;

		string filename = fn[k].substr(fn[k].find_last_of("\\") + 1, fn[k].back());
		size_t lastindex = filename.find_last_of("."); //assumes theres a dot in the filename
		string rawname = filename.substr(0, lastindex);

		detector.detect(img_1, keypoints_1);

		SurfDescriptorExtractor extractor;
		Mat descriptors_1;
		extractor.compute(img_1, keypoints_1, descriptors_1);

		write(fs, rawname, descriptors_1); //requires the filename to start with a letter
		
	}
	printf("\r");
	fs.release();
}