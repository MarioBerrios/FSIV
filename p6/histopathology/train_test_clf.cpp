/*!
  Esto es un esqueleto de programa para usar en las prácticas
  de Visión Artificial.

  Se supone que se utilizará OpenCV.

  Para compilar, puedes ejecutar:
    g++ -Wall -o esqueleto esqueleto.cc `pkg-config opencv --cflags --libs`

*/

#include <iostream>
#include <sstream>
#include <exception>

//Includes para OpenCV, Descomentar según los módulo utilizados.
#include <opencv2/core/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>

#include "features.hpp"
#include "common_code.hpp"

#ifndef NDEBUG
int __Debug_Level = 0;
#endif

const cv::String keys =
    "{help h usage ? |      | print this message   }"
    "{f            |0     | Feature to extract. Default 0 is gray levels [0,1].}"
    "{f_params     |      | Feature parameters (if any). Format <value>:<value>:<value>... }"
    "{v validate   |0     | Use the training (100-v)%/ v% validation partitions to train "
                             "and validate. Default is 0% means not validate.}"
    "{t test       |      | Only test a trained model.}"
    "{class        |0     | Classifier to train/test. 0: K-NN, 1:SVM, 2:RTREES.}"
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
    "{@dataset_path  |<none>| Dataset pathname.}"
    "{@model         |<none>| Model filename to save/load.}"
#ifndef NDEBUG
    "{verbose        |0     | Set the verbose level.}"
#endif
    ;

std::vector<float>
parse_feature_parmas(const std::string& f_params)
{
    std::vector<float> feature_params;
    std::istringstream in (f_params);
    float v;
    while (in)
    {
        in >> v;
        if (in)
            feature_params.push_back(v);
    }
    return feature_params;
}

int
main (int argc, char* const* argv)
{
  int retCode=EXIT_SUCCESS;

  try {

      cv::CommandLineParser parser(argc, argv, keys);
      parser.about("Train/Test a traffic sign classifier using the GTSRB dataset.");
      if (parser.has("help"))
      {
          parser.printMessage();
          return 0;
      }

#ifndef NDEBUG
      __Debug_Level = parser.get<int>("verbose");
#endif
      FEATURE_IDS feature_id = FEATURE_IDS(parser.get<int>("f"));
      std::vector<float> feature_params =
              parse_feature_parmas(parser.get<std::string>("f_params"));
      float validate = parser.get<float>("v");
      bool only_testing = parser.has("t");
      if (validate && only_testing)
      {
          std::cerr << "Error: Validating and only testing are not "
                       "compatible options." << std::endl;
          return EXIT_FAILURE;
      }

      std::string dataset_path = parser.get<std::string>("@dataset_path");
      std::string model_fname = parser.get<std::string>("@model");
      int classifier = parser.get<int>("class");
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


      cv::Mat val_descs, val_labels;
      cv::Mat test_descs, test_labels;

      cv::Mat train_images = fsiv_load_fashion_mnist_images(dataset_path +
                                                            "/train-images-idx3-ubyte");
      cv::Mat train_labels = fsiv_load_fashion_mnist_images(dataset_path +
                                                            "/train-labels-idx1-ubyte");
      CV_Assert(train_images.rows==train_labels.rows);

      std::cout << "Loaded dataset with " << train_images.rows << " samples."
                << std::endl;

      cv::Mat validate_images;
      cv::Mat validate_labels;

      if (validate>0.0)
      {
          fsiv_split_fashion_mnist_ds(validate, train_images, train_labels,
                                    train_images, train_labels,
                                    validate_images, validate_labels);
      }

      std::cout << "Train partition with " << train_images.rows << " samples."
                << std::endl;
      std::cout << "Validation partition with "
                << (validate_images.empty()?0:validate_images.rows)
                << " samples." << std::endl;

      std::cout << "\nExtracting features ..." << std::endl;

      train_images = fsiv_extract_feature_from_partion(train_images,
                                                       feature_id,
                                                       feature_params);
      if (!validate_images.empty())
          validate_images = fsiv_extract_feature_from_partion(validate_images,
                                                           feature_id,
                                                           feature_params);
      std::cout << "Features size: " <<
                   (train_images.rows*train_images.cols*train_images.elemSize())+
                   (validate_images.empty() ? 0 : ((validate_images.rows*validate_images.cols*validate_images.elemSize())));





      cv::Ptr<cv::ml::StatModel> clsf;
      int train_flags = 0;

      if (validate)
      {
          if (classifier == 0)
          {
              cv::Ptr<cv::ml::KNearest> knn;

              //TODO: Create an KNN classifier.
              //Set algorithm type to BRUTE_FORCE.
              //Set it as a classifier (setIsClassifier)
              //Set hyperparameter K.
              knn = cv::ml::KNearest::create();
              knn->setAlgorithmType(cv::ml::KNearest::BRUTE_FORCE);
              knn->setDefaultK(knn_K);
              knn->setIsClassifier(true);
              //

              assert(knn != nullptr);
              clsf = knn;
          } else if (classifier == 1)
          {
              cv::Ptr<cv::ml::SVM> svm;

              //TODO: Create an SVM classifier.
              //Set algorithm type to C_SVC.
              //Set it as a classifier (setIsClassifier)
              //Set hyperparameters: C, kernel, Gamma, Degree.
              svm = cv::ml::SVM::create();
              svm->setType(cv::ml::SVM::C_SVC);
              svm->setKernel(svm_K);
              svm->setC(svm_C);
              svm->setDegree(svm_D);
              svm->setGamma(svm_G);
              //

              assert(svm!=nullptr);
              clsf = svm;
          }
          else if (classifier == 2)
          {
              cv::Ptr<cv::ml::RTrees> rtrees;
              //TODO: Create an RTrees classifier.
              //Set hyperparameters: Number of features used per node (ActiveVarCount),
              // max num of trees, and required OOB error.
              rtrees = cv::ml::RTrees::create();
              if (rtrees_V>0)
                  rtrees->setActiveVarCount(rtrees_V);
              rtrees->setTermCriteria(
                          cv::TermCriteria(cv::TermCriteria::MAX_ITER |
                                           cv::TermCriteria::EPS,
                                           rtrees_T, rtrees_E));
              //
              assert(rtrees!=nullptr);
              clsf = rtrees;
          }
          else
          {
              std::cerr << "Error: unknown classifier." << std::endl;
              return EXIT_FAILURE;
          }
      }
      else
      {
          train_flags = cv::ml::StatModel::UPDATE_MODEL;
          if (classifier==0)
              //TODO: load a KNN classifier.
              clsf = cv::Algorithm::load<cv::ml::KNearest>(model_fname);
              //
          else if (classifier==1)
              //TODO: load a SVM classifier.
              clsf = cv::Algorithm::load<cv::ml::SVM>(model_fname);
              //
          else if (classifier==2)
              //TODO: load a RTrees classifier.
              clsf = cv::Algorithm::load<cv::ml::RTrees>(model_fname);
              //
          else
          {
              std::cerr << "Error: unknown classifier." << std::endl;
              return EXIT_FAILURE;
          }
          if (clsf==nullptr || !clsf->isTrained())
          {
              std::cerr << "Error: Not validate mode activated. I need a trained model!" << std::endl;
              return EXIT_FAILURE;
          }
      }
      if (!only_testing)
      {
          std::cout << "Training with "<< train_labels.rows << " samples ... ";

          cv::Ptr<cv::ml::TrainData> train_data =
                  cv::ml::TrainData::create(train_descs, cv::ml::ROW_SAMPLE,
                                            train_labels);

          //TODO: train the classifer using training data.
          clsf->train(train_data, train_flags);
          //

          assert(clsf->isTrained());
          std::cout << "done." << std::endl;
      }

      if (validate)
          std::cerr << "Validating with ";
      else
          std::cerr << "Testing with ";
      std::cout << val_labels.rows << " samples ... ";

      cv::Mat predict_labels;
      std::cout << "done." << std::endl;

      //TODO: make the predictions over the validation/test data.
      clsf->predict(val_descs, predict_labels);      
      //

      assert(!predict_labels.empty() && predict_labels.rows == val_descs.rows);

      //Compute the confussion matrix.
      cv::Mat cmat = fsiv_compute_confusion_matrix(cv::Mat_<float>(val_labels),
                                              predict_labels);

      float mrr = fsiv_compute_mean_recognition_rate(cmat, categories);
      float acc = fsiv_compute_accuracy(cmat);

      std::cout << "##############################" << std::endl;
      std::cout << "# Model metrics: ";
      if (validate)
          std::cout << " VALIDATION" << std::endl;
      else
          std::cout << " TESTING" << std::endl;
      std::cout << "##############################" << std::endl;
      std::cout << std::endl;
      std::cout << "Accuracy : " << acc << std::endl;
      std::cout << "Mean RR  : " << mrr << std::endl;
      std::cout << "RR per class: " << std::endl;
      for (size_t c=0;c<categories.size();c++)
          std::cout << "\t[" << category_descs[c] << "]: "
                    << fsiv_compute_recognition_rate(cmat, categories[c])
                    << std::endl;
      std::cout << std::endl;

      if (!only_testing)
      {
          std::cout << "Saving the model to '" << model_fname << "'." << std::endl;
          clsf->save(model_fname);
      }

      std::cout << "Model size: " << compute_file_size(model_fname, 1024*1024)
                << " Mb." << std::endl;
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception caught: " << e.what() << std::endl;
    retCode = EXIT_FAILURE;
  }
  return retCode;
}
