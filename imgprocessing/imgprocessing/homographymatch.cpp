#include "homographymatch.h"
#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

/*
	Crudely """""compresses""""" an image by storing its difference to another (closest match found by CBIR)
	and the homogrphy required to warp the other image to fill in the gap.
	'decompress' applies the stored info to retrieve and warp the reference image and add that to the compressed image.
	Yes, I'm well aware it's bad.
	It doesn't work for all images, but a decent example is fish2.jpg compressed using fish.jpg

	As with histcompare, compress only needs to run once before running decompress as many times as you want.
	Only stores homography for one image at a time, needs to be adapted to allow multiple.
*/

homographymatch::homographymatch()
{
}

void homographymatch::decompress(string added) {

	size_t lastindex = added.find_last_of("."); //assumes theres a dot in the filename
	string rawname = added.substr(0, lastindex);

	Mat compressed = imread(rawname + "_compressed.jpg", 1);

	Mat readhomography;
	string imgref;
	vector<Point2f> corners(4);
	Mat readimg;

	FileStorage fs2("homography.csv", FileStorage::READ);
	FileNode kptFileNode = fs2[rawname + "_homography"];
	read(kptFileNode, readhomography);

	FileNode fn = fs2[rawname + "_ref"];
	fn >> imgref;
	readimg = imread(imgref, 1);

	FileNode fn2 = fs2[rawname + "_points"];
	fn2 >> corners;
	fs2.release();

	warpPerspective(readimg, readimg, readhomography, readimg.size(), INTER_LINEAR, BORDER_CONSTANT);

	float m01 = (corners[0].y - corners[1].y) / (corners[0].x - corners[1].x);
	float m12 = (corners[1].y - corners[2].y) / (corners[1].x - corners[2].x);
	float m23 = (corners[2].y - corners[3].y) / (corners[2].x - corners[3].x);
	float m30 = (corners[3].y - corners[0].y) / (corners[3].x - corners[0].x);
	for (int y = 0; y<compressed.rows; y++)
	{
		for (int x = 0; x<compressed.cols; x++)
		{
			if ((y - corners[1].y) > m01*(x - corners[1].x) && (y - corners[2].y) > m12*(x - corners[2].x) && (y - corners[3].y) < m23*(x - corners[3].x) && (y - corners[0].y) < m30*(x - corners[0].x)) {
				compressed.at<Vec3b>(Point(x, y)) = readimg.at<Vec3b>(Point(x, y));
			}
		}
	}

	//imshow("Reconstructed", compressed);
	imwrite(rawname + "_reconstructed.jpg", compressed);
	waitKey(0);
}

void homographymatch::compress(string added, string match) {

	Mat img_object = imread(match, 1);
	Mat img_scene = imread(added, 1);

	if (!img_object.data || !img_scene.data)
	{
		std::cout << " --(!) Error reading images " << std::endl;
	}
	Size standard(img_scene.size());
	resize(img_object, img_object, standard);

	//-- Step 1: Detect the keypoints using SURF Detector
	int minHessian = 400;

	SurfFeatureDetector detector(minHessian);

	std::vector<KeyPoint> keypoints_object, keypoints_scene;

	detector.detect(img_object, keypoints_object);
	detector.detect(img_scene, keypoints_scene);

	//-- Step 2: Calculate descriptors (feature vectors)
	SurfDescriptorExtractor extractor;

	Mat descriptors_object, descriptors_scene;

	extractor.compute(img_object, keypoints_object, descriptors_object);
	extractor.compute(img_scene, keypoints_scene, descriptors_scene);

	//-- Step 3: Matching descriptor vectors using FLANN matcher
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match(descriptors_object, descriptors_scene, matches);

	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_object.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector< DMatch > good_matches;

	for (int i = 0; i < descriptors_object.rows; i++)
	{
		if (matches[i].distance < 3 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	Mat img_matches;
	drawMatches(img_object, keypoints_object, img_scene, keypoints_scene,
		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//-- Localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for (int i = 0; i < good_matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
		scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
	}

	Mat H = findHomography(obj, scene, CV_RANSAC);

	//-- Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0); obj_corners[1] = cvPoint(img_object.cols, 0);
	obj_corners[2] = cvPoint(img_object.cols, img_object.rows); obj_corners[3] = cvPoint(0, img_object.rows);
	std::vector<Point2f> scene_corners(4);

	perspectiveTransform(obj_corners, scene_corners, H);

	Size sizel(500, 500);
	warpPerspective(img_object, img_object, H, img_object.size(), INTER_LINEAR, BORDER_CONSTANT);

	float m01 = (scene_corners[0].y - scene_corners[1].y) / (scene_corners[0].x - scene_corners[1].x);
	float m12 = (scene_corners[1].y - scene_corners[2].y) / (scene_corners[1].x - scene_corners[2].x);
	float m23 = (scene_corners[2].y - scene_corners[3].y) / (scene_corners[2].x - scene_corners[3].x);
	float m30 = (scene_corners[3].y - scene_corners[0].y) / (scene_corners[3].x - scene_corners[0].x);
	for (int y = 0; y<img_object.rows; y++)
	{
		for (int x = 0; x<img_object.cols; x++)
		{
			if ((y - scene_corners[1].y) < m01*(x - scene_corners[1].x) || (y - scene_corners[2].y) < m12*(x - scene_corners[2].x) || (y - scene_corners[3].y) > m23*(x - scene_corners[3].x) || (y - scene_corners[0].y) > m30*(x - scene_corners[0].x)) {
				img_object.at<Vec3b>(Point(x, y)) = img_scene.at<Vec3b>(Point(x, y));;
			}
			else {
				img_object.at<Vec3b>(Point(x, y)) = { 0,0,0 };
			}

		}
	}

	//resize(img_object, img_object, sizel);
	//imshow("Compressed", img_object);
	string filename = added.substr(added.find_last_of("/") + 1, added.back());
	size_t lastindex = filename.find_last_of("."); //assumes theres a dot in the filename
	string rawname = filename.substr(0, lastindex);
	imwrite(rawname + "_compressed.jpg", img_object);

	FileStorage fs("homography.csv", FileStorage::WRITE);
	write(fs, rawname + "_homography", H); //requires the filename to start with a letter
	write(fs, rawname + "_ref", match);
	write(fs, rawname + "_points", scene_corners);
	fs.release();
}