/**
 *  @file metrics.hpp
 *  (C) 2022- FJMC fjmadrid@uco.es
 */
#pragma once
#include <iostream>
#include <vector>
#include <opencv2/core.hpp>

/**
 * @brief Compute the confussion matrix.
 * 
 * Is a matrix where the rows are the ground-truth labels and the columns are
 * the predicted labels.
 * 
 * @param true_labels are supervised labels.
 * @param predicted_labels are the predicted labels.
 * @param n_categories is the number of categories.
 * @return the confussion matrix.
 */
cv::Mat fsiv_compute_confusion_matrix(const cv::Mat &true_labels,
                                      const cv::Mat &predicted_labels,
                                      int n_categories);

/**
 * @brief Compute the accuracy metrix.
 * 
 * @param cmat is the confussion matrix.
 * @return the accuracy.
 */
float fsiv_compute_accuracy(const cv::Mat &cmat);

/**
 * @brief compute the recogniton rate per category.
 * 
 * @param cmat is the confussion matrix.
 * @return a vector with the recognition rate per category.
 */
std::vector<float> fsiv_compute_recognition_rates(const cv::Mat &cmat);

/**
 * @brief Compute the mean recognition rate.
 * 
 * @param rr are the recognition rate per category.
 * @return the mean recognition rate.
 */
float fsiv_compute_mean_recognition_rate(const std::vector<float> &rr);

/**
 * @brief Output model metrics.
 * 
 * @param gt_labels are supervised labels.
 * @param predicted_labels are predicted labels.
 * @param categories is a vector with the categories names.
 * @param out is the stream to print out.
 */
void print_model_metrics(const cv::Mat &gt_labels,
                         const cv::Mat &predicted_labels,
                         const std::vector<std::string>& categories,
                         std::ostream &out);