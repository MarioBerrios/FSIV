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
    "{help h usage ? |      | Print this message   }"
    "{verbose        |0     | Set verbose mode level.}"
    "{b bframes      |100   | Number of frames to estimate background model.}"
    "{a alpha        |0.01  | Weight to update the background model.}"
    "{k threshold    |0.05  | Segmentation threshold.}"
    "{u stup         |10    | Short time updating period. Value 0 means not update.}"
    "{U ltup         |100   | Long time updating period. Value 0 means not update.}"
    "{r              |0     | Radius to remove segmentation noise. Value 0 means not remove.}"
    "{g              |0.0   | Radius to do a previous Gaussian averaging of input frames. Value 0 means not average.}"
    "{c              |      | Use this camera idx.}"
    "{v              |      | Use this video file.}"
    "{@output        |<none>| Path to output video.}"
    ;

typedef struct {
    int r;
    int g;
    float a;
    float k;
    int period_s;
    int period_l;
    int stup;
    int ltup;
    bool verbose;
} AppState;

void
on_change_radius(int v, void* app_state_)
{   
    AppState* app_state = static_cast<AppState*>(app_state_);
    app_state->r = v; 
    if(app_state->verbose)
        std::cout << "Setting seg radius = " << v << std::endl;
}

void
on_change_gauss(int v, void* app_state_)
{
    AppState* app_state = static_cast<AppState*>(app_state_);
    app_state->g = v; 
    if(app_state->verbose)
        std::cout << "Setting Gaussian radius = " << v << std::endl;
}

void
on_change_K(int v, void *app_state_)
{
    AppState* app_state = static_cast<AppState*>(app_state_);
    app_state->k = v/100.0;
    if(app_state->verbose)
        std::cout << "Setting K = " << v/100.0 << std::endl;
}

void
on_change_alfa(int v, void *app_state_)
{
    AppState* app_state = static_cast<AppState*>(app_state_);
    app_state->a = v/100.0;
    if(app_state->verbose)
        std::cout << "Setting Alfa = " << v/100.0 << std::endl;
}

void
on_change_stup(int v, void *app_state_)
{
    AppState* app_state = static_cast<AppState*>(app_state_);
    app_state->stup = v;
    if(app_state->verbose)
        std::cout << "Setting STUP = " << v << std::endl;
}

void
on_change_ltup(int v, void *app_state_)
{
    AppState* app_state = static_cast<AppState*>(app_state_);
    app_state->ltup = v;
    if(app_state->verbose)
        std::cout << "Setting LTUP = " << v << std::endl;
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
    int verbose = parser.get<int>("verbose");
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

    cv::Mat frame;
    bool was_Ok = input.read(frame);
    if (!was_Ok)
    {
        std::cerr << "Error: could not read any image from the input stream.\n";
        return EXIT_FAILURE;
    }

    std::string output_path = parser.get<std::string>("@output");
    float K = parser.get<float>("k");
    int K_int = int(K*100.0f);
    int ste_radius = parser.get<int>("r");
    int gauss_radius = parser.get<int>("g");
    int stup = parser.get<int>("u");
    int ltup = parser.get<int>("U");
    int num_frames = parser.get<int>("b");
    float alfa = parser.get<float>("a");
    int alfa_int = alfa*100.0f;

    AppState app_state;
    app_state.r = ste_radius;
    app_state.g = gauss_radius;
    app_state.a = alfa;
    app_state.k = K;
    app_state.stup = stup;
    app_state.ltup = ltup;
    app_state.verbose = verbose;

    cv::VideoWriter output;
    double fps=25.0; //Default value for live video.
    //TODO
    //Open the output video stream.
    //If the input is a video file, get the fps from it.

    if(is_camera){
        output = cv::VideoWriter(output_path, input.get(cv::CAP_PROP_FOURCC), fps, frame.size());
    } else {
        fps = input.get(cv::CAP_PROP_FPS);
        output = cv::VideoWriter(output_path, input.get(cv::CAP_PROP_FOURCC), 
            fps, frame.size());
    }

    //

    if (!output.isOpened())
    {
        std::cerr << "Error: could not open the output stream ["
                  << output_path << "]." << std::endl;
        return EXIT_FAILURE;
    }

    cv::namedWindow("Input"); //show the current input frame.
    cv::namedWindow("Masked Input"); //show the current frame masked
    cv::namedWindow("Output"); //show the segmentation (mask).
    cv::namedWindow("Background"); //show the model (mean image.)
    //TODO
    //Add trackbars to window Output to control the parameters.

    cv::createTrackbar("R", "Output", NULL, 30, on_change_radius, &app_state);
    cv::setTrackbarPos("R", "Output", app_state.r);
    cv::createTrackbar("G", "Output", NULL, 30, on_change_gauss, &app_state);
    cv::setTrackbarPos("G", "Output", app_state.g);
    cv::createTrackbar("A [0 - 1]", "Output", NULL, 100, on_change_alfa, &app_state);
    cv::setTrackbarPos("A [0 - 1]", "Output", alfa_int);
    cv::createTrackbar("K [0 - 5]", "Output", NULL, 500, on_change_K, &app_state);
    cv::setTrackbarPos("K [0 - 5]", "Output", K_int);
    cv::createTrackbar("STUP", "Output", NULL, 100, on_change_stup, &app_state);
    cv::setTrackbarPos("STUP", "Output", stup);
    cv::createTrackbar("LTUP", "Output", NULL, 1000, on_change_ltup, &app_state);
    cv::setTrackbarPos("LTUP", "Output", ltup);

    //

    cv::Mat Mean, Variance;
    //TODO
    //First, to learn the gaussian model.

    if(!fsiv_learn_gaussian_model(input, Mean, Variance, num_frames, gauss_radius)){
        std::cerr << "Error: could not learn the Gaussian model" << std::endl;
        return EXIT_FAILURE;
    }   

    //

    cv::Mat frame_f;
    cv::Mat mask;
    cv::Mat masked_frame;
    int key = 0;
    int frame_count = num_frames;

    while(was_Ok && key!=27)
    {
        //TODO
        //Process the input stream until achiving the last frame or press "ESC" key.
        //Remember: convert captured frames to in float [0,1].
        //First get the curr frame and use the learned model to segment it (get the mask).
        //Second update the model using the frame count.
        //Third get the masked frame.
        //Fourth show the results.
        //Fifth write the mask into the output RGB stream.
        //Try adjust waitKey's time to approximate the fps of the stream.

        was_Ok = input.read(frame);
        frame.convertTo(frame_f, CV_32F, 1.0/255.0, 0.0);
        if (was_Ok){
            fsiv_segm_by_gaussian_model(frame_f, mask, Mean, Variance, app_state.k, app_state.r);
            fsiv_update_gaussian_model(frame_f, mask, frame_count, Mean, Variance, app_state.a, 
                app_state.stup, app_state.ltup);
            frame_count++;

            masked_frame = cv::Mat();
            fsiv_apply_mask(frame, mask, masked_frame);

            cv::imshow("Input", frame);
            cv::imshow("Masked Input", masked_frame);
            cv::imshow("Output", mask);
            cv::imshow("Background", Mean);
        
            output.write(mask);
        }
        key = cv::waitKey(1000/fps);
        //
    }

    return EXIT_SUCCESS;
}
