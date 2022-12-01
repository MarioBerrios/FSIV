/**
 *  @file test_clf.cpp
 *  (C) 2022- FJMC fjmadrid@uco.es
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include <exception>

// Includes para OpenCV, Descomentar según los módulo utilizados.
#include <opencv2/core/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>

#include "common_code.hpp"


#ifndef NDEBUG
int __Debug_Level = 0;
#endif

const char *keys =
    "{help h usage ? |      | print this message   }"
    "{@dataset       |<none>| Dataset description file.}"
    "{@model         |<none>| Trained model to test.}"
#ifndef NDEBUG
    "{verbose        |0     | Set the verbose level.}"
#endif
    ;

std::vector<float>
parse_feature_parmas(const std::string &f_params)
{
    std::vector<float> feature_params;
    std::istringstream in(f_params);
    float v;
    while (in)
    {
        in >> v;
        if (in)
            feature_params.push_back(v);
    }
    return feature_params;
}

int main(int argc, char *const *argv)
{
    int retCode = EXIT_SUCCESS;

    try
    {

        cv::CommandLineParser parser(argc, argv, keys);
        parser.about("Test a classifier using the histopathology dataset.");
        if (parser.has("help"))
        {
            parser.printMessage();
            return 0;
        }

#ifndef NDEBUG
        __Debug_Level = parser.get<int>("verbose");
#endif
        std::string dataset_file = parser.get<std::string>("@dataset");
        std::string model_fname = parser.get<std::string>("@model");

        if (!parser.check())
        {
            parser.printErrors();
            return 0;
        }

        std::cout.setf(std::ios::unitbuf);
        Dataset dt;

        int ret_code = dt.load(dataset_file);
        if (ret_code != 0)
        {
            std::cerr << "Error: could not load dataset. Return code is "
                      << ret_code << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << "Loaded " << dt.num_samples() << " images using "
                  << dt.memory_size() / (1024 * 1024) << " Mb of memory."
                  << std::endl;

        cv::Mat test_images, test_labels;

        test_images = dt.X();
        test_labels = dt.y();

        std::cout << "Test partition with " << test_images.rows << " samples."
                  << std::endl;

        std::cout << std::endl;
        auto extractor = FeaturesExtractor::create(model_fname);
        std::cout << "Extracting features ... " << std::endl;
        std::cout << "Feature extractor: " << extractor->get_extractor_name()
                  << std::endl;
        std::cout << "Feature extractor params: " << extractor->get_params()
                  << std::endl;
        test_images = fsiv_extract_features(test_images, extractor);

        std::cout << "done." << std::endl;
        std::cout << "Extracted features use "
                  << ((test_images.rows * test_images.cols * test_images.elemSize()) / (1024 * 1024))
                  << " Mb. of memory." << std::endl;

        int clsf_id;
        cv::Ptr<cv::ml::StatModel> clsf = load_classifier_model(model_fname, clsf_id);

        if (clsf == nullptr || !clsf->isTrained())
        {
            std::cerr << "Error: I need a trained model!" << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << std::endl;

        if (clsf_id == 0)
        {
            std::cout << "Loaded a KNN model." << std::endl;
        }
        else if (clsf_id == 1)
        {
            std::cout << "Loaded a SVM model." << std::endl;
        }
        else if (clsf_id == 2)
        {
            std::cout << "Loaded a RTrees model." << std::endl;
        }
        else
        {
            std::cout << "Error: unknown classifier model." << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << std::endl;
        std::cout << "Testing ... ";
        cv::Mat predict_labels;
        fsiv_make_predictions(clsf, test_images, predict_labels);
        std::cout << "done." << std::endl;
        std::cout << std::endl;
        print_model_metrics(test_labels, predict_labels,
                            dt.class_label_names(), std::cout);
        std::cout << "Model size: " << compute_file_size(model_fname, 1024 * 1024)
                  << " Mb." << std::endl;
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        retCode = EXIT_FAILURE;
    }
    return retCode;
}
