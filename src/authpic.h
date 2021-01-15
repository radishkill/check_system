#ifndef AUTHPIC_H
#define AUTHPIC_H

#include <opencv2/opencv.hpp>

const double kWaveLength = 10;

double AuthPic(cv::Mat speckle_database, char *pic2, int h2, int w2);
double AuthPic(cv::Mat speckle_database, cv::Mat speckle_auth);

#endif