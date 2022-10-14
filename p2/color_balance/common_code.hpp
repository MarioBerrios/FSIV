#pragma once
#include <opencv2/core/core.hpp>

/**
 * @brief Scale the color of an image so an input color is transformed into an output color.
 * @param in is the image to be rescaled.
 * @param from is the input color.
 * @param to is the output color.
 * @return the color rescaled image.
 * @pre in.type()==CV_8UC3
 * @warning A BGR color space is assumed for the input image.
 */
cv::Mat fsiv_color_rescaling(const cv::Mat& in, const cv::Scalar& from,
                             const cv::Scalar& to);
/**
 * @brief Apply a "white patch" color balance operation to the image.
 * @arg[in] in is the imput image.
 * @return the color balanced image.
 * @pre in.type()==CV_8UC3
 * @warning A BGR color space is assumed for the input image.
 */
cv::Mat fsiv_wp_color_balance(cv::Mat const& in);

/**
 * @brief Apply a "gray world" color balance operation to the image.
 * @arg[in] in is the imput image.
 * @return the color balanced image.
 * @pre in.type()==CV_8UC3
 * @warning A BGR color space is assumed for the input image.
 */
cv::Mat fsiv_gw_color_balance(cv::Mat const& in);

/**
 * @brief Apply a general color balance operation to the image.
 * @arg[in] in is the imput image.
 * @arg[in] p is the percentage of brightest points used to calculate the color correction factor.
 * @return the color balanced image.
 * @pre in.type()==CV_8UC3
 * @pre 0.0 < p < 100.0
 * @warning A BGR color space is assumed for the input image.
 */
cv::Mat fsiv_color_balance(cv::Mat const& in, float p);
