/**
 *  @file train_clf.cpp
 *  (C) 2022- FJMC fjmadrid@uco.es
 */
#include <iostream>
#include <iomanip>
#include <sstream>
#include <exception>
#include <cstdlib>

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
    "{rseed          |      | set the random seed to this value.}"
    "{f            |1     | Feature to extract. Default 0 is gray levels [0,1].}"
    "{f_params     |      | Feature parameters (if any). Format <value>:<value>:<value>... }"
    "{v validate   |0     | Use the training (100-v)%/ v% validation partitions to train "
    "and validate. Default is 0% means not validate.}"
    "{clf          |0     | Classifier to train/test. 0: K-NN, 1:SVM, 2:RTREES.}"
    "{knn_K        |1     | Parameter K for K-NN class.}"
    "{svm_C        |1.0   | Parámeter C for SVM class.}"
    "{svm_K        |0     | Kernel to use with SVM class. 0:Linear, 1:Polinomial. "
    "2:RBF, 3:SIGMOID, 4:CHI2, 5:INTER}"
    "{svm_D        |3.0   | Degree of svm polinomial kernel.}"
    "{svm_G        |1.0   | Gamma for svm RBF kernel.}"
    "{rtrees_V     |0     | Num of random features sampled per node. "
    "Default 0 meas sqrt(num. of total features).}"
    "{rtrees_T     |50    | Max num. of rtrees in the forest.}"
    "{rtrees_E     |0.1   | OOB error to stop adding more rtrees.}"
    "{@dataset_file|<none>| Dataset description file.}"
    "{@model      |<none>| model filename to save the trained data.}"
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
        parser.about("Train a classifier using the histopathology dataset.");
        if (parser.has("help"))
        {
            parser.printMessage();
            return 0;
        }

#ifndef NDEBUG
        __Debug_Level = parser.get<int>("verbose");
#endif
        cv::uint64_t rseed = cv::getTickCount();
        if (parser.has("rseed"))
            rseed = parser.get<cv::uint64_t>("rseed");
        std::cout << "Using random seed = " << rseed << std::endl;
        srand(rseed);
        FEATURE_IDS feature_id = FEATURE_IDS(parser.get<int>("f"));
        std::vector<float> feature_params;
        if (parser.has("f_params"))
            feature_params = parse_feature_parmas(parser.get<std::string>("f_params"));
        float validate = parser.get<float>("v");

        std::string dataset_file = parser.get<std::string>("@dataset_file");
        std::string model_fname = parser.get<std::string>("@model");
        int classifier = parser.get<int>("clf");
        int knn_K = parser.get<int>("knn_K");
        float svm_C = parser.get<float>("svm_C");
        int svm_K = parser.get<int>("svm_K");
        float svm_D = parser.get<float>("svm_D");
        float svm_G = parser.get<float>("svm_G");
        int rtrees_V = parser.get<int>("rtrees_V");
        int rtrees_T = parser.get<int>("rtrees_T");
        double rtrees_E = parser.get<double>("rtrees_E");
        if (!parser.check())
        {
            parser.printErrors();
            return 0;
        }

        std::cout.setf(std::ios::unitbuf);

        Dataset dt;

        int ret_code = dt.load(dataset_file, rseed);
        if (ret_code != 0)
        {
            std::cerr << "Error: could not load dataset. Return code is "
                      << ret_code << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << "Loaded " << dt.num_samples() << " images using "
                  << dt.memory_size() / (1024 * 1024) << " Mb of memory."
                  << std::endl;

        cv::Mat train_images, train_labels, validate_images, validate_labels;

        if (validate > 0.0)
        {
            std::cout << "Split dataset in two partitions: " << (100.0 - validate) << "% train, "
                      << validate << "% validation." << std::endl;
            dt.split(100.0 - validate, train_images, train_labels,
                     validate_images, validate_labels);
        }
        else
        {
            train_images = dt.X();
            train_labels = dt.y();
        }

        std::cout << "Train partition with " << train_images.rows << " samples."
                  << std::endl;

        if (validate > 0)
            std::cout << "Validation partition with "
                      << (validate_images.empty() ? 0 : validate_images.rows)
                      << " samples." << std::endl;

        std::cout << std::endl;

        auto extractor = FeaturesExtractor::create(feature_id);
        extractor->set_params(feature_params);

        std::cout << "Feature extractor: " << extractor->get_extractor_name()
                  << std::endl;
        std::cout << "Feature extractor params: " << extractor->get_params()
                  << std::endl;
        std::cout << "Training feature extractor ... " << std::endl;
        extractor->train(train_images);
        std::cout << "Done." << std::endl;
        std::cout << "Extracting features ... " << std::endl;
        train_images = fsiv_extract_features(train_images, extractor);
        if (!validate_images.empty())
            validate_images = fsiv_extract_features(validate_images,
                                                    extractor);
        std::cout << "done." << std::endl;
        std::cout << "Extracted features use " << ((train_images.rows * train_images.cols * train_images.elemSize()) + (validate_images.empty() ? 0 : ((validate_images.rows * validate_images.cols * validate_images.elemSize())))) / (1024 * 1024)
                  << " Mb. of memory." << std::endl;

        std::cout << std::endl;
        std::cout << "Training ... " << std::endl;
        cv::Ptr<cv::ml::StatModel> clsf;
        int train_flags = 0;

        if (classifier == 0)
        {
            std::cout << "Using a K-NN classifier with k=" << knn_K << std::endl;
            clsf = fsiv_create_knn_classifier(knn_K);
        }
        else if (classifier == 1)
        {
            std::cout << "Using a SVM classifier with Kernel=" << svm_K
                      << " C=" << svm_C << " D=" << svm_D << " G=" << svm_G
                      << std::endl;
            clsf = fsiv_create_svm_classifier(svm_K, svm_C, svm_D, svm_G);
        }
        else if (classifier == 2)
        {
            std::cout << "Using a RTrees classifier with V=" << rtrees_V
                      << " T=" << rtrees_T << " E=" << rtrees_E
                      << std::endl;
            clsf = fsiv_create_rtrees_classifier(rtrees_V, rtrees_T, rtrees_E);
        }
        else
        {
            std::cerr << "Error: unknown classifier id:"
                      << classifier << std::endl;
            return EXIT_FAILURE;
        }

        fsiv_train_classifier(clsf, train_images, train_labels, train_flags);
        assert(clsf->isTrained());
        std::cout << "done." << std::endl;

        cv::Mat predict_labels;
        fsiv_make_predictions(clsf, train_images, predict_labels);
        std::cout << std::endl;
        print_model_metrics(train_labels, predict_labels,
                            dt.class_label_names(), std::cout);

        if (validate > 0.0)
        {
            std::cout << std::endl;
            std::cout << "Validating ... " << std::endl;
            fsiv_make_predictions(clsf, validate_images, predict_labels);
            std::cout << "done." << std::endl;
            std::cout << std::endl;
            print_model_metrics(validate_labels, predict_labels,
                                dt.class_label_names(), std::cout);
        }

        std::cout << std::endl;
        std::cout << "Saving the model to '" << model_fname << "'." << std::endl;
        // First save the classifier model.
        clsf->save(model_fname);

        // Second save the feature extractor model.
        cv::FileStorage f;
        f.open(model_fname, cv::FileStorage::APPEND);
        if (!f.isOpened())
            throw std::runtime_error("Could not open the file [" + model_fname +
                                     " to append.");
        f << "fsiv_classifier_type" << classifier;
        extractor->save_model(f);
        f.release();
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
