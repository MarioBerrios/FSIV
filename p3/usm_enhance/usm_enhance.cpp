/*!
  Esto es un esqueleto de programa para usar en las prácticas
  de Visión Artificial.

  Se supone que se utilizará OpenCV.

  Para compilar, puedes ejecutar:
    g++ -Wall -o esqueleto esqueleto.cc `pkg-config opencv --cflags --libs`

*/

#include <iostream>
#include <exception>

//Includes para OpenCV, Descomentar según los módulo utilizados.
#include <opencv2/core/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/calib3d/calib3d.hpp>

#include "common_code.hpp"

const cv::String keys =
    "{help h usage ? |      | print this message.}"
    "{i interactive  |      | Interactive mode.}"
    "{r radius       |1     | Window's radius. Default 1.}"
    "{g gain         |1.0   | Enhance's gain. Default 1.0}"
    "{c circular     |      | Use circular convolution.}"
    "{f filter       |0     | Filter type: 0->Box, 1->Gaussian. Default 0.}"
    "{@input         |<none>| input image.}"
    "{@output        |<none>| output image.}"
    ;


struct UserData
{
    cv::Mat in;
    cv::Mat out;
    cv::Mat mask;
    int g;
    int r;
    bool filter_type;
    bool circular;
};

void on_change_r(int v, void * user_data_)
{
    UserData * user_data = static_cast<UserData*>(user_data_);
    user_data->r = v + 1;
    user_data->in.convertTo(user_data->in, CV_32F, 1.0/255.0, 0.0);
    if (user_data->in.channels() > 1){
        std::vector<cv::Mat> channels;
        cv::cvtColor(user_data->in, user_data->out, cv::COLOR_BGR2HSV);
        cv::split(user_data->out, channels);
        channels[2] = fsiv_usm_enhance(channels[2], user_data->g, user_data->r, 
            user_data->filter_type, user_data->circular, &user_data->mask);
        cv::merge(channels, user_data->out);
        cv::cvtColor(user_data->out, user_data->out, cv::COLOR_HSV2BGR);
        
    } else {
        user_data->out = fsiv_usm_enhance(user_data->in, user_data->g, user_data->r, 
            user_data->filter_type, user_data->circular, &user_data->mask);
    }

    user_data->in.convertTo(user_data->in, CV_8U, 255.0, 0.0);
    user_data->out.convertTo(user_data->out, CV_8U, 255.0, 0.0);
    user_data->mask.convertTo(user_data->mask, CV_8U, 255.0, 0.0);

    cv::imshow("UNSHARP MASK", user_data->mask);
    cv::imshow("OUTPUT", user_data->out);
}

void on_change_g(int v, void * user_data_)
{
    UserData * user_data = static_cast<UserData*>(user_data_);
    user_data->g = v;
    user_data->in.convertTo(user_data->in, CV_32F, 1.0/255.0, 0.0);
    if (user_data->in.channels() > 1){
        std::vector<cv::Mat> channels;
        cv::cvtColor(user_data->in, user_data->out, cv::COLOR_BGR2HSV);
        cv::split(user_data->out, channels);
        channels[2] = fsiv_usm_enhance(channels[2], user_data->g, user_data->r, 
            user_data->filter_type, user_data->circular, &user_data->mask);
        cv::merge(channels, user_data->out);
        cv::cvtColor(user_data->out, user_data->out, cv::COLOR_HSV2BGR);
        
    } else {
        user_data->out = fsiv_usm_enhance(user_data->in, user_data->g, user_data->r, 
            user_data->filter_type, user_data->circular, &user_data->mask);
    }

    user_data->in.convertTo(user_data->in, CV_8U, 255.0, 0.0);
    user_data->out.convertTo(user_data->out, CV_8U, 255.0, 0.0);
    user_data->mask.convertTo(user_data->mask, CV_8U, 255.0, 0.0);

    cv::imshow("UNSHARP MASK", user_data->mask);
    cv::imshow("OUTPUT", user_data->out);
}

void on_change_f(int v, void * user_data_)
{
    UserData * user_data = static_cast<UserData*>(user_data_);
    user_data->filter_type = v;
    user_data->in.convertTo(user_data->in, CV_32F, 1.0/255.0, 0.0);
    if (user_data->in.channels() > 1){
        std::vector<cv::Mat> channels;
        cv::cvtColor(user_data->in, user_data->out, cv::COLOR_BGR2HSV);
        cv::split(user_data->out, channels);
        channels[2] = fsiv_usm_enhance(channels[2], user_data->g, user_data->r, 
            user_data->filter_type, user_data->circular, &user_data->mask);
        cv::merge(channels, user_data->out);
        cv::cvtColor(user_data->out, user_data->out, cv::COLOR_HSV2BGR);
        
    } else {
        user_data->out = fsiv_usm_enhance(user_data->in, user_data->g, user_data->r, 
            user_data->filter_type, user_data->circular, &user_data->mask);
    }

    user_data->in.convertTo(user_data->in, CV_8U, 255.0, 0.0);
    user_data->out.convertTo(user_data->out, CV_8U, 255.0, 0.0);
    user_data->mask.convertTo(user_data->mask, CV_8U, 255.0, 0.0);

    cv::imshow("UNSHARP MASK", user_data->mask);
    cv::imshow("OUTPUT", user_data->out);
}

void on_change_c(int v, void * user_data_)
{
    UserData * user_data = static_cast<UserData*>(user_data_);
    user_data->circular = v;
    user_data->in.convertTo(user_data->in, CV_32F, 1.0/255.0, 0.0);
    if (user_data->in.channels() > 1){
        std::vector<cv::Mat> channels;
        cv::cvtColor(user_data->in, user_data->out, cv::COLOR_BGR2HSV);
        cv::split(user_data->out, channels);
        channels[2] = fsiv_usm_enhance(channels[2], user_data->g, user_data->r, 
            user_data->filter_type, user_data->circular, &user_data->mask);
        cv::merge(channels, user_data->out);
        cv::cvtColor(user_data->out, user_data->out, cv::COLOR_HSV2BGR);
        
    } else {
        user_data->out = fsiv_usm_enhance(user_data->in, user_data->g, user_data->r, 
            user_data->filter_type, user_data->circular, &user_data->mask);
    }

    user_data->in.convertTo(user_data->in, CV_8U, 255.0, 0.0);
    user_data->out.convertTo(user_data->out, CV_8U, 255.0, 0.0);
    user_data->mask.convertTo(user_data->mask, CV_8U, 255.0, 0.0);

    cv::imshow("UNSHARP MASK", user_data->mask);
    cv::imshow("OUTPUT", user_data->out);
}

int
main (int argc, char* const* argv)
{
    int retCode=EXIT_SUCCESS;

    try {
        cv::CommandLineParser parser(argc, argv, keys);
        parser.about("Apply an unsharp mask enhance to an image.");
        if (parser.has("help"))
        {
            parser.printMessage();
            return EXIT_SUCCESS;
        }

        cv::String input_n = parser.get<cv::String>("@input");
        cv::String output_n = parser.get<cv::String>("@output");
        if (!parser.check())
        {
            parser.printErrors();
            return EXIT_FAILURE;
        }

        //TODO

        UserData user_data;
/* 
        user_data.g = 1.0;
        user_data.r = 1;
        user_data.filter_type=1;
        user_data.circular =false;
 */
        user_data.g = parser.get<double>("g");
        user_data.r = parser.get<int>("r");
        user_data.filter_type = parser.get<int>("f");
        user_data.circular = parser.has("c");
        
        //

        user_data.in = cv::imread(input_n, cv::IMREAD_UNCHANGED);
        user_data.out = user_data.in.clone();
        user_data.mask = user_data.in.clone();

        if (user_data.in.empty())
        {
            std::cerr << "Error: could not open input image '" << input_n
                      << "'." << std::endl;
            return EXIT_FAILURE;
        }

        cv::namedWindow("INPUT");
        cv::namedWindow("OUTPUT");
        cv::namedWindow("UNSHARP MASK");

        if (parser.has("i")){
            cv::createTrackbar("G [0 - 20]", "OUTPUT", nullptr, 20, on_change_g, &user_data);
            cv::setTrackbarPos("G [0 - 20]", "OUTPUT", user_data.g);
            cv::createTrackbar("R [1, 20]", "OUTPUT", nullptr, 19, on_change_r, &user_data);
            cv::setTrackbarPos("R [1, 20]", "OUTPUT", user_data.r);
            cv::createTrackbar("F", "OUTPUT", nullptr, 1, on_change_f, &user_data);
            cv::setTrackbarPos("F", "OUTPUT", (user_data.filter_type) ? 1 : 0);
            cv::createTrackbar("C", "OUTPUT", nullptr, 1, on_change_c, &user_data);
            cv::setTrackbarPos("C", "OUTPUT", (user_data.circular) ? 1 : 0);
        }

        user_data.in.convertTo(user_data.in, CV_32F, 1.0/255.0, 0.0);
        if (user_data.in.channels() > 1){
            std::vector<cv::Mat> channels;
            cv::cvtColor(user_data.in, user_data.out, cv::COLOR_BGR2HSV);
            cv::split(user_data.out, channels);
            channels[2] = fsiv_usm_enhance(channels[2], user_data.g, user_data.r, 
                user_data.filter_type, user_data.circular, &user_data.mask);
            cv::merge(channels, user_data.out);
            cv::cvtColor(user_data.out, user_data.out, cv::COLOR_HSV2BGR);
            
        } else {
            user_data.out = fsiv_usm_enhance(user_data.in, user_data.g, user_data.r, 
                user_data.filter_type, user_data.circular, &user_data.mask);
        }
        

        user_data.in.convertTo(user_data.in, CV_8U, 255.0, 0.0);
        user_data.out.convertTo(user_data.out, CV_8U, 255.0, 0.0);
        user_data.mask.convertTo(user_data.mask, CV_8U, 255.0, 0.0);

        cv::imshow ("INPUT", user_data.in);
        cv::imshow ("OUTPUT", user_data.out);
        cv::imshow ("UNSHARP MASK", user_data.mask);

        int k = cv::waitKey(0)&0xff;
        if (k!=27)
            cv::imwrite(output_n, user_data.out);
    }
    catch (std::exception& e)
    {
        std::cerr << "Capturada excepcion: " << e.what() << std::endl;
        retCode = EXIT_FAILURE;
    }
    return retCode;
}
