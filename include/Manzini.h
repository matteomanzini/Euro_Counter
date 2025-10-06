#ifndef MANZINI_H
#define MANZINI_H

/**
 * @file Manzini.h
 * @author Matteo Manzini
 * @date 2025-09-18
 */

#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

constexpr float S_X = 720.0;
constexpr float S_Y = 960.0;

/**
 * @brief Coin is defined by radius and its name.
 * @param name is the vector of a pair between the name and the rectangle of the image model.
 */
struct Coin{
    std::vector<std::pair<std::string,cv::Mat>> name;
    float r = -1;
};

/** 
 * @brief CoinDetect represents a point 2D in cartesian coordinates with type and value.
 */
struct CoinDetect{
    std::string type;
    float x = -1;
    float y = -1;
    float r = -1;
    float value;
};

/**
 * @brief CoinMatch defines the match between coin detected and the respective coin from label with their IoU.
 */
struct CoinMatch{
    CoinDetect coin_predict;
    CoinDetect coin_label;
    float IoU;
};

/**
 * Returns the type of the coin with respect to its value.
 * @param value is the value of coin.
 * @return the string of the type of coin.
 */
std::string getCoinType(float value);

/**
 * Computes the sub-matrix of original image based on the circle.
 * @param img is the initial image.
 * @param out is the rectangle of coin based on circle.
 * @param circle cointains the coordinates of the circle, like center and radius.
 */
void getRectangle(const cv::Mat& img, cv::Mat& out, cv::Vec3f circle);

/**
 * Sets the matches between predicted coins and the file of labels respect to the max IoU.
 * @param coins_predict is the vector of CoinDetect obtained from the templateCoin method.
 * @param coins_from_labels contains the coordinates of the coins from label file.
 * @param vec_matches is the vector that contains all matches between coins_predict and coins_from_labels with highest IoU:
 */
void setMatch(std::vector<CoinDetect>& coins_predict, const std::vector<CoinDetect>& coins_from_labels, std::vector<CoinMatch>& vec_matches, cv::Size og);

/**
 * Computes the match template between the gray-scale of rectangles of img_test e img_model.
 * @param img_test is the rectangle of the coin in image test.
 * @param img_model is the rectangle of the coin in image model.
 * @return the maximum value obtained from the match between the two params.
 */
double templateCoin(const cv::Mat& img_test, const cv::Mat& img_model);

/**
 * Computes the area of the circle based on the radius.
 * @param radius is the radius of the circle.
 * @return the area of the circle.
 */
float areaCircle(float radius);

/**
 * Computes the intersection of two circles.
 * @param x1 is the abscissa coordinate of first circle.
 * @param y1 is the ordinate coordinate of first circle.
 * @param r1 is the radius of first circle.
 * @param x2 is the abscissa coordinate of second circle.
 * @param y2 is the ordinate coordinate of second circle.
 * @param r2 is the radius of second circle.
 * @return the area of intersection of two circles.
 */
float intersectionCircles(float x1, float y1, float r1, float x2, float y2, float r2);

/**
 * Computes the Intersection over Union of two circles, that is the ratio between intersection area and the union area.
 * @param x1 is the abscissa coordinate of first circle.
 * @param y1 is the ordinate coordinate of first circle.
 * @param r1 is the radius of first circle.
 * @param x2 is the abscissa coordinate of second circle.
 * @param y2 is the ordinate coordinate of second circle.
 * @param r2 is the radius of second circle.
 * @return intersection over union of two circles.
 */
float IoU(float x1, float y1, float r1, float x2, float y2, float r2);

/**
 * Computes the rotations of the image based on the angle.
 * @param src is the image to rotate.
 * @param angle define the interval degree.
 * @return vec_models is the vector that contains all rotations of img_model.
 */
cv::Mat rotateImage(const cv::Mat& src, double angle);

/**
 * Computes the detection of euros in the img_test. It identifies a coin and generates the bounding box associated to a value.
 * @param img_test is the initial image to test and to detect and identify the coins.
 * @param circles_test is the vector computed by lineDetection method.
 * @param vec_models is the map of model images computed by  setup method.
 * @param coins_predict is the vector that contains the coins predicted in the img_test.
 * @param true_rectangles is the vector that contains the bounding boxes of the coins_predict.
 * @param total_count is the sum of values of coins_predict.
 * @param frame is true if img_test is taken from video file; false otherwise.
 */
void detection(const cv::Mat& img_test, const std::vector<cv::Vec3f>& circles_test, std::map<std::string, Coin>& vec_models, std::vector<CoinDetect>& coins_predict, std::vector<cv::Rect>& true_rectangles, double & total_count, bool frame);

#endif
