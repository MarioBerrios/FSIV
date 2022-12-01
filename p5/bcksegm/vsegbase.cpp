/* 
   (c) Fundamentos de Sistemas Inteligenties en Vision
   University of Cordoba, Spain
*/

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
//#include <unistd.h>
#include <ctype.h>
#include <cmath>


#include <opencv2/core/utility.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

#include "common_code.hpp"


/*
  Use CMake to compile
*/

const cv::String keys =
        "{help h usage ? |      | print this message   }"
        "{t threshold    |13    | Segmentation threshold.}"
        "{s              |0     | Radius of structural element. 0 means not remove.}"
        "{g              |0     | Radius of the gaussian kernel. 0 means not average.}"
        "{c              |      | Use this camera idx.}"
        "{v              |      | Use this video file.}"
        "{@output        |<none>| Path to output video.}"
        ;

typedef struct {
    int thr;
    int r;
    int g;
} AppState;

void
on_change_threshold(int v, void* app_state_)
{
    AppState* app_state = static_cast<AppState*>(app_state_);
    app_state->thr = v; 
}

void
on_change_radius(int v, void* app_state_)
{
    AppState* app_state = static_cast<AppState*>(app_state_);
    app_state->r = v; 
}

void
on_change_gauss(int v, void* app_state_)
{
    AppState* app_state = static_cast<AppState*>(app_state_);
    app_state->g = v; 
}

int
main (int argc, char * const argv[])
{

    cv::CommandLineParser parser(argc, argv, keys);
    parser.about("Background segmentation in video.");
    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }
    cv::VideoCapture input;
    bool is_camera = false;
    if (parser.has("c"))
    {
        input.open(parser.get<int>("c"));
        is_camera=true;
    }
    else if (parser.has("v"))
    {
        input.open(parser.get<std::string>("v"));
    }
    else
    {
        std::cerr << "Error: Wrong CLI. one of 'v' or 'c' options must be specified."
                  << std::endl;
        return EXIT_FAILURE;
    }
    if (! input.isOpened())
    {
        std::cerr << "Error: could not open the input video stream."
                  << std::endl;
        return EXIT_FAILURE;
    }

    cv::Mat prev_frame;
    bool was_Ok = input.read(prev_frame);
    if (!was_Ok)
    {
        std::cerr << "Error: could not read any image from the input stream.\n";
        return EXIT_FAILURE;
    }

    std::string output_path = parser.get<std::string>("@output");
    int threshold = parser.get<int>("threshold");
    int ste_radius = parser.get<int>("s");
    int g_radius = parser.get<int>("g");

    AppState app_state;
    app_state.thr = threshold;
    app_state.r = ste_radius;
    app_state.g = g_radius;

    cv::VideoWriter output;
    double fps=25.0; //Default value for live video.
    //TODO
    //Open the output video stream.
    //If the input is a video file, get the fps from it.

    if(is_camera){
        output = cv::VideoWriter(output_path, input.get(cv::CAP_PROP_FOURCC), fps, prev_frame.size());
    } else {
        fps = input.get(cv::CAP_PROP_FPS);
        output = cv::VideoWriter(output_path, input.get(cv::CAP_PROP_FOURCC), 
            fps, prev_frame.size());
    }

    //

    if (!output.isOpened())
    {
        std::cerr << "Error: could not open the output stream ["
                  << output_path << "]." << std::endl;
        return EXIT_FAILURE;
    }

    cv::namedWindow("Input");
    cv::namedWindow("Masked Input");
    cv::namedWindow("Output");
    //TODO
    //Trackbars to window Output to control the parameters.

    cv::createTrackbar("T", "Output", NULL, 255, on_change_threshold, &app_state);
    cv::setTrackbarPos("T", "Output", app_state.thr);
    cv::createTrackbar("R", "Output", NULL, 30, on_change_radius, &app_state);
    cv::setTrackbarPos("R", "Output", app_state.r);
    cv::createTrackbar("G", "Output", NULL, 30, on_change_gauss, &app_state);
    cv::setTrackbarPos("G", "Output", app_state.g);

    //

    cv::Mat curr_frame, mask, masked_frame;
    was_Ok = input.read(curr_frame);
    int key = 0;
    while(was_Ok && key!=27)
    {
        //TODO
        //Process the input stream until achiving the last frame or press "ESC" key.
        //First get the curr frame and used the prev one to compute the mask.
        //Try adjust waitKey's time to approximate the fps of the stream.

        
        cv::imshow("Input", curr_frame);

        cv::GaussianBlur(prev_frame, prev_frame, 
            cv::Size(2*app_state.g + 1, 2*app_state.g + 1), 0.0);
        cv::Mat aux;
        cv::GaussianBlur(curr_frame, aux, 
            cv::Size(2*app_state.g + 1, 2*app_state.g + 1), 0.0);

        fsiv_segm_by_dif(prev_frame, aux, mask, app_state.thr, app_state.r);
        masked_frame = cv::Mat();
        fsiv_apply_mask(curr_frame, mask, masked_frame);

        cv::imshow("Masked Input", masked_frame);
        cv::imshow("Output", mask);

        output.write(masked_frame);
        prev_frame = curr_frame.clone();
        was_Ok = input.read(curr_frame);

        key = cv::waitKey(1000/fps);

        //
    }

    return EXIT_SUCCESS;
}
