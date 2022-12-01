/**
 *  @file metrics.cpp
 *  (C) 2022- FJMC fjmadrid@uco.es
 */
#include <iomanip>
#include <iostream>
#include "metrics.hpp"

cv::Mat
fsiv_compute_confusion_matrix(const cv::Mat &true_labels,
                              const cv::Mat &predicted_labels,
                              int n_categories)
{
    CV_Assert(true_labels.rows == predicted_labels.rows);
    CV_Assert(true_labels.type() == CV_32SC1);
    CV_Assert(predicted_labels.type() == CV_32SC1);
    cv::Mat cmat = cv::Mat::zeros(n_categories, n_categories, CV_32F);

    //TODO: Compute the confussion matrix.
    //Remenber: Rows are the Ground Truth. Cols are the predictions.

    for (int i = 0; i < true_labels.rows; i++){
        int t = cv::max(true_labels.at<int>(i), 0);
        int p = cv::max(predicted_labels.at<int>(i), 0);
        cmat.at<float>(t, p) += 1.0;
    }

    //
    CV_Assert(std::abs(cv::sum(cmat)[0] - static_cast<double>(true_labels.rows)) <=
              1.0e-6);
    return cmat;
}

std::vector<float>
fsiv_compute_recognition_rates(const cv::Mat &cmat)
{
    CV_Assert(!cmat.empty() && cmat.type() == CV_32FC1);
    CV_Assert(cmat.rows == cmat.cols);
    std::vector<float> RR(cmat.rows);

    for (int category = 0; category < cmat.rows; ++category)
    {
        RR[category] = 0.0;

        //TODO: compute the recognition rate (RR) for the category.
        //Avoid zero divisions!!.

        float total = 0;
        for (int i = 0; i < cmat.cols; i++){
            total += cmat.at<float>(category, i);
        }
        if (total > 0)
            RR[category] = cmat.at<float>(category, category) / total;

        //
        CV_Assert(RR[category] >= 0.0f && RR[category] <= 1.0f);
    }
    return RR;
}

float fsiv_compute_accuracy(const cv::Mat &cmat)
{
    CV_Assert(!cmat.empty() && cmat.type() == CV_32FC1);
    CV_Assert(cmat.rows == cmat.cols && cmat.rows > 1);

    float acc = 0.0;

    //TODO: compute the accuracy.
    //Hint: the accuracy is the rate of correct classifications
    //  to the total.
    //Remenber: avoid zero divisions!!.
        
    float top = 0;
    float bottom = 0;
    for (int i = 0; i < cmat.rows; i++){
        for (int j = 0; j < cmat.cols; j++){
            bottom += cmat.at<float>(i, j);
        }
        top += cmat.at<float>(i, i);
    }
    
    if (bottom > 0)
        acc = top/bottom;

    //
    CV_Assert(acc >= 0.0f && acc <= 1.0f);
    return acc;
}

float fsiv_compute_mean_recognition_rate(const std::vector<float> &rr)
{
    float m_rr = 0.0;
    //TODO
    //Remenber: the MRR is the mean value of the recognition rates.
    float total = 0;
    for (auto item: rr)
        total += item;
    if (rr.size() > 0)
        m_rr = total / rr.size();
    //
    return m_rr;
}

void print_model_metrics(const cv::Mat &gt_labels,
                         const cv::Mat &predicted_labels,
                         const std::vector<std::string>& categories,
                         std::ostream &out)
{
    cv::Mat cmat = fsiv_compute_confusion_matrix(gt_labels, predicted_labels,
                                                 categories.size());
    float acc = fsiv_compute_accuracy(cmat);
    std::vector<float> rr = fsiv_compute_recognition_rates(cmat);
    float m_rr = fsiv_compute_mean_recognition_rate(rr);
    out << "#########################" << std::endl;
    out << "Model metrics:         " << std::endl;
    out << std::endl;
    out << "Recognition rate per class:" << std::endl;
    for (size_t i = 0; i < rr.size(); ++i)
        out << std::setw(20) << std::setfill(' ')
            << categories[i]
            << ": " << rr[i] << std::endl;
    out << std::endl;
    out << "Mean recognition rate: " << m_rr << std::endl;
    out << "Accuracy: " << acc << std::endl;
}