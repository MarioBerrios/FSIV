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
    "{help h usage ? |      | print this message   }"
    "{i interactive  |      | use interactive mode.}"
    "{p              |0     | 0<=P<=100 is the percentage of brightest points used. "
                             "Default value 0 means use the WhitPatch method. "
                             "Value 100 means use the GrayWorld method. "
                             "Other values mean scale the mean of p% brightness points.}"
    "{@input         |<none>| input image.}"
    "{@output        |<none>| output image.}"
    ;


/**
 * @brief Application State.
 * Use this structure to maintain the state of the application
 * that will be passed to the callbacks.
 */
struct UserData
{
    cv::Mat input;
    cv::Mat output;
};

/** @brief Standard mouse callback
 * Use this function an argument for cv::setMouseCallback to control the
 * mouse interaction with a window.
 *
 * @arg event says which mouse event (move, push/release a mouse button ...)
 * @arg x and
 * @arg y says where the mouse is.
 * @arg flags give some keyboard state.
 * @arg user_data allow to pass user data to the callback.
 */
void on_mouse (int event, int x, int y, int flags, void * user_data_)
{
    UserData *user_data = static_cast<UserData*>(user_data_);
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        //TODO
        //Si el usuario  pulsan con el ratón, recoger el color de ese
        //punto y re-escalar el color de la imagen de forma que el color
        //seleccionado sea el nuevo blanco.
        cv::Scalar color_base = user_data->input.at<cv::Vec3b>(y, x);
        user_data->output=fsiv_color_rescaling(user_data->input, color_base, cv::Scalar(255,255,255));

        cv::imshow("OUTPUT", user_data->output);
        //
    }
}

/** @brief Standard trackbar callback
 * Use this function an argument for cv::createTrackbar to control
 * the trackbar changes.
 *
 * @arg v give the trackbar position.
 * @arg user_data allow to pass user data to the callback.
 */
void on_change(int v, void * user_data_)
{
    UserData * user_data = static_cast<UserData*>(user_data_);

    //TODO
    //Si el usuario mueve el deslizador, en función del valor aplica
    //el balance correcto. Recuerda:
    //  v==0 significa aplicar el criterio WhitePatch
    //  v==100 sería aplicar el criterio GrayWorld.
    //  Para valores intermedios usar el nivel médio de los p% valores
    //  más brillantes para escalar a blanco puro.
    if (v == 0){
        user_data->output = fsiv_wp_color_balance(user_data->input);
    } else if (v == 100){
        user_data->output = fsiv_gw_color_balance(user_data->input);
    } else {
        user_data->output = fsiv_color_balance(user_data->input, v);
    }

    cv::imshow("OUTPUT", user_data->output);
    //
}

int
main (int argc, char* const* argv)
{
    int retCode=EXIT_SUCCESS;

    try {

        cv::CommandLineParser parser(argc, argv, keys);
        parser.about("Apply a color balance to an image.");
        if (parser.has("help"))
        {
            parser.printMessage();
            return EXIT_SUCCESS;
        }
        bool interactive_mode = parser.has("i");
        int p = parser.get<int>("p");
        if (p<0 || p>100)
        {
            std::cerr << "Error: p is out of range [0, 100]." << std::endl;
            return EXIT_FAILURE;
        }
        cv::String input_n = parser.get<cv::String>("@input");
        cv::String output_n = parser.get<cv::String>("@output");
        if (!parser.check())
        {
            parser.printErrors();
            return EXIT_FAILURE;
        }

        cv::Mat input;

        //TODO
        //Carga la imagen de entrada;

        input = cv::imread(input_n);

        //

        cv::Mat output = input.clone(); //solo para inicializar.

        cv::namedWindow("INPUT");
        cv::namedWindow("OUTPUT");

        UserData user_data;
        if (interactive_mode)
        {
            user_data.input=input;
            user_data.output=output;
            cv::setMouseCallback("INPUT", on_mouse, &user_data);
            cv::createTrackbar("P", "OUTPUT", &p, 100, on_change,
                           &user_data);
            output = fsiv_wp_color_balance(input);
        }
        else
        {
            //TODO
            //Procesa en función del valor del parámetro P.
            //Recuerda:
            //  P==0 significa aplicar el criterio WhitePatch
            //  P==100 sería aplicar el criterio GrayWorld.
            //  Para valores intermedios usar el nivel médio de los P% valores
            //  más brillantes para escalar a blanco puro.
            if (p == 0){
                output = fsiv_wp_color_balance(input);
            } else if (p == 100){
                output = fsiv_gw_color_balance(input);
            } else {
                output = fsiv_color_balance(input, p);
            }

            //
        }

        cv::imshow ("INPUT", input);
        cv::imshow("OUTPUT", output);
        int k = cv::waitKey(0)&0xff;

        if (k!=27)
        {
            //TODO
            //Almacena la imagen.
            if (!cv::imwrite(output_n, output))
            {
                std::cerr << "Error: could not save the result in file '"
                          << output_n << "'."<< std::endl;
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
