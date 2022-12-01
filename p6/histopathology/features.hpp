/**
 *  @file features.hpp
 *  (C) 2022- FJMC fjmadrid@uco.es
 */
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/core.hpp>

/**
 * @brief Define the differents feature extractors.
 */
typedef enum {
    FSIV_NON_EXTRACTOR=0, //None extractor is used.
    FSIV_GREY_LEVELS=1, // Use pixel grey levels [0,1].
    //TODO: Add new features to extract.
    //FSIV_XXXXX=2,
    //FSIV_YYYYY=3,
    //....
} FEATURE_IDS;

/**
 * @brief Base class to define feature extractors.
 */
class FeaturesExtractor
{
public:
    
    /**
     * @brief Get the extractor enum type.
     * 
     * @return FEATURE_IDS type.
     */
    FEATURE_IDS get_extractor_type() const;
    
    /**
     * @brief Get extractor name.
     * @return the extractor name.
     */
    virtual std::string get_extractor_name() const = 0;

    /**
     * @brief Virtual constructor for defined feature extractors.
     * @param id is the feature extractor type to create.
     * @return a shared ptr to the extractor.
     */
    static cv::Ptr<FeaturesExtractor> create(FEATURE_IDS id);

    /**
     * @brief Virtual constructor loading from a file storage.
     * @param fname is the file storage from which load the feature extractor.
     * @return a shared ptr to the extractor.
     */
    static cv::Ptr<FeaturesExtractor> create(const std::string& fname);

    /**
     * @brief Set extractor parameters.
     * @param params are the parameters.
     */
    void set_params(const std::vector<float>& params);

    /**
     * @brief Get extactor parameters.
     * @return the parameters.
     */
    const std::vector<float>& get_params() const;

    /**
     * @brief Train the extractor with samples.
     * @param samples are the input samples used to train process.
     * @warning By default this method does nothing. Override if your extractor need training.            
     */
    virtual void train(const cv::Mat& samples);

    /**
     * @brief Extract features from an image.
     * @param img the input image.
     * @return the extracted features.
     */
    virtual cv::Mat extract_features(const cv::Mat& img) = 0;

    /**
     * @brief Save the trained data for the feature extractor.
     * 
     * At least the feature type id and the parameters are saved with
     * labels 'fsiv_feature_id' and 'fsiv_feature_params' labels.
     * 
     * If you override this method use 'fsiv_xxxx' labels for your data.
     * 
     * @param f is the file storage where to save the data.
     */
    virtual void save_model(cv::FileStorage& f) const;

    /**
     * @brief Load the trained data for the feature extractor.
     * 
     * At least the feature type id and the parameters are load using
     * the labels 'fsiv_feature_id' and 'fsiv_feature_params'.
     * 
     * @param f is the file storage from which to load the data.
     * @return true if success.     
     */
    virtual bool load_model(cv::FileStorage& f);

protected:
    FEATURE_IDS type_;
    std::vector<float> params_;
};


/**
 * @brief Extract features from a dataset.
 *
 * @param dt is are the dataset's samples (one sample per row).
 * @param extractor is the features extractor to use.
 * @post ret_v.type()==CV_32FC1
 * @post ret_v.rows==dataset.rows
 */
cv::Mat fsiv_extract_features (const cv::Mat& dt,
                               cv::Ptr<FeaturesExtractor>& extractor);

/**
 * @brief Outputs a parameters vector.
 * @param out is the output stream.
 * @param params is the parameter vector.
 * @return the output stream.
 */
inline std::ostream&
operator << (std::ostream& out, const std::vector<float>& params)
{
    out << '[';
    for (auto& v: params)
        out << ' ' << v;
    out << " ]";
    return out;
}
