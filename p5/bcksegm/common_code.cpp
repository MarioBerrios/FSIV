// ----------------------------------------
// seglib.cpp
// (c) FSIV, University of Cordoba
// ----------------------------------------

#include "common_code.hpp"
#include <opencv2/imgproc.hpp>
#include <iostream>

void fsiv_remove_segmentation_noise(cv::Mat & img, int r)
{
    CV_Assert(img.type()==CV_8UC1);
    CV_Assert(r>0);
    //TODO
    //Apply a closing+opening using a square structuring element with radius r.
    cv::Mat struct_element = 
        cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2*r+1, 2*r+1));

    cv::morphologyEx(img, img, cv::MORPH_CLOSE, struct_element);
    cv::morphologyEx(img, img, cv::MORPH_OPEN, struct_element);
    //
}

void fsiv_segm_by_dif(const cv::Mat & prevFrame, const cv::Mat & curFrame,
                      cv::Mat & difimg, int thr, int r)
{
    CV_Assert(prevFrame.type()==CV_8UC3 && prevFrame.type()==curFrame.type());
    CV_Assert(prevFrame.size()==prevFrame.size());

    //TODO
    //Remenber: use >= to compare with 'thr'.
    cv::Mat prevGray, curGray;
    cv::cvtColor(prevFrame, prevGray, cv::COLOR_RGB2GRAY);
    cv::cvtColor(curFrame, curGray, cv::COLOR_RGB2GRAY);

    cv::absdiff(prevGray, curGray, difimg);
    difimg = difimg >= thr;
    if (r > 0)
        fsiv_remove_segmentation_noise(difimg, r);
    
    //
    CV_Assert(difimg.type()==CV_8UC1);
    CV_Assert(difimg.size()==curFrame.size());
}

void fsiv_apply_mask(const cv::Mat & frame, const cv::Mat & mask,
                     cv::Mat & outframe)
{
    CV_Assert(frame.type()==CV_8UC1 || frame.type()==CV_8UC3);
    CV_Assert(mask.type()==CV_8UC1);
    CV_Assert(frame.size()==mask.size());

    //TODO
    //Remenber: frame could have 3 channels.
    
    frame.copyTo(outframe, mask);

    //
    CV_Assert(outframe.type()==frame.type());
    CV_Assert(outframe.size()==frame.size());
}

bool
fsiv_learn_gaussian_model(cv::VideoCapture & input,
                          cv::Mat & mean,
                          cv::Mat & variance,
                          int num_frames,
                          int gauss_r,
                          const char * wname)
{
    CV_Assert(input.isOpened());
    bool was_ok = true;

    //TODO
    // Remenber you can compute the variance as:
    // varI = sum_n{I^2}/n - meanI²
    // Hint: convert to input frames to float [0,1].
    // Hint: use cv::accumulate() and cv::accumulateSquare().

    for (int i = 0; i < num_frames; i++){
        cv::Mat frame;
        input >> frame;
        frame.convertTo(frame, CV_32F, 1.0/255.0, 0.0);
        if(i == 0){
            mean = cv::Mat::zeros(frame.size(), frame.type());
            variance = cv::Mat::zeros(frame.size(), frame.type());
        }
        if(gauss_r > 0)
            cv::GaussianBlur(frame, frame, cv::Size(2*gauss_r + 1, 2*gauss_r + 1), 0.0);

        cv::accumulate(frame, mean);
        cv::accumulateSquare(frame, variance);     
    }

    cv::Mat power_mean;
    cv::divide(mean, num_frames, mean);
    cv::divide(variance, num_frames, variance);
    cv::pow(mean, 2, power_mean);
    variance -= power_mean;

    //
    CV_Assert(!was_ok || mean.type()==CV_32FC3);
    CV_Assert(!was_ok || variance.type()==CV_32FC3);
    return was_ok;
}

void
fsiv_segm_by_gaussian_model(const cv::Mat & frame,
                            cv::Mat & mask,
                            const cv::Mat & mean,
                            const cv::Mat & variance, float k, int r)
{
    CV_Assert(frame.type()==CV_32FC3);

    //TODO
    //Remenber: a point belongs to the foreground (255) if |mean-I| >= k*stdev

    cv::Mat diff, stdev, mask_C3, frame_aux = frame.clone();
    std::vector<cv::Mat> channels_mask;

    if (r > 0){
        std::vector<cv::Mat> channels;
        frame_aux.convertTo(frame_aux, CV_8U, 255.0, 0.0);
        cv::split(frame_aux, channels);
        fsiv_remove_segmentation_noise(channels[0], r);
        fsiv_remove_segmentation_noise(channels[1], r);
        fsiv_remove_segmentation_noise(channels[2], r);
        cv::merge(channels, frame_aux);
        frame_aux.convertTo(frame_aux, CV_32F, 1.0/255.0, 0.0);

    }

    cv::absdiff(mean, frame_aux, diff);
    cv::sqrt(variance, stdev);

    mask_C3 = diff >= (k * stdev);
    cv::split(mask_C3, channels_mask);

    cv::bitwise_or(channels_mask[0], channels_mask[1], mask);
    cv::bitwise_or(channels_mask[2], mask, mask);

    //
    CV_Assert(mask.type()==CV_8UC1);
}

void
fsiv_update_gaussian_model(const cv::Mat & frame,
                           const cv::Mat & mask,
                           unsigned long frame_count,
                           cv::Mat & mean,
                           cv::Mat & variance,
                           float alpha,
                           unsigned short_term_update_period,
                           unsigned long_term_update_period)
{
    CV_Assert(frame.type()==CV_32FC3);
    CV_Assert(mask.type()==CV_8UC1);
    CV_Assert(mean.type()==CV_32FC3);
    CV_Assert(variance.type()==CV_32FC3);
    CV_Assert(frame.size()==mask.size());

    //TODO
    //Remember: In the short term updating you must update the model using
    //the background only (not mask).
    //However in the long term updating you must update
    //the model using both background and foreground (without mask).
    //Hint: a period is met when (idx % period) == 0
    //Hint: use accumulateWeighted to update the model.

    if(long_term_update_period != 0 && frame_count%long_term_update_period == 0){
        cv::accumulateWeighted(frame, mean, alpha);
        cv::Mat power;
        cv::pow(frame, 2, power);
        cv::accumulateWeighted(power, variance, alpha);
    } else if(short_term_update_period != 0 && frame_count%short_term_update_period == 0){
        cv::Mat notMask, power;
        cv::bitwise_not(mask, notMask);
        cv::accumulateWeighted(frame, mean, alpha, notMask);
        cv::pow(frame, 2, power);
        cv::accumulateWeighted(power, variance, alpha, notMask);
    }

    

    //
}
