#pragma once
#include <opencv2/core.hpp>

/**
 * @brief Return a Gaussian filter.
 * @arg[in] r is the filter's radius.
 * @return the filter.
 * @pre r>0;
 * @post ret_v.type()==CV_32FC1
 * @post retV.rows==retV.cols==2*r+1
 * @post (abs(cv::sum(retV)-1.0)<1.0e-6
 */
cv::Mat fsiv_create_gaussian_filter(const int r);


/**
 * @brief Extend an image centering on the result.
 * @param in the input image.
 * @param new_size is the new geometry.
 * @param ext_type is the type of extension: 0->Padding with zeros. 1->circular.
 * @return the extended image.
 * @pre img.rows<new_size.height
 * @pre img.cols<new_size.width
 * @post ret_v.type()==img.type()
 * @post ret_v.size()==new_size
 */
cv::Mat fsiv_extend_image(const cv::Mat& img, const cv::Size& new_size, int ext_type = 0);

/**
 * @brief Create a sharpening filter.
 * @param filter_type specify what type of laplacian to use: 0->LAP_4, 1->LAP_8, 2->DoG.
 * @param r1 if filter type is 2 (DoG), r1 is the radius for the first Gaussian filter.
 * @param r2 if filter type is 2 (DoG), r2 is the radius for the second Gaussian filter.
 * @return the filter.
 * @pre filter_type in {0,1,2}
 * @post retval.type()==CV_32FC1
 */
cv::Mat fsiv_create_sharpening_filter(const int filter_type, int r1=1, int r2=2);

/**
 * @brief Do a sharpeing enhance to an image.
 * @param img is the input image.
 * @param filter_type is the sharpening filter to use: 0->LAP_4, 1->LAP_8, 2->DOG.
 * @param only_luma if the input image is RGB only enhances the luma, else enhances all RGB channels.
 * @param r1 if filter type is DOG, is the radius of first Gaussian filter.
 * @param r2 if filter type is DOG, is the radius of second Gaussian filter.
 * @param circular if it is true, use circular convolution.
 * @return the enahance image.
 * @pre filter_type in {0,1,2}.
 * @pre 0<r1<r2
 */
cv::Mat fsiv_image_sharpening(const cv::Mat& in, int filter_type, bool only_luma,
                      int r1, int r2, bool circular);
