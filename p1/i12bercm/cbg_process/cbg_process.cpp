/*!
    Esto es un esqueleto de programa para usar en las prácticas
    de Visión Artificial.

    Se supone que se utilizará OpenCV.

    Para compilar, puedes ejecutar:
    g++ -Wall -o esqueleto esqueleto.cc `pkg-config opencv --cflags --libs`

*/

#include <iostream>
#include <exception>

// Includes para OpenCV, Descomentar según los módulo utilizados.
#include <opencv2/core/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/calib3d/calib3d.hpp>

#include "common_code.hpp"

const cv::String keys =
    "{help h usage ? |      | print this message.}"
    "{i interactive  |      | Activate interactive mode.}"
    "{l luma         |      | process only \"luma\" if color image.}"
    "{c contrast     |1.0   | contrast parameter.}"
    "{b bright       |0.0   | bright parameter.}"
    "{g gamma        |1.0   | gamma parameter.}"
    "{@input         |<none>| input image.}"
    "{@output        |<none>| output image.}";

typedef struct
{
    cv::Mat in;
    cv::Mat out;
    double contrast;
    double bright;
    double gamma;
    bool luma;
} AppState;

void
on_change_contrast(int v, void* app_state_)
{
    AppState* app_state = static_cast<AppState*>(app_state_);
    app_state->contrast = ((double) v)/100.0; 
    
    app_state->out = cbg_process(app_state->in, app_state->out, app_state->contrast,
                app_state->bright, app_state->gamma, app_state->luma);

    cv::imshow("PROCESADA", app_state->out);
}

void
on_change_bright(int v, void* app_state_)
{
    AppState* app_state = static_cast<AppState*>(app_state_);
    app_state->bright = ((double) v) * 2.0 / 200.0 - 1.0; 
    
    app_state->out = cbg_process(app_state->in, app_state->out, app_state->contrast,
                app_state->bright, app_state->gamma, app_state->luma);

    cv::imshow("PROCESADA", app_state->out);
}

void
on_change_gamma(int v, void* app_state_)
{
    AppState* app_state = static_cast<AppState*>(app_state_);
    app_state->gamma = ((double) v)/100.0; 
    
    app_state->out = cbg_process(app_state->in, app_state->out, app_state->contrast,
                app_state->bright, app_state->gamma, app_state->luma);

    cv::imshow("PROCESADA", app_state->out);
}

void
on_change_luma(int v, void* app_state_)
{
    AppState* app_state = static_cast<AppState*>(app_state_);
    app_state->luma = v;
    
    app_state->out = cbg_process(app_state->in, app_state->out, app_state->contrast,
                app_state->bright, app_state->gamma, app_state->luma);

    cv::imshow("PROCESADA", app_state->out);
}

int main(int argc, char *const *argv)
{
    int retCode = EXIT_SUCCESS;

    try
    {
        cv::CommandLineParser parser(argc, argv, keys);
        parser.about("Ajust the contrast/bright/gamma parameters of an image. (ver 0.0.0)");
        if (parser.has("help"))
        {
            parser.printMessage();
            return 0;
        }

        cv::String input_name = parser.get<cv::String>(0);
        cv::String output_name = parser.get<cv::String>(1);

        if (!parser.check())
        {
            parser.printErrors();
            return 0;
        }

        cv::Mat input;
        cv::Mat output;
        cv::namedWindow("ORIGINAL");
        cv::namedWindow("PROCESADA");

        // TODO

        AppState app_state;

        app_state.in = cv::imread(input_name);
        app_state.out = app_state.in.clone();
        app_state.contrast = parser.get<double>("c");
        app_state.bright = parser.get<double>("b");
        app_state.gamma = parser.get<double>("g");
        app_state.luma = parser.has("l");

        if (parser.has("i")){

            cbg_process(app_state.in, app_state.out, app_state.contrast,
                app_state.bright, app_state.gamma, app_state.luma);
        
            cv::createTrackbar("C [0, 2]", "PROCESADA", NULL, 200, on_change_contrast, &app_state);
            cv::setTrackbarPos("C [0, 2]", "PROCESADA", app_state.contrast * 100);
            cv::createTrackbar("B [-1, 1]", "PROCESADA", NULL, 200, on_change_bright, &app_state);
            cv::setTrackbarPos("B [-1, 1]", "PROCESADA", (app_state.bright + 1) * 200 / (1 + 1));
            cv::createTrackbar("G [0, 2]", "PROCESADA", NULL, 200, on_change_gamma, &app_state);
            cv::setTrackbarPos("G [0, 2]", "PROCESADA", app_state.gamma * 100);
            cv::createTrackbar("L [0, 1]", "PROCESADA", NULL, 1, on_change_luma, &app_state);
            cv::setTrackbarPos("L [0, 1]", "PROCESADA", app_state.luma ? 1 : 0);

        } else {
            cbg_process(app_state.in, app_state.out, app_state.contrast,
                app_state.bright, app_state.gamma, app_state.luma);
        }

        cv::imshow("ORIGINAL", app_state.in);
        cv::imshow("PROCESADA", app_state.out);

        int key = cv::waitKey(0) & 0xff;

        if (key != 27)
        {
            if (!cv::imwrite(output_name, app_state.out))
            {
                std::cerr << "Error: could not save the result in file '" << output_name << "'." << std::endl;
                return EXIT_FAILURE;
            }
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Capturada excepcion: " << e.what() << std::endl;
        retCode = EXIT_FAILURE;
    }
    return retCode;
}
