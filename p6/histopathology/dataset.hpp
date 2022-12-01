/**
 *  @file dataset.hpp
 *  (C) 2022- FJMC fjmadrid@uco.es
 */
#pragma once

#include <string>
#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>

/**
 * @brief The Dataset
 */
class Dataset
{
public:

    /**
     * @brief Create a empty dataset.
     * Use @see load to load the samples from disk.
     */
    Dataset();

    /**
     * @brief Get the number of samples in the dataset.
     * @return number of samples.
     */
    int num_samples() const;
    /**
     * @brief load the dataset from disk.
     * @param fname is the dataset file description.
     * @param rseed the seed used to random shuffle the input images. Value 0 means do not shuffle.
     * @return 0 if success.
     */
    int load (const std::string& fname, std::uint64_t rseed=0);

    /**
     * @brief split the dataset into the train and validation partitions.
     * @param train_size is the percentage used to train.
     * @param X_t the train samples.
     * @param y_t the train labels.
     * @param X_v the validation samples.
     * @param y_v the validation labels.
     */
    void split (float train_size, cv::Mat& X_t, cv::Mat& y_t,
                cv::Mat& X_v, cv::Mat& y_v) const;

    /**
     * @brief Compute the memory used by the dataset.
     * @return the memory used by the dataset.
     */
    size_t memory_size() const;

    /**
     * @brief Get the class label names.
     * @return a vector with the class label names.
     */
    const std::vector<std::string>& class_label_names() const;

    /**
     * @brief Get the samples.
     * One row per sample.
     * @return the samples.
     */
    const cv::Mat& X() const;
    cv::Mat& X();

    /**
     * @brief Get the labels
     * @return the samples.
     */
    const cv::Mat& y() const;
    cv::Mat& y();

protected:
    cv::Mat X_; /**< samples.*/
    cv::Mat y_; /**< labels.*/
    std::vector<std::string> class_label_names_; /**< class label names.*/

};

/**
 * @brief Utility to compute the size of a file.
 * @param fname
 * @param units are the units used. For example use (1024x1024) for Mb.
 * @return the number of units.
 */
float compute_file_size(std::string const& fname, const long units=1);
