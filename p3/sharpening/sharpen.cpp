/*!
  Esto es un esqueleto de programa para usar en las prácticas
  de Visión Artificial.

  Se supone que se utilizará OpenCV.

  Para compilar, puedes ejecutar:
    g++ -Wall -o esqueleto esqueleto.cc `pkg-config opencv --cflags --libs`

*/

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <exception>

//Includes para OpenCV, Descomentar según los módulo utilizados.
#include <opencv2/core/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/calib3d/calib3d.hpp>

#include "common_code.hpp"

const char* keys =
    "{help h usage ? |      | print this message.}"
    "{i interactive  |      | Activate interactive mode.}"
    "{l luma         |      | process only \"luma\" if color image.}"
    "{f filter       |0     | filter to use.}"
    "{r1             |1     | r1 for DoG filter.}"
    "{r2             |2     | r2 for DoG filter. (0<r1<r2)}"
    "{c circular     |      | use circular convolution.}"
    "{@input         |<none>| input image.}"
    "{@output        |<none>| output image.}"
    ;

struct UserData
{
    cv::Mat input;
    cv::Mat output;
    bool luma;
    bool circular;
    int r1;
    int r2;
    int filter_type;
};

void on_change_l(int v, void * user_data_)
{
    UserData * user_data = static_cast<UserData*>(user_data_);
    user_data->luma = v;
   if (user_data->r1 < user_data->r2)
        user_data->output = fsiv_image_sharpening(user_data->input, user_data->filter_type, 
            user_data->luma, user_data->r1, user_data->r2, user_data->circular);
    cv::imshow("OUTPUT",user_data->output);
}

void on_change_f(int v, void * user_data_)
{
    UserData * user_data = static_cast<UserData*>(user_data_);
    user_data->filter_type = v;
   if (user_data->r1 < user_data->r2)
        user_data->output = fsiv_image_sharpening(user_data->input, user_data->filter_type, 
            user_data->luma, user_data->r1, user_data->r2, user_data->circular);
    cv::imshow("OUTPUT",user_data->output);
}

void on_change_r1(int v, void * user_data_)
{
    UserData * user_data = static_cast<UserData*>(user_data_);
    user_data->r1 = v+1;
   if (user_data->r1 < user_data->r2)
        user_data->output = fsiv_image_sharpening(user_data->input, user_data->filter_type, 
            user_data->luma, user_data->r1, user_data->r2, user_data->circular);
    cv::imshow("OUTPUT",user_data->output);
}

void on_change_r2(int v, void * user_data_)
{
    UserData * user_data = static_cast<UserData*>(user_data_);
    user_data->r2 = v+1;
   if (user_data->r1 < user_data->r2)
        user_data->output = fsiv_image_sharpening(user_data->input, user_data->filter_type, 
            user_data->luma, user_data->r1, user_data->r2, user_data->circular);
    cv::imshow("OUTPUT",user_data->output);
}

void on_change_c(int v, void * user_data_)
{
    UserData * user_data = static_cast<UserData*>(user_data_);
    user_data->circular = v;
    if (user_data->r1 < user_data->r2)
        user_data->output = fsiv_image_sharpening(user_data->input, user_data->filter_type, 
            user_data->luma, user_data->r1, user_data->r2, user_data->circular);
    
    cv::imshow("OUTPUT",user_data->output);
}

int
main (int argc, char* const* argv)
{
    int retCode=EXIT_SUCCESS;

    try {    

        cv::CommandLineParser parser(argc, argv, keys);
        parser.about("Enhance an image using a sharpening filter. (ver 0.0.0)");
        if (parser.has("help"))
        {
            parser.printMessage();
            return 0;
        }

        cv::String input_name = parser.get<cv::String>(0);
        cv::String output_name = parser.get<cv::String>(1);

        //TODO
        //CLI parameters.
        UserData user_data;
        user_data.circular = parser.has("c");
        user_data.luma = parser.has("l");
        user_data.r1 = parser.get<int>("r1");
        user_data.r2 = parser.get<int>("r2");
        user_data.filter_type = parser.get<int>("f");
        //

        if (!parser.check())
        {
            parser.printErrors();
            return 0;
        }

        user_data.input = cv::imread(input_name);

        if (user_data.input.empty())
        {
            std::cerr << "Error: could not open the input image '" << input_name << "'." << std::endl;
            return EXIT_FAILURE;
        }
        
        user_data.output = user_data.input.clone();
        cv::namedWindow("INPUT");
        cv::namedWindow("OUTPUT");

        //TODO

        if (parser.has("i")){
            cv::createTrackbar("Luma", "OUTPUT", nullptr, 1, on_change_l, &user_data);
            cv::setTrackbarPos("Luma", "OUTPUT", (user_data.luma)?1:0);
            cv::createTrackbar("Filter [0, 2]", "OUTPUT", nullptr, 2, on_change_f, &user_data);
            cv::setTrackbarPos("Filter [0, 2]", "OUTPUT", user_data.filter_type);
            cv::createTrackbar("R1 [1, 50]", "OUTPUT", nullptr, 49, on_change_r1, &user_data);
            cv::createTrackbar("R2 [1, 20]", "OUTPUT", nullptr, 49, on_change_r2, &user_data);
            cv::setTrackbarPos("R2 [1, 20]", "OUTPUT", user_data.r2+1);
            cv::setTrackbarPos("R1 [1, 50]", "OUTPUT", user_data.r1+1);
            cv::createTrackbar("Circ", "OUTPUT", nullptr, 1, on_change_c, &user_data);
            cv::setTrackbarPos("Circ", "OUTPUT", (user_data.circular)?1:0);
        }

        user_data.output = fsiv_image_sharpening(user_data.input, user_data.filter_type, 
            user_data.luma, user_data.r1, user_data.r2, user_data.circular);

        //


        cv::imshow("INPUT", user_data.input);
        cv::imshow("OUTPUT",user_data.output);


        int key = cv::waitKey(0) & 0xff;
        if (key!=27)
                {
                    //TODO
                    //Almacena la imagen.
                    if (!cv::imwrite(output_name, user_data.output))
                    {
                        std::cerr << "Error: could not save the result in file '"
                                << output_name << "'."<< std::endl;
                        return EXIT_FAILURE;
                    }
                    //
                }
        }
    catch (std::exception& e)
    {
        std::cerr << "Capturada excepcion: " << e.what() << std::endl;
        retCode = EXIT_FAILURE;
    }
    return retCode;
}
