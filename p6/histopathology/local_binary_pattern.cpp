/**
 *  @file gray_levels_features.cpp
 *  (C) 2022- FJMC fjmadrid@uco.es
 */
#include <opencv2/imgproc.hpp>
#include "local_binary_pattern.hpp"

static const char * name_ = "Local Binary Pattern";

LocalBinaryPattern::LocalBinaryPattern()
{
    //TODO:: Set the default values to the params vector.
    type_ = FSIV_LBP;
    params_.resize(1);
    params_[0]=16.0; //Default resized image is 16x16
    //
}

std::string
LocalBinaryPattern::get_extractor_name () const
{
    return std::string(name_);
}

cv::Mat
LocalBinaryPattern::extract_features(const cv::Mat& img)
{
    CV_Assert(img.type()==CV_8UC3);
    cv::Mat feature;
    //TODO
    //Hint: use mat::converTo to normalize the input range.
    //Hint: use mat::reshape() method to get a row descriptor.
    cv::Mat aux, window;

    int S = static_cast<int>(params_[0]);
    CV_Assert(S>0);

    cv::cvtColor(img, aux, cv::COLOR_BGR2GRAY);
    aux.convertTo(aux, CV_32F, 1.0/255.0);
    //cv::resize(aux, aux, cv::Size(S, S));
    std::vector<cv::Mat> channels;

    cv::Mat hist = cv::Mat::zeros(256, 1, CV_32FC1);

    for (int i = 0; i <= aux.rows - (3); i++){
        for (int j = 0; j <= aux.cols - (3); j++){
            window = aux(cv::Rect(j, i, 3, 3)).clone();
            const float center = window.at<float>(1, 1);
            unsigned char value = 0;
            if (window.at<float>(0,0) >= center) value |= 0x80;
            if (window.at<float>(0,1) >= center) value |= 0x40;
            if (window.at<float>(0,2) >= center) value |= 0x20;
            if (window.at<float>(1,2) >= center) value |= 0x10;
            if (window.at<float>(2,2) >= center) value |= 0x08;
            if (window.at<float>(2,1) >= center) value |= 0x04;
            if (window.at<float>(2,0) >= center) value |= 0x02;
            if (window.at<float>(1,0) >= center) value |= 0x01;
            //std::cout << (window.at<float>(0,0) >= center) << std::endl;
            hist.at<float>(value) = hist.at<float>(value) + 1;      
        }
    }

    feature = hist.reshape(1, 1);

    //
    CV_Assert(feature.rows==1);
    CV_Assert(feature.type()==CV_32FC1);
    return feature;
}
