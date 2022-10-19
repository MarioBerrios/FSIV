#include "common_code.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

cv::Mat fsiv_color_rescaling(const cv::Mat& in, const cv::Scalar& from, const cv::Scalar& to)
{
    CV_Assert(in.type()==CV_8UC3);
    cv::Mat out;
    //TODO
    //Cuidado con dividir por cero.
    //Evita los bucles.

    cv::Scalar factor;
    cv::divide(to, from, factor);
    cv::multiply(in, factor, out);

    //
    CV_Assert(out.type()==in.type());
    CV_Assert(out.rows==in.rows && out.cols==in.cols);
    return out;
}

cv::Mat fsiv_wp_color_balance(cv::Mat const& in)
{
    CV_Assert(in.type()==CV_8UC3);
    cv::Mat out;
    //TODO
    //Sugerencia: utiliza el espacio de color GRAY para
    //saber la ilumimancia de un pixel.
    cv::Point max;
    cv::Mat aux;

    cv::cvtColor(in, aux, cv::COLOR_BGR2GRAY);
    cv::minMaxLoc(aux, NULL, NULL, NULL, &max);
    cv::Scalar color_base = in.at<cv::Vec3b>(max);
    out = fsiv_color_rescaling(in, color_base, cv::Scalar(255, 255, 255));

    //
    CV_Assert(out.type()==in.type());
    CV_Assert(out.rows==in.rows && out.cols==in.cols);
    return out;
}

cv::Mat fsiv_gw_color_balance(cv::Mat const& in)
{
    CV_Assert(in.type()==CV_8UC3);
    cv::Mat out;
    //TODO

    cv::Scalar color_base = cv::mean(in);
    out = fsiv_color_rescaling(in, color_base, cv::Scalar(128, 128, 128));

    //
    CV_Assert(out.type()==in.type());
    CV_Assert(out.rows==in.rows && out.cols==in.cols);
    return out;
}

cv::Mat fsiv_color_balance(cv::Mat const& in, float p)
{
    CV_Assert(in.type()==CV_8UC3);
    CV_Assert(0.0f<p && p<100.0f);
    cv::Mat out;
    //TODO
    //Sugerencia: utiliza el espacio de color GRAY para
    //saber la ilumimancia de un pixel.

    cv::Mat hist = cv::Mat::zeros(256, 1, CV_32FC1);
    cv::Mat aux;
    cv::cvtColor(in, aux, cv::COLOR_BGR2GRAY);

    int histSize = 256;
    float range[] = {0, 256};
    const float* histRange = {range};
    cv::calcHist(&aux, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);
    cv::normalize(hist, hist, 1.0, 0.0, cv::NORM_L1);

    for(int i = 1 ; i < hist.rows; i++){
        hist.at<float>(i) = hist.at<float>(i) + hist.at<float>(i - 1);
    }

    float maxValue = 0;
    for (int i = 0; i < hist.rows; i++){
        if (hist.at<float>(i) >= (1 - p/100)){
            maxValue = i;
            break;
        }
    }

    cv::Mat mask = aux >= maxValue;
    cv::Scalar color_base = cv::mean(in, mask);
    out = fsiv_color_rescaling(in, color_base, cv::Scalar(255, 255, 255));

    //
    CV_Assert(out.type()==in.type());
    CV_Assert(out.rows==in.rows && out.cols==in.cols);
    return out;
}
