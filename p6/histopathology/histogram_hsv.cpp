/**
 *  @file gray_levels_features.cpp
 *  (C) 2022- FJMC fjmadrid@uco.es
 */
#include <opencv2/imgproc.hpp>
#include "histogram_hsv.hpp"

static const char * name_ = "Histogram of HSV";

HistogramHSV::HistogramHSV()
{
    //TODO:: Set the default values to the params vector.
    type_ = FSIV_HISTOGRAM_HSV;
    params_.resize(1);
    params_[0]=16.0; //Default resized image is 16x16
    //
}

std::string
HistogramHSV::get_extractor_name () const
{
    return std::string(name_);
}

cv::Mat
HistogramHSV::extract_features(const cv::Mat& img)
{
    CV_Assert(img.type()==CV_8UC3);
    cv::Mat feature;
    //TODO
    //Hint: use mat::converTo to normalize the input range.
    //Hint: use mat::reshape() method to get a row descriptor.
    cv::Mat aux, hist_h, hist_s, hist_v;
    std::vector<cv::Mat> channels;

    int S = static_cast<int>(params_[0]);
    CV_Assert(S>0);

    cv::cvtColor(img, aux, cv::COLOR_BGR2HSV);
    cv::split(aux, channels);
    //cv::resize(channels[0], channels[0], cv::Size(S, S));

    int histSize = 360;
    float range[] = {0, 360};
    const float* histRange = {range};
    cv::calcHist(&channels[0], 1, 0, cv::Mat(), hist_h, 1, &histSize, &histRange);

    histSize = 100;
    float range_sv[] = {0, 100};
    const float* histRange_sv = {range_sv};
    cv::calcHist(&channels[1], 1, 0, cv::Mat(), hist_s, 1, &histSize, &histRange_sv);
    cv::calcHist(&channels[2], 1, 0, cv::Mat(), hist_v, 1, &histSize, &histRange_sv);

    feature = hist_h; //transform from "1 channel HxW" to "1 chanel, 1x(WxH)".
    feature.push_back(hist_s);
    feature.push_back(hist_v);
    feature = feature.reshape(1, 1);
    //
    CV_Assert(feature.rows==1);
    CV_Assert(feature.type()==CV_32FC1);
    return feature;
}
