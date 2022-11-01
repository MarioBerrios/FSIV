#include <iostream>
#include <cmath>
#include "common_code.hpp"
#include <opencv2/imgproc.hpp>

cv::Mat
fsiv_create_gaussian_filter(const int r)
{
    CV_Assert(r>0);
    cv::Mat ret_v;

    //TODO: Remenber 6*sigma is approx 99,73% of the distribution.


    float sigma = ((float)(2*r+1))/6.0;
    ret_v = cv::Mat::zeros(2*r+1, 2*r+1, CV_32F);

    for (int i = r; i >= -r; --i){
        for(int j = -r; j <= r; ++j){
            float dividendo = - (std::pow(i, 2) + std::pow(j, 2));
            float divisor = 2 * std::pow(sigma, 2);
            ret_v.at<float>(i+r, j+r) = std::exp(dividendo/divisor);
        }
    }

    cv::normalize(ret_v, ret_v, 1.0, 0.0, cv::NORM_L1);


    //
    CV_Assert(ret_v.type()==CV_32FC1);
    CV_Assert(ret_v.rows==(2*r+1) && ret_v.rows==ret_v.cols);
    CV_Assert(std::abs(1.0-cv::sum(ret_v)[0])<1.0e-6);
    return ret_v;
}

cv::Mat
fsiv_extend_image(const cv::Mat& img, const cv::Size& new_size, int ext_type)
{
    CV_Assert(img.rows<new_size.height);
    CV_Assert(img.cols<new_size.width);
    cv::Mat out;
    //TODO
    //Hint: use cv::copyMakeBorder()

    out = cv::Mat::zeros(new_size, img.type());
    //int border = (new_size.height - img.size().height)/2;
    float border_tb = (new_size.height - img.size().height)/2.0;
    float border_lr = (new_size.width - img.size().width)/2.0;
    if (ext_type == 0)
        cv::copyMakeBorder(img, out, floor(border_tb), ceil(border_tb),
            floor(border_lr), ceil(border_lr), cv::BORDER_CONSTANT, cv::Scalar(0,0,0));
    else
        cv::copyMakeBorder(img, out, floor(border_tb), ceil(border_tb),
            floor(border_lr), ceil(border_lr), cv::BORDER_WRAP);

    //
    CV_Assert(out.type()==img.type());
    CV_Assert(out.rows == new_size.height);
    CV_Assert(out.cols == new_size.width);
    return out;
}

cv::Mat
fsiv_create_sharpening_filter(const int filter_type, int r1, int r2)
{
    CV_Assert(0<=filter_type && filter_type<=2);
    CV_Assert(0<r1 && r1<r2);
    cv::Mat filter;
    //TODO
    //Remenber DoG = G[r2]-G[r1].
    //Hint: use fsiv_extend_image() to extent G[r1].

    if (filter_type == 0){
        float data_impulso[9] = { 0, 0, 0, 0, 1, 0, 0, 0, 0 };
        cv::Mat impulso = cv::Mat(3, 3, CV_32FC1, data_impulso);
        float data_lap_4[9] = { 0, 1, 0, 1, -4, 1, 0, 1, 0 };
        cv::Mat lap_4 = cv::Mat(3, 3, CV_32FC1, data_lap_4);

        filter = impulso - lap_4;

    } else if (filter_type == 1) {
        float data_impulso[9] = { 0, 0, 0, 0, 1, 0, 0, 0, 0 };
        cv::Mat impulso = cv::Mat(3, 3, CV_32FC1, data_impulso);
        float data_lap_8[9] = { 1, 1, 1, 1, -8, 1, 1, 1, 1 };
        cv::Mat lap_8 = cv::Mat(3, 3, CV_32FC1, data_lap_8);

        filter = impulso - lap_8;

    } else if (filter_type == 2){
        cv::Mat G_r1 = fsiv_create_gaussian_filter(r1);
        cv::Mat G_r2 = fsiv_create_gaussian_filter(r2);
        G_r1 = fsiv_extend_image(G_r1, G_r2.size());

        cv::Mat impulso = cv::Mat::zeros(G_r1.size(), CV_32FC1);
        impulso.at<float>(r2, r2) = 1;

        cv::Mat dog = G_r2 - G_r1;
        filter = impulso - dog;

    }

    //
    CV_Assert(filter.type()==CV_32FC1);
    CV_Assert((filter_type == 2) || (filter.rows==3 && filter.cols==3) );
    CV_Assert((filter_type != 2) || (filter.rows==(2*r2+1) &&
                                     filter.cols==(2*r2+1)));
    return filter;
}



cv::Mat
fsiv_image_sharpening(const cv::Mat& in, int filter_type, bool only_luma,
                      int r1, int r2, bool circular)
{
    CV_Assert(in.depth()==CV_8U);
    CV_Assert(0<r1 && r1<r2);
    CV_Assert(0<=filter_type && filter_type<=2);
    cv::Mat out;
    //TODO
    //Hint: use cv::filter2D.
    //Remenber: if circular, first the input image must be circular extended,
    //  and then clip the result.

    cv::Mat filter = fsiv_create_sharpening_filter(filter_type, r1, r2);

    if (circular == 1){
        cv::Size new_size = cv::Size(in.cols + filter.cols, in.rows + filter.rows);
        cv::Mat aux;
        if (only_luma){
            std::vector<cv::Mat> channels;
            cv::cvtColor(in, aux, cv::COLOR_BGR2HSV);
            cv::split(aux, channels);
            channels[2] = fsiv_extend_image(channels[2], new_size, 1);
            cv::filter2D(channels[2], channels[2], -1, filter);
            channels[2] = channels[2](cv::Rect(filter.cols/2, 
                filter.rows/2, in.cols, in.rows)).clone();
            cv::merge(channels, out);
            cv::cvtColor(out, out, cv::COLOR_HSV2BGR);

        } else {
            aux = fsiv_extend_image(in, new_size, 1);
            cv::filter2D(aux, out, -1, filter);
            out = out(cv::Rect(filter.cols/2, filter.rows/2, in.cols, in.rows)).clone();
        }
    } else {

        if (only_luma){
            std::vector<cv::Mat> channels;
            cv::cvtColor(in, out, cv::COLOR_BGR2HSV);
            cv::split(out, channels);
            cv::filter2D(channels[2], channels[2], -1, filter);
            cv::merge(channels, out);
            cv::cvtColor(out, out, cv::COLOR_HSV2BGR);
        } else {
            cv::filter2D(in, out, -1, filter);
        }
    }

    //
    CV_Assert(out.type()==in.type());
    CV_Assert(out.size()==in.size());
    return out;
}
