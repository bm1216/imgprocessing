
#include <stdio.h>
#include <iostream>
#include "sumsquaredistance.h"
#include "correlcoeff.h"
#include "surfcomp.h"
#include "keypointcontrol.h"
#include "histcompare.h"
#include "homographymatch.h"
#include <ctime>
#include <opencv2/opencv.hpp>
#include "opencv2/opencv_modules.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;

/*	TODO:
	Rather than storing histogram entry for each image, see if you can store them in a data structure (red black tree?)
	and store that instead (to achieve faster access and potentially lower storage requirement)

	How the fuck do you store red black trees (map)

*/

int main()
{
	clock_t t = clock();

	//histcompare hc;
	//hc.computegrams("../memes/*.png");
	//cv::Mat result = cv::imread(hc.comparegrams("../pepe.png", "../memes/*.png"), 1);

	homographymatch hm;
	hm.compress("fish2.jpg", "fish.jpg");
	hm.decompress("fish2.jpg");
	t = clock() - t;
	cout << "Time elapsed: " << t / double(CLOCKS_PER_SEC) << "s" << endl;

	return 0;
}
