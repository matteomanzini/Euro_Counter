#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/videoio.hpp>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "Jin.h"
#include "Guidolin.h"

/**
 * @file Euro_Counter.cpp
 * @author Federico Guidolin
 * @date 2025-09-18
 */

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        std::cerr << "Missing arguments\n";
        return 1;
    }

    //Declare a Coin member for each coin category
    Coin EUR_001;
    Coin EUR_100;
    Coin EUR_002;
    Coin EUR_200;
    Coin EUR_010;
    Coin EUR_020;
    Coin EUR_050;

    std::filesystem::path path_dataset_images = "coin_counter/dataset/images";
    std::filesystem::path path_dataset_labels = "coin_counter/dataset/labels";

    std::map<std::string, Coin> vec_models;

    std::string file = argv[1];
    std::string video_type = ".MOV", image_type = ".jpg";

    std::string ext = file.substr(file.find_last_of("."));

    cv::Size S(S_X, S_Y);

    //Load each Coin into the vec_mdels map with a key corresponding to the value of the coin
    vec_models.insert(std::pair<std::string, Coin>("0.01",EUR_001));
    vec_models.insert(std::pair<std::string, Coin>("0.02",EUR_002));
    vec_models.insert(std::pair<std::string, Coin>("0.10",EUR_010));
    vec_models.insert(std::pair<std::string, Coin>("0.20",EUR_020));
    vec_models.insert(std::pair<std::string, Coin>("0.50",EUR_050));
    vec_models.insert(std::pair<std::string, Coin>("1.00",EUR_100));
    vec_models.insert(std::pair<std::string, Coin>("2.00",EUR_200));
    
    //setup the vec_models map 
    setup(path_dataset_images, path_dataset_labels, vec_models);

    //check the extension of the file recived by command line
    if(ext.compare(video_type) == 0)
    {
        cv::Mat img;
        cv::VideoCapture video(file);
        int count = 0;

        if (!video.isOpened()) 
        {
            std::cerr << "Error: Could not open video file." << std::endl;
            return 1;
        }

        cv::VideoWriter out("results" + file.substr(file.find_last_of("/")), cv::VideoWriter::fourcc('a', 'v', 'c', '1'), 30.0, S);

        std::vector<cv::Vec3f> circles_test;
        std::vector<CoinDetect> coins_from_labels, coins_predict;
        std::vector<cv::Rect> true_rectangles;

        double total_count = 0;

        std::vector<int> frames;
        std::string lblPath(argv[1]);
        
        //loads the frames in the labels directory corresponding to the input file
        getFrames(lblPath, frames);

        while (video.read(img))
        {
            cv::Mat img_test;

            cv::Size og(img.cols, img.rows);
            
            //each frame is resized to imporve performances and to have consistent size with the models
            cv::resize(img, img_test, S);

            std::vector<int>::iterator goodFrame;
            if(!frames.empty()) goodFrame = std::find(frames.begin(), frames.end(), count);

            //in order to increase the execution speed one in two frames are elaborated, with the exception of frames in the labels directory
            if(count % 2 == 0 || goodFrame != frames.end())
            {
                circles_test.clear();
                coins_from_labels.clear();
                coins_predict.clear();
                true_rectangles.clear();
                total_count = 0;
                
                //Hough transform and template matching are applied to the image
                lineDetection(img_test, circles_test);
                detection(img_test, circles_test,  vec_models, coins_predict, true_rectangles, total_count, true);
                
                //If the frame is found in the labels directory the coordinates and labels found in the frame are compared to the true labels
                if(goodFrame != frames.end())
                {
                    std::string filename = lblPath + "/frame_" + std::to_string(*goodFrame) + ".txt";
                    std::cout << filename << std::endl;
                    std::vector<CoinMatch> matches;
                    readCoordinates(filename, coins_from_labels);
                    setMatch(coins_predict, coins_from_labels, matches, og);
                }
            }

            //Green rectangles are assigned to recognized coins and the value is written in the top left corner of the rectangle
            for(auto rect : true_rectangles) cv::rectangle(img_test, rect, cv::Scalar(0, 255, 0), 3);

            for(auto elem : coins_predict)
            {
                std::string vl = std::to_string(elem.value).substr(0, 4);
                cv::putText(img_test, vl, cv::Point2f(elem.x - elem.r, elem.y - elem.r), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 2);
            }
            
            //The sum of the recognized coins is written in the top left corner of the image
            cv::putText(img_test, "Total: " + std::to_string(total_count).substr(0, 4), cv::Point2f(10.0, 50.0),cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,0,255), 2);

            //Shows the frames found in the labels directory
            if(goodFrame != frames.end())
            {
                cv::namedWindow("frame");
                cv::imshow("frame", img_test);
                cv::waitKey(0);

                cv::imwrite("results/frame_" + std::to_string(*goodFrame) + ".jpg", img_test);
            }

            out.write(img_test);

            count++;            
        }

        video.release();
        out.release();
        cv::destroyAllWindows();
    }
    else if(ext.compare(image_type) == 0)
    {
        cv::Mat img = cv::imread(file), img_test;

        if(img.empty())
        {
            std::cerr << "Warning, the filename is not correct!" << std::endl;
            return 1;
        }

        cv::Size og(img.cols, img.rows);

        //Each image is resized to imporve performances and to have consistent size with the models
        cv::resize(img, img_test, S);

        //Replace the folder and extension on the filename recived by command line to obtain the path to the corresponding label file
        std::string filename(argv[1]);
        std::string img_name = filename.substr(filename.find_last_of("/"));
        int pos = filename.find("images");
        if(pos != std::string::npos)
        {
            filename.replace(pos, 6, "labels");
        }
        pos = filename.rfind(".jpg");
        if(pos != std::string::npos)
        {
            filename.replace(pos, 4, ".txt");
        }

        std::vector<cv::Vec3f> circles_test;
        std::vector<CoinDetect> coins_predict, coins_from_labels;
        double total_count = 0;
        std::vector<CoinMatch> matches;
        std::vector<cv::Rect> true_rectangles;

        //Hough transform and template matching are applied to the image
        lineDetection(img_test, circles_test);
        detection(img_test, circles_test, vec_models, coins_predict, true_rectangles, total_count, false);

        //Read the coordinates from the corresponding label file
        readCoordinates(filename, coins_from_labels);
        setMatch(coins_predict, coins_from_labels, matches, og);

        //Green rectangles are assigned to recognized coins and the value is written in the top left corner of the rectangle
        for(auto rect : true_rectangles) cv::rectangle(img_test, rect, cv::Scalar(0, 255, 0), 3);

        for(auto elem : coins_predict)
        {
            std::string vl = std::to_string(elem.value).substr(0, 4);
            cv::putText(img_test, vl, cv::Point2f(elem.x - elem.r, elem.y - elem.r), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 2);
        }

        //The sum of the recognized coins is written in the top left corner of the image
        cv::putText(img_test, std::to_string(total_count).substr(0, 4), cv::Point2f(10.0, 50.0),cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,0,255), 2);

        cv::namedWindow("Final image", cv::WINDOW_AUTOSIZE);
        cv::imshow("Final image", img_test);
        cv::waitKey(0);

        cv::imwrite("results/" + img_name, img_test);
    }
    else
    {
        std::cerr << "File type not supported, use " << video_type << " or " << image_type << "\n";
        return 1;
    }

    return 0;
}
