#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
class predictimage
{
public:
	predictimage();
	void predict(Mat added, vector<Mat> similar, vector<String> names);
};

