// ----------------------------------------
// common_code.hpp
// (c) FSIV, University of Cordoba
// ----------------------------------------
#pragma once

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

/**
 * @brief Remove segmentation noise using morphological operations.
 * @param img image where removing the noise.
 * @param r is the radius of the structuring element.
 * @pre img.type()==CV_8UC1
 * @pre r>0
 */
void fsiv_remove_segmentation_noise(cv::Mat & img, int r);

/**
 * @brief Applies a segmentation method based on image difference
 * @param[in] prevFrame Previous image frame (RGB)
 * @param[in] curFrame  Current image frame (RGB)
 * @param[out] difimg  Single-channel generated mask
 * @param[in] thr Theshold used to decide if a pixel contains enough motion to be considered foreground.
 * @param[in] r  Radius to remove segmentation noise. (r=0 means not remove).
 */
void fsiv_segm_by_dif(const cv::Mat & prevFrame,
                      const cv::Mat & curFrame,
                      cv::Mat & difimg, int thr=11, int r=0);

/**
 * @brief Applies a mask to an RGB image
 * @param[in] frame RGB input image.
 * @param[in] mask  Single-channel mask.
 * @param[out] outframe Output RGB frame.
 */
void fsiv_apply_mask(const cv::Mat & frame, const cv::Mat & mask, cv::Mat & outframe);

/**
 * @brief Learns a gaussian background model given an input stream.
 * @param input     RGB input image.
 * @param[out] mean  the mean image.
 * @param[out] variance the variance image.
 * @param[in] num_frames Number of frames used to estimated the model.
 * @param[in] gauss_r is the radius used to gaussian avegaging of input frames.
 * @param[in] wname Window used to show the captured frames (if gived).
 */
bool fsiv_learn_gaussian_model(cv::VideoCapture & input,
                               cv::Mat & mean,
                               cv::Mat & variance,
                               int num_frames=100, int gauss_r=0, const char * wname=nullptr);

/**
 * @brief Applies a segmentation method based on a Gaussian model of the background.
 * @param[in] frame RGB input image.
 * @param[out] mask  mask image with foreground pixels to 255.
 * @param[in] mean Model's mean of each RGB pixel.
 * @param[in] variance Model's variance of each RGB pixel.
 * @param[in] k define the detection threshold.
 * @param[in] r radius used to remove segmentation noise (value 0 means not remove).
 */
void fsiv_segm_by_gaussian_model(const cv::Mat & frame,
                                 cv::Mat & mask,
                                 const cv::Mat & mean,
                                 const cv::Mat & variance, float k=3.0, int r=0);
/**
 * @brief Update the Background Gaussian model.
 * @param[in] frame is current frame image.
 * @param[in] mask is the current segmentation mask.
 * @param[in] frame_count is the number of this frame into the stream.
 * @param[in,out] mean is the current Model's mean to be updated.
 * @param[in,out] variance is the current Model's variance to be updated.
 * @param[in] alpha is the update rate.
 * @param[in] short_term_update_period specifies the short term update period.
 * @param[in] long_term_update_period specifies the long term update period.
 */
void fsiv_update_gaussian_model(const cv::Mat & frame,
                                const cv::Mat & mask,
                                unsigned long frame_count,
                                cv::Mat & mean,
                                cv::Mat & variance,
                                float alpha,
                                unsigned short_term_update_period=1,
                                unsigned long_term_update_period=10);

