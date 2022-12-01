/**
 *  @file show_dataset.cpp
 *  (C) 2022- FJMC fjmadrid@uco.es
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <exception>

// Includes para OpenCV, Descomentar según los módulo utilizados.
#include <opencv2/core/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "dataset.hpp"

const char *keys =
    "{help h usage ? |      | print this message.   }"
    "{@dataset_file  |<none>| dataset description file.}";

static const int LEFT_ARROW = 81;
static const int RIGHT_ARROW = 83;

int main(int argc, char *const *argv)
{
    int retCode = EXIT_SUCCESS;

    try
    {

        cv::CommandLineParser parser(argc, argv, keys);
        parser.about("Load the histopathology dataset. ");
        if (parser.has("help"))
        {
            parser.printMessage();
            return 0;
        }
        std::string dataset_file = parser.get<std::string>("@dataset_file");
        if (!parser.check())
        {
            parser.printErrors();
            return 0;
        }

        /*Ahora toca que tu rellenes con lo que hay que hacer ...*/
        Dataset dataset;
        int ret_code = dataset.load(dataset_file);
        if (ret_code != 0)
        {
            std::cerr << "Error: could not load dataset. Return code is "
                      << ret_code << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << "Loaded " << dataset.num_samples() << " images using "
                  << dataset.memory_size() / (1024 * 1024) << " Mb of memory."
                  << std::endl;

        int key = 0;
        int idx = 0;
        std::string wname = "IMAGE";
        cv::namedWindow(wname, cv::WINDOW_GUI_EXPANDED);
        cv::resizeWindow(wname, cv::Size(512, 512));
        std::ostringstream img_name;
        std::cout << "Pres keys:" << std::endl
                  << "\t'<-'  Backward" << std::endl
                  << "\t'->'  Forward" << std::endl
                  << "\t'ESC' Quit." << std::endl;
        do
        {
            img_name.clear();
            img_name.str("");
            img_name << std::setw(4) << std::setfill('0')
                     << idx << ": "
                     << dataset.class_label_names()[dataset.y().at<int>(idx)];

            cv::Mat img = dataset.X().row(idx).reshape(3, 128);
            cv::resize(img, img, cv::Size(512, 512), 0, 0, cv::INTER_LANCZOS4);
            cv::putText(img, img_name.str(), cv::Point(20, 30),
                        cv::FONT_HERSHEY_SIMPLEX, 1.0,
                        cv::Scalar(0, 255, 255), 2);
            cv::imshow(wname, img);
            key = cv::waitKey(0) & 0xff;
            if (key == LEFT_ARROW)
                idx = (idx - 1 + dataset.num_samples()) % dataset.num_samples();
            else if (key == RIGHT_ARROW)
                idx = (idx + 1) % dataset.num_samples();
            else if (key != 27)
                std::cout << "Unknonw keypress code '" << key
                          << "' [Press '<-', '->', or 'ESC']." << std::endl;
        } while (key != 27);

        cv::destroyWindow(wname);
    }
    catch (std::exception &e)
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
