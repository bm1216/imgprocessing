#include "predictimage.h"

/*
	Computes a predicted image by taking a block from the original and comparing it to blocks in
	given similar images, picking the closest one.
	The difference between the original and predicted image is then taken and stored.
*/

predictimage::predictimage()
{
}

void predictimage::predict(Mat added, vector<Mat> similar, vector<String> names) {
	Mat predicted(480, 640, CV_32FC3, Scalar(0, 0, 0)); //Make a template image, same size as all others
	printf("Starting iteration \n");

	int step = 32; //block size, in this case max 32
	FileStorage fs("blockplacement.csv", FileStorage::WRITE); //store reference to used blocks
	fs << "references" << "[";
	for (int i = 0; i < predicted.cols; i+=step) {
		for (int j = 0; j < predicted.rows; j += step) {
			Mat original = added(Rect(i, j, step, step));
			int closestmatch = 0;
			double minval = -1;
			
			for (int k = 0; k < similar.size(); k++) {
				Mat segment = similar[k](Rect(i, j, step, step));
				//Work out sum of squared difference
				double ssd = 0;
				for (int a = 0; a < segment.cols; a++) {
					for (int b = 0; b < segment.rows; b++) {
						Vec3b colour1 = original.at<Vec3b>(Point(a, b));
						Vec3b colour2 = segment.at<Vec3b>(Point(a, b));
						double diff0 = (colour1[0] - colour2[0]);
						double diff1 = (colour1[1] - colour2[1]);
						double diff2 = (colour1[2] - colour2[2]);
						ssd += (diff0 * diff0);
						ssd += (diff1 * diff1);
						ssd += (diff2 * diff2);
					}
				}
				if (minval < 0) { minval = ssd; closestmatch = k; }
				else if (ssd < minval) { minval = ssd; closestmatch = k; }
			}
			fs << "{:" << "Col" << i << "Row" << j << "Ref" << names[closestmatch] << "}";
			similar[closestmatch](Rect(i, j, step, step)).copyTo(predicted.rowRange(j, j+step).colRange(i, i+step));
		}
		
	}
	fs << "]";
	printf(" Done!\nSaving image\n");
	cv::imwrite("predicted.png", predicted);
	added.convertTo(added, CV_32FC3);
	Mat dif = added - predicted;
	cv::imwrite("difference.png", dif);
	//Doing this reconstructs the image
	//Mat recon = predicted + dif;
	//cv::imwrite("reconstructed.png", recon);

	fs.release();
}