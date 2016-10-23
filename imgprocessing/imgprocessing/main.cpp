
#include <stdio.h>
#include <iostream>
#include "sumsquaredistance.h"
#include "correlcoeff.h"
#include "surfcomp.h"
#include "keypointcontrol.h"
#include "histcompare.h"
#include "homographymatch.h"
#include "predictimage.h"
#include <ctime>
#include <opencv2/opencv.hpp>
#include "opencv2/opencv_modules.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;

/*	
	Relevant classes:
	 - keypointcontrol - compute & store keypoints
	 - surfcomp - compare keypoints
	 - predictimage - create a predicted image and store the image difference
*/

int main()
{
	
	string imgpath;
	string filepath;
	cout << "Enter folder path" << endl;
	cin >> filepath;
	cout << "Enter image" << endl;
	cin >> imgpath;
	
	/*
	printf("Loading images \n");
	Mat added = imread("object0006_view03.png", 1);

	vector<Mat> similar;
	similar.push_back(imread("object0006_view01.png", 1));
	similar.push_back(imread("object0006_view02.png", 1));
	similar.push_back(imread("object0006_view04.png", 1));
	similar.push_back(imread("object0006_view05.png", 1));

	vector<string> names;
	names.push_back("object0006_view01.png");
	names.push_back("object0006_view02.png");
	names.push_back("object0006_view04.png");
	names.push_back("object0006_view05.png");
	*/
	printf("Starting clock \n");
	
	clock_t t = clock();

	//predictimage pi;
	//pi.predict(added,similar,names);
	
	
	//keypointcontrol kc;
	//kc.computekeypoints(filepath);
	surfcomp sc;
	sc.genfeatures(imgpath,filepath);
	

	t = clock() - t;
	cout << "Time elapsed: " << t / double(CLOCKS_PER_SEC) << "s" << endl;

	return 0;
}
