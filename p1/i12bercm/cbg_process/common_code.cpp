#include "common_code.hpp"

cv::Mat
convert_image_byte_to_float(const cv::Mat& img, cv::Mat& out)
{
    CV_Assert(img.depth()==CV_8U);
    //TODO
    img.convertTo(out, CV_32F,1.0/255.0, 0.0);
    //
    CV_Assert(out.rows==img.rows && out.cols==img.cols);
    CV_Assert(out.depth()==CV_32F);
    CV_Assert(img.channels()==out.channels());
    return out;
}

cv::Mat
convert_image_float_to_byte(const cv::Mat& img, cv::Mat& out)
{
    CV_Assert(img.depth()==CV_32F);
    //TODO
    img.convertTo(out, CV_8U, 255.0, 0.0);
    //
    CV_Assert(out.rows==img.rows && out.cols==img.cols);
    CV_Assert(out.depth()==CV_8U);
    CV_Assert(img.channels()==out.channels());
    return out;
}

cv::Mat
convert_bgr_to_hsv(const cv::Mat& img, cv::Mat& out)
{
    CV_Assert(img.channels()==3);
    //TODO
    cv::cvtColor(img, out, cv::COLOR_BGR2HSV);
    //
    CV_Assert(out.channels()==3);
    return out;
}

cv::Mat
convert_hsv_to_bgr(const cv::Mat& img, cv::Mat& out)
{
    CV_Assert(img.channels()==3);
    //TODO
    cv::cvtColor(img, out, cv::COLOR_HSV2BGR);
    //
    CV_Assert(out.channels()==3);
    return out;
}

cv::Mat
cbg_process (const cv::Mat & in, cv::Mat& out,
             double contrast, double brightness, double gamma,
             bool only_luma)
{
    CV_Assert(in.depth()==CV_8U);
    //TODO
    //Recuerda: es recomendable trabajar en flotante [0,1]
    //Después deshacer el cambio a byte [0,255]

    cv::Mat aux_in;
    convert_image_byte_to_float(in, aux_in);
    if (!out.empty())
        convert_image_byte_to_float(out, out);

    if(only_luma){

        std::vector<cv::Mat> hsv_channels;
        convert_bgr_to_hsv(aux_in, aux_in);
        cv::split(aux_in, hsv_channels);

        cv::pow(hsv_channels[2], gamma, aux_in);
        hsv_channels[2] = contrast * aux_in + brightness;
        cv::merge(hsv_channels, out);
        convert_hsv_to_bgr(out, out);

    } else {
        cv::pow(aux_in, gamma, aux_in);
        out = contrast * aux_in + cv::Scalar(brightness, brightness, brightness);
    }

    convert_image_float_to_byte(out, out);

    //
    CV_Assert(out.rows==in.rows && out.cols==in.cols);
    CV_Assert(out.depth()==CV_8U);
    CV_Assert(out.channels()==in.channels());
    return out;
}
