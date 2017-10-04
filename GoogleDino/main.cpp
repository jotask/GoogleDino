#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace cv;

const char* source_window = "jota";

Mat src;
Mat src_gray;
int thresh = 250;
int max_thresh = 255;
RNG rng( time(NULL) );

Mat result;

int area = 190;
int max_area = 800;

/// Function header
void thresh_callback(int, void*);

enum TYPE { DINOSAUR, CACTUS };

std::string getType(const cv::Rect rect);

/** @function main */
int main(int argc, char** argv)
{

	VideoCapture cap("game.flv");

	/// Create Window
	namedWindow(source_window, CV_WINDOW_AUTOSIZE);

	while (true) {

		cap.read(src);

		Mat original = src.clone();

		if (src.empty())
		{
			cap.open("game.flv");
			continue;
		}

		/// Convert image to gray and blur it
		cvtColor(src, src_gray, CV_BGR2GRAY);
		blur(src_gray, src_gray, Size(3, 3));

		createTrackbar(" Canny thresh:", source_window, &thresh, max_thresh, thresh_callback);
		createTrackbar(" Area:", source_window, &area, max_area);

		thresh_callback(0, 0);

		Mat tmp;
		hconcat(original, result, tmp);

		imshow(source_window, tmp);

		if (waitKey(30) >= 0) break;

	}

	cvDestroyWindow("jota");

	return EXIT_SUCCESS;

}

/** @function thresh_callback */
void thresh_callback(int, void*)
{
	Mat canny_output;
	std::vector<std::vector<Point> > contours;
	std::vector<Vec4i> hierarchy;

	/// Detect edges using canny
	Canny(src_gray, canny_output, thresh, thresh * 2, 3);

	/// Find contours
	findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Approximate contours to polygons + get bounding rects and circles
	std::vector<std::vector<Point> > contours_poly(contours.size());
	std::vector<Rect> boundRect(contours.size());
	std::vector<Point2f>center(contours.size());
	std::vector<float>radius(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
		minEnclosingCircle((Mat)contours_poly[i], center[i], radius[i]);
	}

	/// Draw contours
	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));

		cv::Rect r(boundRect[i]);

		if ( (r.width * r.height) < area)
			continue;

		if((r.width * r.height) == 340)
		{
			std::cout << "gameover" << std::endl;
		}
		
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		//rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);

		std::string str;
		str = getType(r);
		str = std::to_string(r.width * r.height);

		putText(drawing, str, boundRect[i].tl(), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);

	}

	result = drawing.clone();

}

std::string getType( const cv::Rect rect)
{
	int c = rng.uniform(0, 1);
	if (c == 0)
	{
		return "Cactus";
	}
	else
	{
		return "Dinosaur";
	}
}