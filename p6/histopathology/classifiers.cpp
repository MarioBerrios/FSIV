/**
 *  @file classifiers.cpp
 *  (C) 2022- FJMC fjmadrid@uco.es
 */

#include "classifiers.hpp"

cv::Ptr<cv::ml::StatModel>
fsiv_create_knn_classifier(int K)
{
    cv::Ptr<cv::ml::KNearest> knn;

    // TODO: Create an KNN classifier.
    // Hint: Set algorithm type to BRUTE_FORCE.
    // Hint: Set it as a classifier (setIsClassifier)
    // Remenber: Set hyperparameter K.

    knn = cv::ml::KNearest::create();
    knn->setAlgorithmType(cv::ml::KNearest::BRUTE_FORCE);
    knn->setIsClassifier(true);
    knn->setDefaultK(K);

    //

    CV_Assert(knn != nullptr);
    return knn;
}

cv::Ptr<cv::ml::StatModel>
fsiv_create_svm_classifier(int Kernel,
                           double C,
                           double degree,
                           double gamma)
{
    cv::Ptr<cv::ml::SVM> svm;
    // TODO: Create an SVM classifier.
    // Set algorithm type to C_SVC.
    // Set hyperparameters: Kernel, C, Gamma, Degree.

    svm = cv::ml::SVM::create();
    svm->setType(cv::ml::SVM::C_SVC);

    svm->setKernel(Kernel);
    svm->setC(C);
    svm->setDegree(degree);
    svm->setGamma(gamma);

    //
    CV_Assert(svm != nullptr);
    return svm;
}

cv::Ptr<cv::ml::StatModel>
fsiv_create_rtrees_classifier(int V,
                              int T,
                              double E)
{
    cv::Ptr<cv::ml::RTrees> rtrees;
    // TODO: Create an RTrees classifier.
    // Set hyperparameters: Number of features used per node (ActiveVarCount),
    //  max num of trees, and required OOB error.
    // Remenber:: to set T and E parameters use a cv::TerminCriteria object
    //    where T is the max iterations and E is the epsilon.

    rtrees = cv::ml::RTrees::create();
    rtrees->setMaxCategories(10);

    if (V > 0)
        rtrees->setActiveVarCount(V);
    
    cv::TermCriteria criteria = cv::TermCriteria(
        cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS,
        T,
        E
    );
    rtrees->setTermCriteria(criteria);

    //
    CV_Assert(rtrees != nullptr);
    return rtrees;
}

void fsiv_train_classifier(cv::Ptr<cv::ml::StatModel> &clsf,
                           const cv::Mat &samples, const cv::Mat &labels,
                           int flags)
{
    CV_Assert(clsf != nullptr);

    // TODO: train the classifier.
    // Hint: you can use v::ml::TrainData to set the parameters.
    // Remenber: we are using ROW_SAMPLE ordering in the dataset.

    cv::Ptr<cv::ml::TrainData> train_data = 
        cv::ml::TrainData::create(samples, cv::ml::ROW_SAMPLE, labels);
    clsf->train(train_data, flags);

    //
    CV_Assert(clsf->isTrained());
}

void fsiv_make_predictions(cv::Ptr<cv::ml::StatModel> &clsf,
                           const cv::Mat &samples, cv::Mat &predictions)
{
    CV_Assert(clsf != nullptr);
    CV_Assert(clsf->isTrained());
    // TODO: do the predictions.
    // Remenber: the classefied used float to save the labels.

    clsf->predict(samples, predictions);
    predictions.convertTo(predictions, CV_32S);

    //
    CV_Assert(predictions.depth() == CV_32S);
    CV_Assert(predictions.rows == samples.rows);
}


cv::Ptr<cv::ml::StatModel>
fsiv_load_knn_classifier_model(const std::string &model_fname)
{
    cv::Ptr<cv::ml::StatModel> clsf;

    // TODO: load a KNN classifier.
    // Hint: use the generic interface cv::Algorithm::load< classifier_type >
    clsf = cv::Algorithm::load<cv::ml::KNearest>(model_fname);
    //

    CV_Assert(clsf != nullptr);
    return clsf;
}

cv::Ptr<cv::ml::StatModel>
fsiv_load_svm_classifier_model(const std::string &model_fname)
{
    cv::Ptr<cv::ml::StatModel> clsf;

    // TODO: load a SVM classifier.
    // Hint: use the generic interface cv::Algorithm::load< classifier_type >
    clsf = cv::Algorithm::load<cv::ml::SVM>(model_fname);
    //

    CV_Assert(clsf != nullptr);
    return clsf;
}

cv::Ptr<cv::ml::StatModel>
fsiv_load_rtrees_classifier_model(const std::string &model_fname)
{
    cv::Ptr<cv::ml::StatModel> clsf;

    // TODO: load a RTrees classifier.
    // Hint: use the generic interface cv::Algorithm::load< classifier_type >
    clsf = cv::Algorithm::load<cv::ml::RTrees>(model_fname);
    //

    CV_Assert(clsf != nullptr);
    return clsf;
}

cv::Ptr<cv::ml::StatModel>
load_classifier_model(const std::string &model_fname, int& clsf_id)
{
    cv::FileStorage f;
    cv::Ptr<cv::ml::StatModel> clsf;

    f.open(model_fname, cv::FileStorage::READ);
    auto node = f["fsiv_classifier_type"];
    if (node.empty() || !node.isInt())
        throw std::runtime_error("Could not find 'fsiv_classifier_type' "
                                 "label in model file");    
    node >> clsf_id;
    f.release();
    if (clsf_id == 0)
        clsf = fsiv_load_knn_classifier_model(model_fname);
    else if (clsf_id == 1)
        clsf = fsiv_load_svm_classifier_model(model_fname);
    else if (clsf_id == 2)
        clsf = fsiv_load_rtrees_classifier_model(model_fname);
    else
        throw std::runtime_error("Unknown classifier id: " +
                                 std::to_string(clsf_id));

    CV_Assert(clsf != nullptr);
    return clsf;
}