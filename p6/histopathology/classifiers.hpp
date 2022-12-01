/**
 *  @file classifiers.hpp
 *  (C) 2022- FJMC fjmadrid@uco.es
 */
#pragma once

#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>

/**
 * @brief Create a KNN classifier.
 *
 * @param K is the number of NN neighbours used to classify a sample.
 * @return an instance of the classifier.
 */
cv::Ptr<cv::ml::StatModel> fsiv_create_knn_classifier(int K);

/**
 * @brief Create a SVM classifier.
 *
 * @param Kernel is the kernel type @see cv::ml::SVM::KernelTypes
 * @param C is the SVM's C parameter.
 * @param degree is the degree when kernel type is POLY
 * @param gamma is the gamma exponent when kernel type is RBF.
 * @return an instance of the classifier
 */
cv::Ptr<cv::ml::StatModel> fsiv_create_svm_classifier(int Kernel,
                                                      double C,
                                                      double degree,
                                                      double gamma);

/**
 * @brief Create a Random Trees classifier.
 *
 * @param V is the number of features used by node. Value 0 means not set this value.
 * @param T is the maximum number of generated trees.
 * @param E is minimun the OOB error allowed.
 * @return an instance of the classifier
 */
cv::Ptr<cv::ml::StatModel> fsiv_create_rtrees_classifier(int V,
                                                         int T,
                                                         double E);

/**
 * @brief Train a classifier.
 *
 * @param clsf the instance of the classifier to be trainned.
 * @param samples are the input samples.
 * @param labels are the input labels.
 * @param flags are the train flags.
 * @pre clfs != nullptr
 * @post clfs->isTrained()
 */
void fsiv_train_classifier(cv::Ptr<cv::ml::StatModel> &clsf,
                           const cv::Mat &samples, const cv::Mat &labels,
                           int flags = 0);

/**
 * @brief Make predictions using a trained classifier.
 *
 * @param clsf an instance of the classifier to be used.
 * @param samples the samples to predict the labels.
 * @param predictions the labels predicted.
 * @pre clsf->isTrained()
 * @post predictions.depth()=CV_32S
 * @post predictions.rows == samples.rows.
 */
void fsiv_make_predictions(cv::Ptr<cv::ml::StatModel> &clsf,
                           const cv::Mat &samples, cv::Mat &predictions);

/**
 * @brief Load a knn classifier's model from file.
 *
 * @param model_fname is the file name.
 * @return an instance of the classifier.
 * @post ret_v != nullptr
 */
cv::Ptr<cv::ml::StatModel> fsiv_load_knn_classifier_model(
    const std::string &model_fname);

/**
 * @brief Load a svm classifier's model from file.
 *
 * @param model_fname is the file name.
 * @return an instance of the classifier.
 * @post ret_v != nullptr
 */
cv::Ptr<cv::ml::StatModel> fsiv_load_svm_classifier_model(
    const std::string &model_fname);

/**
 * @brief Load a rtrees classifier's model from file.
 *
 * @param model_fname is the file name.
 * @return an instance of the classifier.
 * @post ret_v != nullptr
 */
cv::Ptr<cv::ml::StatModel> fsiv_load_rtrees_classifier_model(
    const std::string &model_fname);

/**
 * @brief Load a classifier model from a file storage.
 * 
 * @param[in] model_fname  is the file storage pathname.
 * @param[out] clsf_id is the loaded classifier id.
 * @return the classifier instance. 
 */
cv::Ptr<cv::ml::StatModel> load_classifier_model(const std::string &model_fname,
                                                 int &clsf_id);