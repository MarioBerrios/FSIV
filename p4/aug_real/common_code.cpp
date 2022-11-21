#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>
#include "common_code.hpp"

std::vector<cv::Point3f>
fsiv_generate_3d_calibration_points(const cv::Size& board_size,
                                    float square_size)
{
    std::vector<cv::Point3f> ret_v;
    //TODO
    //Remenber: the first inner point has (1,1) in board coordinates.
    for (int i = 1; i <= board_size.height; i++){
        for (int j = 1; j <= board_size.width; j++){
            ret_v.push_back(cv::Point3f(j * square_size, i * square_size, 0));
        }
    }
    //
    CV_Assert(ret_v.size()==board_size.width*board_size.height);
    return ret_v;
}


bool
fsiv_find_chessboard_corners(const cv::Mat& img, const cv::Size &board_size,
                             std::vector<cv::Point2f>& corner_points,
                             const char * wname)
{
    CV_Assert(img.type()==CV_8UC3);
    bool was_found = false;
    //TODO

    was_found = cv::findChessboardCorners(img, board_size, corner_points);
    if(was_found){
        cv::Mat aux;
        cv::cvtColor(img, aux, cv::COLOR_BGR2GRAY);
        cv::TermCriteria criteria = cv::TermCriteria(
            cv::TermCriteria::EPS + cv::TermCriteria::COUNT,
            40,
            0.001
        );
        cv::cornerSubPix(aux, corner_points, cv::Size(5,5), cv::Size(-1, -1), criteria);
    }

    if (wname != nullptr){
        cv::drawChessboardCorners(img, board_size, corner_points, was_found);
        cv::imshow(wname, img);
    }

    //
    return was_found;
}


void fsiv_compute_camera_pose(const std::vector<cv::Point3f> &_3dpoints,
                              const std::vector<cv::Point2f> &_2dpoints,
                              const cv::Mat& camera_matrix,
                              const cv::Mat& dist_coeffs,
                              cv::Mat& rvec,
                              cv::Mat& tvec)
{
    CV_Assert(_3dpoints.size()>=4 && _3dpoints.size()==_2dpoints.size());
    //TODO

    cv::solvePnP(_3dpoints, _2dpoints, camera_matrix, dist_coeffs, rvec, tvec);

    //
    CV_Assert(rvec.rows==3 && rvec.cols==1 && rvec.type()==CV_64FC1);
    CV_Assert(tvec.rows==3 && tvec.cols==1 && tvec.type()==CV_64FC1);
}

void
fsiv_draw_axes(cv::Mat& img,
               const cv::Mat& camera_matrix, const cv::Mat& dist_coeffs,
               const cv::Mat& rvec, const cv::Mat& tvec,
               const float size, const int line_width)
{
    //TODO

    std::vector<cv::Point2f> img_points;
    std::vector<cv::Point3f> points;
    points.push_back(cv::Point3f(0,0,0));
    points.push_back(cv::Point3f(size,0,0));
    points.push_back(cv::Point3f(0,size,0));
    points.push_back(cv::Point3f(0,0,-size));

    cv::projectPoints(points, rvec, tvec, camera_matrix, dist_coeffs, img_points);
    cv::line(img, img_points[0], img_points[1], cv::Scalar(0,0,255), line_width);
    cv::line(img, img_points[0], img_points[2], cv::Scalar(0,255,0), line_width);
    cv::line(img, img_points[0], img_points[3], cv::Scalar(255,0,0), line_width);

    //
}

void
fsiv_load_calibration_parameters(cv::FileStorage &fs,
                                 cv::Size &camera_size,
                                 float& error,
                                 cv::Mat& camera_matrix,
                                 cv::Mat& dist_coeffs,
                                 cv::Mat& rvec,
                                 cv::Mat& tvec)
{
    CV_Assert(fs.isOpened());
    //TODO

    fs["image-width"] >> camera_size.width;
    fs["image-height"] >> camera_size.height;
    fs["error"] >> error;
    fs["camera-matrix"] >> camera_matrix;
    fs["distortion-coefficients"] >> dist_coeffs;
    fs["rvec"] >> rvec;
    fs["tvec"] >> tvec;

    //
    CV_Assert(fs.isOpened());
    CV_Assert(camera_matrix.type()==CV_64FC1 && camera_matrix.rows==3 && camera_matrix.cols==3);
    CV_Assert(dist_coeffs.type()==CV_64FC1 && dist_coeffs.rows==1 && dist_coeffs.cols==5);
    CV_Assert(rvec.type()==CV_64FC1 && rvec.rows==3 && rvec.cols==1);
    CV_Assert(tvec.type()==CV_64FC1 && tvec.rows==3 && tvec.cols==1);
    return;
}

void
fsiv_draw_3d_model(cv::Mat &img, const cv::Mat& M, const cv::Mat& dist_coeffs,
                   const cv::Mat& rvec, const cv::Mat& tvec,
                   const float size)
{
    CV_Assert(img.type()==CV_8UC3);
    //TODO

    std::vector<cv::Point2f> img_points;
    std::vector<cv::Point3f> points;

    float half_square = size/2.0;

    points.push_back(cv::Point3f(2*size,2*size,0));
    points.push_back(cv::Point3f(5*size,2*size,0));
    points.push_back(cv::Point3f(2*size,3*size,0));
    points.push_back(cv::Point3f(5*size,3*size,0));
 
    points.push_back(cv::Point3f(3*size,2*size,-size));
    points.push_back(cv::Point3f(5*size,2*size,-size));
    points.push_back(cv::Point3f(3*size,3*size,-size));
    points.push_back(cv::Point3f(5*size,3*size,-size));    

    points.push_back(cv::Point3f(2*size,2*size,-2*size));
    points.push_back(cv::Point3f(3*size,2*size,-2*size));
    points.push_back(cv::Point3f(2*size,3*size,-2*size));
    points.push_back(cv::Point3f(3*size,3*size,-2*size));
    points.push_back(cv::Point3f(3*size,2*size + half_square,-2*size));
    points.push_back(cv::Point3f(5*size,2*size + half_square,-2*size));

    points.push_back(cv::Point3f(2*size + half_square,2*size + half_square,-3*size));

    cv::projectPoints(points, rvec, tvec, M, dist_coeffs, img_points);

    cv::line(img, img_points[0], img_points[1], cv::Scalar(0,0,0), 3);
    cv::line(img, img_points[0], img_points[2], cv::Scalar(0,0,0), 3);
    cv::line(img, img_points[2], img_points[3], cv::Scalar(0,0,0), 3);
    cv::line(img, img_points[1], img_points[3], cv::Scalar(0,0,0), 3);

    cv::line(img, img_points[4], img_points[5], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[6], img_points[7], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[5], img_points[7], cv::Scalar(0,0,255), 3);
    
    cv::line(img, img_points[1], img_points[5], cv::Scalar(0,0,0), 3);
    cv::line(img, img_points[3], img_points[7], cv::Scalar(0,0,0), 3);

    cv::line(img, img_points[8], img_points[9], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[8], img_points[10], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[9], img_points[11], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[10], img_points[11], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[12], img_points[13], cv::Scalar(0,0,255), 3);

    cv::line(img, img_points[12], img_points[4], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[12], img_points[6], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[13], img_points[5], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[13], img_points[7], cv::Scalar(0,0,255), 3);

    cv::line(img, img_points[0], img_points[8], cv::Scalar(0,0,0), 3);
    cv::line(img, img_points[4], img_points[9], cv::Scalar(0,0,0), 3);
    cv::line(img, img_points[2], img_points[10], cv::Scalar(0,0,0), 3);
    cv::line(img, img_points[6], img_points[11], cv::Scalar(0,0,0), 3);
 
    cv::line(img, img_points[14], img_points[8], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[14], img_points[9], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[14], img_points[10], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[14], img_points[11], cv::Scalar(0,0,255), 3);
    //
}

void
fsiv_project_image(const cv::Mat& input, cv::Mat& output,
                   const cv::Size& board_size,
                   const std::vector<cv::Point2f>& _2dpoints)
{
    CV_Assert(!input.empty() && input.type()==CV_8UC3);
    CV_Assert(!output.empty() && output.type()==CV_8UC3);
    CV_Assert(board_size.area()==_2dpoints.size());
    //TODO

    std::vector<cv::Point2f> input_corners;
    input_corners.push_back(cv::Point2f(0, 0));
    input_corners.push_back(cv::Point2f(input.cols-1, 0));
    input_corners.push_back(cv::Point2f(0, input.rows-1));
    input_corners.push_back(cv::Point2f(input.cols-1, input.rows-1));

    std::vector<cv::Point2f> board_corners;
    board_corners.push_back(_2dpoints[0]);
    board_corners.push_back(_2dpoints[board_size.width-1]);
    board_corners.push_back(_2dpoints[(board_size.height-1) * board_size.width]);
    board_corners.push_back(_2dpoints[board_size.height * board_size.width - 1]);

    cv::Mat M = cv::getPerspectiveTransform(input_corners, board_corners);
    cv::Mat aux = output.clone();
    cv::Mat mask = cv::Mat::zeros(output.size(), output.type());
    cv::fillConvexPoly(mask, std::vector<cv::Point>{board_corners[0], board_corners[1], 
        board_corners[3], board_corners[2]}, cv::Scalar(255,255,255));

    cv::warpPerspective(input, aux, M, aux.size());
    aux.copyTo(output, mask);

    //
}
