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

      //

      if (!parser.check())
      {
          parser.printErrors();
          return 0;
      }

      cv::Mat input = cv::imread(input_name);

      if (input.empty())
	  {
		  std::cerr << "Error: could not open the input image '" << input_name << "'." << std::endl;
		  return EXIT_FAILURE;
	  }
      cv::Mat output = input.clone();

      //TODO


      //

      cv::namedWindow("INPUT");
      cv::namedWindow("OUTPUT");


      cv::imshow("INPUT", input);
      cv::imshow("OUTPUT", output);


      int key = cv::waitKey(0) & 0xff;

      //TODO
      //Write the result if it's asked for.

  }
  catch (std::exception& e)
  {
    std::cerr << "Capturada excepcion: " << e.what() << std::endl;
    retCode = EXIT_FAILURE;
  }
  return retCode;
}
