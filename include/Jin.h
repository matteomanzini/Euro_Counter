#ifndef JIN_H
#define JIN_H

/**
 * @file Jin.h
 * @author Tommaso Jin
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
#include "Manzini.h"

/**
 * This function setup the container for Coin and its value reading from the dataset
 * @param path_dataset_images is the path to dataset of template images
 * @param path_dataset_labels is the path to dataset of template images' labels
 * @param vec_models is the container of pair(coin's value, Coin)
  */
void setup(std::filesystem::path path_dataset_images,std::filesystem::path path_dataset_labels, std::map<std::string, Coin>& vec_models);

/**
 * This function detects and highlights the circle shape object in the scene based on Hough Circle algorithm
 * @param img is the scene image on which the detection is applied
 * @param circles is the vector that contains the detected circle's position
  */
void lineDetection(const cv::Mat& img, std::vector<cv::Vec3f>& circles);

/**
 * This function reads from the test label's file and store the annotation onn a vector of CoinDetect
 * @param filename is the name of test label's file
 * @param coins_from_labels is the vector that store the annotation of the label's text file
  */
void readCoordinates(const std::string& filename, std::vector<CoinDetect>& coins_from_labels);

#endif
