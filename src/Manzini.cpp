#include "Manzini.h"

/**
 * @file Manzini.cpp
 * @author Matteo Manzini
 * @date 2025-09-18
 */

std::string getCoinType(float value)
{
    std::string coin_type;
    // defines a error rate to set-up the string value
    float error = 1e-4;
    if(fabs(value - 2) < error) coin_type = "EUR_200";
    else if(fabs(value - 1) < error) coin_type = "EUR_100";
    else if(fabs(value - 0.5) < error) coin_type = "EUR_050";
    else if(fabs(value - 0.2) < error) coin_type = "EUR_020";
    else if(fabs(value - 0.1) < error) coin_type = "EUR_010";
    else if(fabs(value - 0.05) < error) coin_type = "EUR_005";
    else if(fabs(value - 0.02) < error) coin_type = "EUR_002";
    else if(fabs(value - 0.01) < error) coin_type = "EUR_001";

    return coin_type;
}

void getRectangle(const cv::Mat& img, cv::Mat& out, cv::Vec3f circle)
{
    float coeff = S_X/img.cols;

    cv::Mat temp;
    cv::Point center(cvRound(circle[0]*coeff), cvRound(circle[1]*coeff));
    int radius = cvRound(circle[2]*coeff);

    // it generates a rectangle with dimensions slightly larger than the coin's circle
    int size = static_cast<int>(radius*2.1);

    int x = std::max(center.x - size/2, 0);
    int y = std::max(center.y - size/2, 0);
    int width = std::min(size, cvRound(img.cols*coeff) - x);
    int height = std::min(size, cvRound(img.rows*coeff) - y);

    cv::Rect rect(x, y, width, height);
    cv::resize(img, temp, cv::Size(S_X, S_Y));
    
    out = temp(rect).clone();
}

void setMatch(std::vector<CoinDetect>& coins_predict, const std::vector<CoinDetect>& coins_from_labels, std::vector<CoinMatch>& vec_matches, cv::Size og)
{
    float coeff = og.width/S_X;

    for(int i = 0; i < coins_from_labels.size(); i++)
    {
        float best_IoU = 0;
        CoinDetect tempModel;
        for(int j = 0; j < coins_predict.size(); j++){
            if(coins_predict[j].type == coins_from_labels[i].type)
            {
                // computes the resize of coins_predict due to the resize of initial image 
                float iou = IoU(coins_from_labels[i].x, coins_from_labels[i].y, coins_from_labels[i].r, static_cast<int>(coins_predict[j].x*coeff), static_cast<int>(coins_predict[j].y*coeff), static_cast<int>(coins_predict[j].r*coeff));
                if(iou > best_IoU)
                {
                    best_IoU = iou;
                    tempModel.type = coins_predict[j].type;
                    // resize of the coordinates
                    tempModel.x = static_cast<int>(coins_predict[j].x*coeff);
                    tempModel.y = static_cast<int>(coins_predict[j].y*coeff);
                    tempModel.r = static_cast<int>(coins_predict[j].r*coeff);
                    tempModel.value = coins_predict[j].value;
                }
            }
        }

        // creates a match between coins predict and coin from label file respect to the best_IoU
        CoinMatch match;
        match.coin_predict = tempModel;
        match.coin_label = coins_from_labels[i];
        match.IoU = best_IoU;
        vec_matches.push_back(match);

        std::cout << "\nCoin predict: " << tempModel.type << " -> " << tempModel.x << ", " << tempModel.y << ", " << tempModel.r << std::endl;
        std::cout << "Coin labels: " << coins_from_labels[i].type << " -> " << coins_from_labels[i].x << ", " << coins_from_labels[i].y << ", " << coins_from_labels[i].r << std::endl;
        std::cout << "IoU: " << best_IoU << std::endl;
    }
}

float areaCircle(float radius)
{
    return M_PI * radius * radius;
}

float intersectionCircles(float x1, float y1, float r1, float x2, float y2, float r2)
{
    // computes the distance between centers
    float d = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    if (d > (r1 + r2)) return 0;

    if (d <= fabs(r1 - r2)){
        float rmin = std::min(r1, r2);
        return areaCircle(rmin);
    }

    // computes the corners in the center of first and second circle respectively
    float alpha = 2 * acos(((r1*r1) + (d*d) - (r2*r2))/(2*r1*d));
    float beta = 2 * acos(((r2*r2) + (d*d) - (r1*r1))/(2*r2*d));

    // computes the area of two parts of intersecting circles
    float area1 = 0.5 * r1 * r1 * (alpha - std::sin(alpha));
    float area2 = 0.5 * r2 * r2 * (beta - std::sin(beta));

    return area1 + area2;
}

float IoU(float x1, float y1, float r1, float x2, float y2, float r2)
{
    float intersection = intersectionCircles(x1, y1, r1, x2, y2, r2);
    float uni = areaCircle(r1) + areaCircle(r2) - intersection;
    if(uni == 0) return 0;
    return intersection / uni;
}

double templateCoin(cv::Mat& img_test, cv::Mat& img_model)
{

    cv::Mat result, gray_model, gray_test;
    double bestScore = 0;

    cv::cvtColor(img_test, gray_test, cv::COLOR_BGR2GRAY);
    cv::cvtColor(img_model, gray_model, cv::COLOR_BGR2GRAY);

    cv::GaussianBlur(gray_model, gray_model, cv::Size(3, 3), 2);

    for (double angle = 0; angle < 360; angle += 45.0) 
    {
        cv::Mat rotated = rotateImage(gray_model, angle);

        // Ensure rotated template fits inside source image
        if (rotated.cols > gray_test.cols || rotated.rows > gray_test.rows) continue;

        // Template match between gray scale test image and rotated model image
        cv::matchTemplate(gray_test, rotated, result, cv::TM_CCOEFF_NORMED);

        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

        if (maxVal > bestScore) 
        {
            bestScore = maxVal;
        }
    }
    return bestScore;
}

cv::Mat rotateImage(const cv::Mat& src, double angle) 
{
    cv::Point2f center(src.cols / 2.0F, src.rows / 2.0F);

    // computes the rotation matrix respect to the parameters    
    cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
    cv::Mat dst;
    // applies the rotation matrix to the src image and computes the rotation of all src image
    cv::warpAffine(src, dst, rot, src.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0));
    return dst;
}

void detection(const cv::Mat& img_test, const std::vector<cv::Vec3f>& circles_test, std::map<std::string, Coin>& vec_models, std::vector<CoinDetect>& coins_predict, std::vector<cv::Rect>& true_rectangles, double& total_count, bool frame)
{
    for(const auto& circle : circles_test)
    {
        // definition of iterator to use in the vec_models
        std::map<std::string,Coin>::iterator it;
        cv::Mat test, res_test;
        getRectangle(img_test, test, circle);
        std::string::size_type sz;

        double best_score = 0, best_model_index = -1, score;
        std::string best_model;

        if(frame) cv::resize(test, res_test, cv::Size(test.cols*0.9, test.rows*0.9));
        
        for(it = vec_models.begin(); it != vec_models.end(); it++)
        {
            for (int rec = 0; rec < it->second.name.size(); rec++)
            {
                cv::Mat coin_model = it->second.name[rec].second;
                
                if(test.cols - coin_model.cols <= 15 && test.cols - coin_model.cols >= -5 && test.rows - coin_model.rows <= 15 && test.rows - coin_model.rows >= -5)
                {
                    if(frame)
                    {
                        score = templateCoin(res_test, coin_model);
                    } 
                    else
                    {
                        score = templateCoin(test, coin_model);
                    }                     

                    if(score > best_score){
                        best_score = score;
                        best_model = it->second.name[rec].first;
                        best_model_index = std::stod(it->first,&sz);
                    }
                }
            }
        }

        if(best_score >= 0.5 && best_model_index != -1){
            
            cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
            int radius = cvRound(circle[2]);

            int x = center.x - (radius*1);
            int y = center.y - (radius*1);
            int width = std::min(2*radius, img_test.cols - x);
            int height = std::min(2*radius, img_test.rows - y);

            cv::Rect rect(x, y, width, height);
            true_rectangles.push_back(rect);

            total_count += best_model_index;

            CoinDetect coin_predict, coin_out;
            coin_predict.x = cvRound(circle[0]);
            coin_predict.y = cvRound(circle[1]);
            coin_predict.r = cvRound(circle[2]);
            coin_predict.value = best_model_index;
            
            // returns the value of type of the coin predicted
            coin_predict.type = getCoinType(coin_predict.value);

            coins_predict.push_back(coin_predict);
        }
    }
}
