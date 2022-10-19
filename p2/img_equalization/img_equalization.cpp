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
    "{r radius       |0     | radius used to local processing.}"
    "{m hold_median  |      | the histogram's median will not be transformed.}"
    "{@input         |<none>| input image.}"
    "{@output        |<none>| output image.}"
    ;

int
main (int argc, char* const* argv)
{
  int retCode=EXIT_SUCCESS;
  
  try {    

      cv::CommandLineParser parser(argc, argv, keys);
      parser.about("Apply an histogram equalization to the image. (ver 1.0.0)");
      if (parser.has("help"))
      {
          parser.printMessage();
          return 0;
      }

      cv::String input_name = parser.get<cv::String>(0);
      cv::String output_name = parser.get<cv::String>(1);
      int radius = parser.get<int>("r");
      bool hold_median = parser.has("m");

      if (!parser.check())
      {
          parser.printErrors();
          return 0;
      }

      cv::namedWindow("ORIGINAL", cv::WINDOW_GUI_EXPANDED);

      cv::Mat input = cv::imread(input_name);
      //cv::Mat input = cv::imread(input_name, cv::IMREAD_GRAYSCALE);

      if (input.empty())
      {
          std::cerr << "Error: could not open the input image '"
                    << input_name << "'." << std::endl;
          return EXIT_FAILURE;
      }

      cv::Mat output = input.clone();

      //TODO

      if (input.channels() > 1){
          cv::namedWindow("PROCESADA_RGB", cv::WINDOW_GUI_EXPANDED);
          cv::namedWindow("PROCESADA_HSV", cv::WINDOW_GUI_EXPANDED);

          std::vector<cv::Mat> channels_RGB, channels_HSV;

          cv::split(input, channels_RGB);
          fsiv_image_equalization(channels_RGB[0], channels_RGB[0], hold_median, radius);
          fsiv_image_equalization(channels_RGB[1], channels_RGB[1], hold_median, radius);
          fsiv_image_equalization(channels_RGB[2], channels_RGB[2], hold_median, radius);
          cv::merge(channels_RGB, output);
          cv::imshow("PROCESADA_RGB", output);

          output.release();

          cv::cvtColor(input, output, cv::COLOR_BGR2HSV);
          cv::split(output, channels_HSV);
          fsiv_image_equalization(channels_HSV[2], channels_HSV[2], hold_median, radius);
          cv::merge(channels_HSV, output);
          cv::cvtColor(output, output, cv::COLOR_HSV2BGR);
          cv::imshow("PROCESADA_HSV", output);

      } else {
          cv::namedWindow("PROCESADA", cv::WINDOW_GUI_EXPANDED);
          fsiv_image_equalization(input, output, hold_median, radius);
          cv::imshow("PROCESADA", output);
      }

      //

      cv::imshow("ORIGINAL", input);

      int key = cv::waitKey(0) & 0xff;

      if (key != 27)
      {
          if (!cv::imwrite(output_name, output))
          {
              std::cerr << "Error: could not save the result in file '"
                        << output_name << "'."<< std::endl;
                return EXIT_FAILURE;
            }
      }
  }
  catch (std::exception& e)
  {
    std::cerr << "Capturada excepcion: " << e.what() << std::endl;
    retCode = EXIT_FAILURE;
  }
  catch (...)
  {
    std::cerr << "Capturada excepcion desconocida!" << std::endl;
    retCode = EXIT_FAILURE;
  }
  return retCode;
}


