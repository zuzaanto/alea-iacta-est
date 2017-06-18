#pragma once
#include <opencv2/core.hpp>
#include <opencv/cvwimage.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/photo.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2\core\core.hpp"
#include <ctype.h>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	namedWindow("Interfejs", 1);
	//string path = string(PROJECT_SOURCE_DIR) + "/data/wm_lab_3/2.webm";
	VideoCapture capture("yellow_ni.mp4");

	// Obecna klatka
	Mat frame;
	Mat img_col;
	Mat img_grey;
	Mat img_bin;
	int wihe;
	Mat img_cnt;
	Mat img_little_cnt;
	int old_dice_count=0;
	int old_dots_count=0;
	int *old_dots_number = new int[5];
	int same_frame_count=0;

	// Petla do odczytu filmu
	while (true)
	{
		// Pobranie klatki
		if (!capture.read(frame)) break;
		//frame=imread("blue.jpg");
		pyrDown(frame, frame);
		//pyrDown(frame, frame);
		// Wyswietlenie
		imshow("Interfejs", frame);
		img_col = frame.clone();
		cvtColor(img_col, img_grey, CV_BGR2GRAY);
		wihe = (img_col.rows>img_col.cols ? img_col.cols : img_col.rows);
//		threshold(img_grey, img_bin, 160, 255, THRESH_BINARY);
		adaptiveThreshold(img_grey, img_bin, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 301,-30);
		img_cnt = img_bin.clone();
		floodFill(img_bin, Point(0, 0), 255);
		bitwise_not(img_bin, img_little_cnt);

		vector<vector<Point> >contours;
		findContours(img_cnt, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

		int cnt_number = 0;
		
		int *cnt_ind = new int[contours.size()];
		int *cnt_length = new int[contours.size()];
		Moments *cnt_mom = new Moments[contours.size()];
		int *cnt_cent_x = new int[contours.size()];
		int *cnt_cent_y = new int[contours.size()];
		for (int i = 0; i < contours.size(); i++) {
			if (contourArea(contours.at(i)) > 120) {
				cnt_mom[cnt_number] = moments(contours.at(i), true);
				//cnt_length[cnt_number] = arcLength(contours.at(i));
				cnt_cent_x[cnt_number] = (cnt_mom[cnt_number].m10 / cnt_mom[cnt_number].m00);
				cnt_cent_y[cnt_number] = (cnt_mom[cnt_number].m01 / cnt_mom[cnt_number].m00);
				cnt_ind[cnt_number] = i;
				drawContours(img_col, contours, i, Scalar(255, 0, 0), CV_FILLED); //-1 rysuje wszystkie kontury, Scalar-> na niebiesko
				cnt_number++;
			}
		}
		floodFill(img_little_cnt, Point(0, 0), 0);
		floodFill(img_little_cnt, Point(0, 2), 0);
		vector<vector<Point> >all_dots;

		findContours(img_little_cnt, all_dots, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
		int *dots_number = new int[all_dots.size()];
		int dots_total = 0;

		if (cnt_number == 5) {
			//std::cout << "rzucono 5 kosci" << std::endl;
			for (int j = 0; j < cnt_number; j++) {
				dots_number[j] = 0;
				vector<vector<Point> >dots;
				Rect roi = boundingRect(contours.at(cnt_ind[j]));
				Mat tmp = img_little_cnt.clone();

				findContours(tmp(roi), dots, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
				for (int i = 0; i < dots.size(); i++) {

					if (contourArea(dots.at(i)) > 5 && contourArea(dots.at(i)) < contourArea(contours.at(cnt_ind[j])) / 20) {
						drawContours(img_col(roi), dots, i, Scalar(255, 0, 255), CV_FILLED);
						dots_number[j]++;
						dots_total++;
					}
				}

				//std::cout << dots_number[j] << std::endl;
			}
			imshow("img_col", img_col);
			//gdy 5 kosci+
			if (/*cnt_number == old_dice_count &&*/dots_total==old_dots_count && dots_total!=0&&dots_number[0]!=0&& dots_number[1] != 0&& dots_number[2] != 0&& dots_number[3] != 0&& dots_number[4] != 0) {
				if (same_frame_count>=10)
				{
					
					//imshow("img_bin", img_bin);
					//imshow("bin_inv", img_little_cnt);
					imshow("img_col", img_col);
					putText(img_grey, "Liczba oczek na kosciach/number of dots on dice:",Point(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 200, 250), 1, CV_AA);
					char str[200];
					//std::cout << "Liczba oczek na kosciach/number of dots on dice:" << std::endl;
					for (int ind=0; ind < 5; ind++) {
						sprintf(str, "%d", dots_number[ind]);
						putText(img_grey, str, Point(30, 50+20*ind), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 200, 250), 1, CV_AA);
						//cout << dots_number[ind] << endl;
					}
					sprintf(str, "liczba kosci/number of dice: %d", cnt_number);
					putText(img_grey, str, Point(30, 200), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 200, 250), 1, CV_AA);
					//std::cout << "liczba kosci/number of dice" << std::endl;
					//std::cout << cnt_number << std::endl;
					sprintf(str, "suma oczek/sum of dots: %d", dots_total);
					putText(img_grey, str, Point(30, 220), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 200, 250), 1, CV_AA);
					//std::cout << "suma oczek/sum of dots" << std::endl;
					//std::cout << dots_total << std::endl;
					imshow("img_grey", img_grey);
					same_frame_count = 0;
				}
				else
				same_frame_count++;
			}
			
		}
		else {
			//imshow("img_bin", img_bin);
			imshow("img_col", img_col);
		
			if (cnt_number == old_dice_count &&dots_total == old_dots_count) {
				if (same_frame_count >= 10)
				{
					//std::cout << "To play YATZY you need 5 dice exactly!" << std::endl;
					same_frame_count = 0;
				}
				else
					same_frame_count++;
			}
		}
		old_dice_count = cnt_number;
		old_dots_count = dots_total;
		old_dots_number = dots_number;
		delete[](cnt_ind);
		delete[](cnt_length);
		delete[](cnt_mom);
		delete[](cnt_cent_x);
		delete[](cnt_cent_y);
		delete[](dots_number);

		char c = waitKey(100);

		// Jezeli wcisnieto esc - koniec
		if (c == 27) break;

	}

	//delete(old_dots_number);


	// Wczytanie

	//Mat img_col = imread("blue.jpg");
	//skalowanie
	/*pyrDown(img_col, img_col);
	pyrDown(img_col, img_col);
	Mat img_grey;
	cvtColor(img_col, img_grey, CV_BGR2GRAY);

	Mat img_bin;
	//threshold(img_grey, img_bin, 160, 255, THRESH_BINARY);
	int wihe=(img_col.rows>img_col.cols?img_col.cols:img_col.rows);
	adaptiveThreshold(img_grey, img_bin, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, wihe, -30);


	//find contours
	Mat img_cnt = img_bin.clone();
	//inverting image to look for dots:
	Mat img_little_cnt;
	bitwise_not(img_bin, img_little_cnt);
	//finding dice contours
	vector<vector<Point> >contours; 
	findContours(img_cnt, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

	int cnt_number = 0;
	int *cnt_ind = new int[contours.size()];
	int *cnt_length = new int[contours.size()];
	Moments *cnt_mom = new Moments[contours.size()];
	int *cnt_cent_x = new int[contours.size()];
	int *cnt_cent_y = new int[contours.size()];
	for (int i = 0; i < contours.size(); i++) {
		if (contourArea(contours.at(i)) > 120) {
			cnt_mom[cnt_number] = moments(contours.at(i), true);
			//cnt_length[cnt_number] = arcLength(contours.at(i));
			cnt_cent_x[cnt_number] = (cnt_mom[cnt_number].m10 / cnt_mom[cnt_number].m00);
			cnt_cent_y[cnt_number] = (cnt_mom[cnt_number].m01 / cnt_mom[cnt_number].m00);
			cnt_ind[cnt_number] = i;
			drawContours(img_col, contours, i, Scalar(255, 0, 0), CV_FILLED); //-1 rysuje wszystkie kontury, Scalar-> na niebiesko
			cnt_number++;
		}
	}
	floodFill(img_little_cnt, Point(0, 0), 0);

	vector<vector<Point> >all_dots;
	
	findContours(img_little_cnt, all_dots, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	int *dots_number = new int[all_dots.size()];
	int dots_total = 0;
	
	if (cnt_number==5){ 
		std::cout << "Liczba oczek na kosciach/number of dots on dice:" << std::endl;
		for (int j = 0; j < cnt_number; j++) {
			dots_number[j] = 0;
			vector<vector<Point> >dots;
			Rect roi = boundingRect(contours.at(cnt_ind[j]));
			Mat tmp = img_little_cnt.clone();
			
			findContours(tmp(roi), dots, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
			for (int i = 0; i < dots.size(); i++) {
			
					if (contourArea(dots.at(i)) > 6 && contourArea(dots.at(i)) < contourArea(contours.at(cnt_ind[j]))/20) {
					drawContours(img_col(roi), dots, i, Scalar(255, 0, 255), CV_FILLED);
					dots_number[j]++;
					dots_total++;
				}
			}

			std::cout << dots_number[j] << std::endl;
		}
		
		std::cout << "liczba kosci/number of dice" << std::endl;
		std::cout << cnt_number << std::endl;
		std::cout << "suma oczek/sum of dots" << std::endl;
		std::cout << dots_total << std::endl;
	}
	else
		std::cout << "To play YATZY you need 5 dice exactly!" << std::endl;





	// Wyswietl input
	imshow("img_grey", img_grey);
	imshow("img_bin", img_bin);
	imshow("bin_inv", img_little_cnt);
	//imshow("eroded", eroded);
	//imshow("dilated", dilated);
	imshow("img_col", img_col);



	waitKey(0);
	destroyAllWindows();
	// Return
	*/
try {
	delete[](old_dots_number);
}
catch (cv::Exception& e) {
	const char* err_msg = e.what();
	std::cout << "exception caught: " << err_msg << std::endl;
	}
	
	return 0;
}
