#include "Jin.h"

/**
 * @file Jin.cpp
 * @author Tommaso Jin
 * @date 2025-09-18
 */

void setup(std::filesystem::path path_dataset_images, std::filesystem::path path_dataset_labels, std::map<std::string, Coin>& vec_models)
{
    for(auto const& dir : std::filesystem::recursive_directory_iterator(path_dataset_images))
    {
        if(!dir.path().empty())
        {
            //only if it is a image file(.jpg)
            std::string path = dir.path().string();
            std::size_t found = path.find(".jpg");
            if(found != std::string::npos)
            {
                //to extract the category of that image
                int startfound = path.find("images/");
                std::string redundant = "images/";
                int endfound = path.find("/IMG");

                //store image and its name                
                cv::Mat image = cv::imread(dir.path());
                std::string filename = dir.path().stem().string();
                if(startfound != std::string::npos)
                {
                    
                    std::string cat = path.substr(startfound+redundant.length(), endfound-startfound-redundant.length());
                    
                    //fillfull only the correspondent Coin based on the category
                    //We choose to consider category and value two distinc element because the datset structure use category to classify the coin 
                    //and in the label's text, it refered to the value using EUR_XXX
                    if(cat == "1_CENT")
                    {
                        if(auto search = vec_models.find("0.01"); search != vec_models.end())
                            search->second.name.push_back(std::pair<std::string,cv::Mat>(filename,image));                        
                    }
                    if(cat == "1_EURO")
                    {
                        if(auto search = vec_models.find("1.00"); search != vec_models.end())
                            search->second.name.push_back(std::pair<std::string,cv::Mat>(filename,image));
                    }
                    if(cat == "2_CENT")
                    {
                        if(auto search = vec_models.find("0.02"); search != vec_models.end())
                            search->second.name.push_back(std::pair<std::string,cv::Mat>(filename,image));
                    }
                    if(cat == "2_EURO")
                    {
                        if(auto search = vec_models.find("2.00"); search != vec_models.end())
                            search->second.name.push_back(std::pair<std::string,cv::Mat>(filename,image));
                    }
                    if(cat == "10_CENT")
                    {
                        if(auto search = vec_models.find("0.10"); search != vec_models.end())
                            search->second.name.push_back(std::pair<std::string,cv::Mat>(filename,image));
                    }
                    if(cat == "20_CENT")
                    {
                        if(auto search = vec_models.find("0.20"); search != vec_models.end())
                            search->second.name.push_back(std::pair<std::string,cv::Mat>(filename,image));
                    }
                    if(cat == "50_CENT")
                    {
                        if(auto search = vec_models.find("0.50"); search != vec_models.end())
                            search->second.name.push_back(std::pair<std::string,cv::Mat>(filename,image));
                    }
                }
                
            }
        }
        
    }


    for(auto const& dir : std::filesystem::recursive_directory_iterator(path_dataset_labels))
    {
        if(!dir.path().empty())
        {
            //only if it is text file(.txt)            
            std::string path = dir.path().string();
            std::size_t found = path.find(".txt");
            if(found != std::string::npos)
            { 
                //to extract the category of the label's file                
                int startfound = path.find("labels/");
                std::string redundant = "labels/";
                int endfound = path.find("/IMG");

                //open the file and save the file's name                
                std::ifstream input(dir.path());
                std::string filename = dir.path().stem().string();

                //variables to store the annotations of label's file
                std::string obj;
                std::string obj_name;
                cv::Point2i center;
                char c;
                float ray;

                if(input.is_open() && startfound != std::string::npos)
                {

                    std::string cat = path.substr(startfound+redundant.length(), endfound-startfound-redundant.length());
                    
                    while(std::getline(input, obj))
                    {
                        std::istringstream iss(obj);
                        iss >> obj_name;
                        iss >> c;
                        iss >> center.x;
                        iss >> c;
                        iss >> center.y;
                        iss >> c;
                        iss >> ray;
                    }
                    
                    //find the correspondent pair value,Coin and update the memeber image with the correspondent template image
                    // and store the radius of the circle
                    if(cat == "1_CENT")
                    {
                        if(auto search = vec_models.find("0.01"); search != vec_models.end())
                        {
                            auto it = std::find_if(search->second.name.begin(), search->second.name.end(), [&](const auto &p) {return p.first == filename;});
                            
                            if(it != search->second.name.end() && !it->second.empty())
                            {
                                cv::Mat temp;

                                getRectangle(it->second, temp, cv::Vec3f(center.x,center.y,ray));
                                
                                float coeff = S_X/(float)it->second.cols;
                                
                                it->second = temp.clone();
                                
                                if(search->second.r == -1) search->second.r = ray*coeff;
                            }
                        }
                    }
                    if(cat == "1_EURO")
                    {
                        if(auto search = vec_models.find("1.00"); search != vec_models.end())
                        {
                            auto it = std::find_if(search->second.name.begin(), search->second.name.end(), [&](const auto &p) {return p.first == filename;});
                            if(it != search->second.name.end() && !it->second.empty())
                            {
                                cv::Mat temp;

                                getRectangle(it->second, temp, cv::Vec3f(center.x,center.y,ray));
                                
                                float coeff = S_X/(float)it->second.cols;
                                
                                it->second = temp.clone();
                                
                                if(search->second.r == -1) search->second.r = ray*coeff;
                            }
                        }
                    }
                    if(cat == "2_CENT")
                    {
                        if(auto search = vec_models.find("0.02"); search != vec_models.end())
                        {
                            auto it = std::find_if(search->second.name.begin(), search->second.name.end(), [&](const auto &p) {return p.first == filename;});
                            if(it != search->second.name.end() && !it->second.empty())
                            {
                                cv::Mat temp;

                                getRectangle(it->second, temp, cv::Vec3f(center.x,center.y,ray));
                                
                                float coeff = S_X/(float)it->second.cols;
                                
                                it->second = temp.clone();
                                
                                if(search->second.r == -1) search->second.r = ray*coeff;
                            }
                        }
                    }
                    if(cat == "2_EURO")
                    {
                        if(auto search = vec_models.find("2.00"); search != vec_models.end())
                        {
                            auto it = std::find_if(search->second.name.begin(), search->second.name.end(), [&](const auto &p) {return p.first == filename;});
                            if(it != search->second.name.end() && !it->second.empty())
                            {
                                cv::Mat temp;

                                getRectangle(it->second, temp, cv::Vec3f(center.x,center.y,ray));
                                
                                float coeff = S_X/(float)it->second.cols;
                                
                                it->second = temp.clone();
                                
                                if(search->second.r == -1) search->second.r = ray*coeff;
                            }
                        }
                    }
                    if(cat == "10_CENT")
                    {
                        if(auto search = vec_models.find("0.10"); search != vec_models.end())
                        {
                            auto it = std::find_if(search->second.name.begin(), search->second.name.end(), [&](const auto &p) {return p.first == filename;});
                            if(it != search->second.name.end() && !it->second.empty())
                            {
                                cv::Mat temp;

                                getRectangle(it->second, temp, cv::Vec3f(center.x,center.y,ray));
                                
                                float coeff = S_X/(float)it->second.cols;
                                
                                it->second = temp.clone();
                                
                                if(search->second.r == -1) search->second.r = ray*coeff;
                            }
                        }
                    }
                    if(cat == "20_CENT")
                    {
                        if(auto search = vec_models.find("0.20"); search != vec_models.end())
                        {
                            auto it = std::find_if(search->second.name.begin(), search->second.name.end(), [&](const auto &p) {return p.first == filename;});
                            if(it != search->second.name.end() && !it->second.empty())
                            {
                                cv::Mat temp;

                                getRectangle(it->second, temp, cv::Vec3f(center.x,center.y,ray));
                                
                                float coeff = S_X/(float)it->second.cols;
                                
                                it->second = temp.clone();
                                
                                if(search->second.r == -1) search->second.r = ray*coeff;
                            }
                        }
                    }
                    if(cat == "50_CENT")
                    {
                        if(auto search = vec_models.find("0.50"); search != vec_models.end())
                        {
                            auto it = std::find_if(search->second.name.begin(), search->second.name.end(), [&](const auto &p) {return p.first == filename;});
                            if(it != search->second.name.end() && !it->second.empty())
                            {
                                cv::Mat temp;

                                getRectangle(it->second, temp, cv::Vec3f(center.x,center.y,ray));
                                
                                float coeff = S_X/(float)it->second.cols;
                                
                                it->second = temp.clone();
                                
                                if(search->second.r == -1) search->second.r = ray*coeff;
                            }
                        }
                    }
                }
                input.close();
            }
        }
    }
}

void lineDetection(const cv::Mat& img, std::vector<cv::Vec3f>& circles)
{
    //convert to gray beacuse Hough Circle needs gray level image
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    //to eliminate noise
    cv::Mat gaussian;
    cv::GaussianBlur(gray, gaussian, cv::Size(9, 9), 2, 2);

    //parameters for detecting only specified interval of radius and distance between the centers of the detected circles
    int dp = 1; 
    int param1 = 90;
    int param2 = 45;

    cv::HoughCircles(gaussian, circles, cv::HOUGH_GRADIENT, dp, gaussian.rows/32, param1, param2);

    //highlight the detected circle
    for(int i = 0; i < circles.size(); i++)
    {
        cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
    }
}

void readCoordinates(const std::string& filename, std::vector<CoinDetect>& coins_from_labels)
{
    std::ifstream file(filename);

    std::string line;

    while(std::getline(file, line))
    {
        if(line.empty()) continue;

        std::istringstream iss(line);
        char ch;
        CoinDetect out;

        std::string tempType;
        iss >> tempType;
        if(tempType == "SUM") break;

        out.type = tempType;

        iss >> ch >> out.x >> ch >> out.y >> ch >> out.r;

        coins_from_labels.push_back(out);
    }
}
