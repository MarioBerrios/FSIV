/**
 *  @file gray_levels_features.cpp
 *  (C) 2022- FJMC fjmadrid@uco.es
 */
#include <opencv2/imgproc.hpp>
#include "gray_levels_features.hpp"

static const char * name_ = "Gray levels [0,1]";

GrayLevelsFeatures::GrayLevelsFeatures()
{
    //TODO:: Set the default values to the params vector.
    type_ = FSIV_GREY_LEVELS;
    params_.resize(1);
    params_[0]=16.0; //Default resized image is 16x16
    //
}



std::string
GrayLevelsFeatures::get_extractor_name () const
{
    return std::string(name_);
}

cv::Mat
GrayLevelsFeatures::extract_features(const cv::Mat& img)
{
    CV_Assert(img.type()==CV_8UC3);
    cv::Mat feature;
    //TODO
    //Hint: use mat::converTo to normalize the input range.
    //Hint: use mat::reshape() method to get a row descriptor.
    int S = static_cast<int>(params_[0]);
    CV_Assert(S>0);
    cv::cvtColor(img, feature, cv::COLOR_BGR2GRAY);
    feature.convertTo(feature, CV_32F, 1.0/255.0);
    cv::resize(feature, feature, cv::Size(S, S));
    feature = feature.reshape(1, 1); //transform from "1 channel HxW" to "1 chanel, 1x(WxH)".
    //
    CV_Assert(feature.rows==1);
    CV_Assert(feature.type()==CV_32FC1);
    return feature;
}
