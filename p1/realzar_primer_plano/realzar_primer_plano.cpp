/*!
  Esto es un esqueleto de programa para usar en las prácticas
  de Visión Artificial.

  Se supone que se utilizará OpenCV.

  Para compilar, puedes ejecutar:
    g++ -Wall -o esqueleto esqueleto.cc `pkg-config opencv --cflags --libs`

*/

#include <iostream>
#include <exception>
#include <sstream>

//Includes para OpenCV, Descomentar según los módulo utilizados.
#include <opencv2/core/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/calib3d/calib3d.hpp>

#include "common_code.hpp"

const cv::String keys =
    "{help h usage ? |        | Print this message}"
    "{r              |        | Use a rectangle (-r=x,y,widht,height)}"
    "{c              |        | Use a circle (-c=x,y,radius)}"
    "{p              |        | Use a closed polygon (-p=x1,y1,x2,y2,x3,y3,...)}"
    "{i              |        | Interactive mode. Follow this flag with the flag shape to use (i.e. -i -c)}"
    "{@input         | <none> | input image.}"
    "{@output        | <none> | output image.}"
    ;


struct AppState {
    cv::Mat in;
    cv::Mat mask;
    cv::Mat out;
    int mask_type;
    std::vector<cv::Point> points;
};

void
on_mouse(int event, int x, int y, int flags, void * app_state_)
{
    AppState * app_state (static_cast<AppState*>(app_state_));

    if (event == cv::EVENT_LBUTTONDOWN){

        app_state->points.push_back(cv::Point(x, y));

        if (app_state->mask_type == 1 && app_state->points.size() == 2){

            int min_x, min_y, max_x, max_y;
            if (app_state->points[0].x < app_state->points[1].x){
                min_x = app_state->points[0].x;
                max_x = app_state->points[1].x;
            } else {
                min_x = app_state->points[1].x;
                max_x = app_state->points[0].x;
            }

            if (app_state->points[0].y < app_state->points[1].y){
                min_y = app_state->points[0].y;
                max_y = app_state->points[1].y;
            } else {
                min_y = app_state->points[1].y;
                max_y = app_state->points[0].y;
            }

            int rect_width = max_x - min_x;
            int rect_height = max_y - min_y;

            app_state->mask = generate_rectagle_mask(app_state->mask.cols, app_state->mask.rows, 
            min_x, min_y, rect_width, rect_height);
        
            app_state->mask = convert_gray_to_rgb(app_state->mask);

            app_state->out = combine_images(app_state->in, app_state->out, app_state->mask);
            cv::imshow("MASK", app_state->mask);
            cv::imshow("OUTPUT", app_state->out);
            app_state->points.clear();

        } else if (app_state->mask_type == 2 && app_state->points.size() == 2){
            int radius = (int)
                std::sqrt(std::pow(app_state->points[0].x - app_state->points[1].x, 2) + 
                std::pow(app_state->points[0].y - app_state->points[1].y, 2));

            app_state->mask = generate_circle_mask(app_state->mask.cols, app_state->mask.rows, 
            app_state->points[0].x, app_state->points[0].y, radius);
        
            app_state->mask = convert_gray_to_rgb(app_state->mask);

            app_state->out = combine_images(app_state->in, app_state->out, app_state->mask);
            cv::imshow("MASK", app_state->mask);
            cv::imshow("OUTPUT", app_state->out);
            app_state->points.clear();
        }
    }
}

int
main (int argc, char* const* argv)
{
    int retCode=EXIT_SUCCESS;

    try {
        cv::CommandLineParser parser(argc, argv, keys);
        parser.about("Do a foreground enhance using a ROI.");
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

        cv::Mat in = cv::imread(input_n, cv::IMREAD_UNCHANGED);
        if (in.empty())
        {
            std::cerr << "Error: could not open input image '" << input_n
                      << "'." << std::endl;
            return EXIT_FAILURE;
        }
        cv::Mat mask = in.clone();
        cv::Mat out = in.clone();

        //TODO

        AppState app_state;
        app_state.mask_type = 1;

        out = convert_rgb_to_gray(out);
        out = convert_gray_to_rgb(out);

        std::vector<int> parameter_values;
        std::string token;

        cv::namedWindow("INPUT", cv::WINDOW_GUI_EXPANDED);
        cv::namedWindow("MASK",  cv::WINDOW_GUI_EXPANDED);
        cv::namedWindow("OUTPUT",  cv::WINDOW_GUI_EXPANDED);

        if(parser.has("i")){
            app_state.in = in.clone();
            app_state.mask = mask.clone();
            app_state.out = out.clone();
           
            if (parser.has("r")){
                app_state.mask_type  = 1;
            } else if (parser.has("c")){
                app_state.mask_type  = 2;
            } else if (parser.has("p")){
                app_state.mask_type  = 3;
            }


            cv::setMouseCallback("OUTPUT", on_mouse, (void *) &app_state);

        } else if(parser.has("r")){
            std::stringstream r_params (parser.get<std::string>("r"));
            while (std::getline(r_params, token, ',')){
                parameter_values.push_back(std::stoi(token));
            }

            mask = generate_rectagle_mask(mask.cols, mask.rows, parameter_values[0], 
                parameter_values[1], parameter_values[2], parameter_values[3]);
            mask = convert_gray_to_rgb(mask);

            app_state.out = combine_images(in, out, mask);

        } else if(parser.has("c")){
            std::stringstream c_params (parser.get<std::string>("c"));
            while (std::getline(c_params, token, ',')){
                parameter_values.push_back(std::stoi(token));
            }

            mask = generate_circle_mask(mask.cols, mask.rows, parameter_values[0], 
                parameter_values[1], parameter_values[2]);
            mask = convert_gray_to_rgb(mask);
            
            app_state.out = combine_images(in, out, mask);

        } else if(parser.has("p")){
            std::stringstream p_params (parser.get<std::string>("p"));
            while (std::getline(p_params, token, ',')){
                parameter_values.push_back(std::stoi(token));
            }
            std::vector<cv::Point> points;
            for (auto it = parameter_values.begin(); it != parameter_values.end(); it += 2){
                points.push_back(cv::Point(*it, *(it + 1)));
            }

            mask = generate_polygon_mask(mask.cols, mask.rows, points);
            mask = convert_gray_to_rgb(mask);

            app_state.out = combine_images(in, out, mask);
        }

        //

        cv::imshow("INPUT", in);
        cv::imshow("MASK", mask);
        cv::imshow("OUTPUT", out);
        int k = cv::waitKey(0)&0xff;
        if (k!=27)
            cv::imwrite(output_n, app_state.out);
        if (k == 13){
            if (app_state.mask_type == 3){
                mask = generate_polygon_mask(mask.cols, mask.rows, app_state.points);
                mask = convert_gray_to_rgb(mask);
                out = combine_images(in, out, mask);

                cv::imshow("MASK", mask);
                cv::imshow("OUTPUT", out);
                cv::waitKey(0)&0xff;

                cv::imwrite(output_n, app_state.out);

            } else {
                cv::imwrite(output_n, app_state.out);
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Capturada excepcion: " << e.what() << std::endl;
        retCode = EXIT_FAILURE;
    }
    return retCode;
}
