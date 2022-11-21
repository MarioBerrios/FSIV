/*!
  Esto es un esqueleto de programa para usar en las prácticas
  de Visión Artificial.

  Se supone que se utilizará OpenCV.

  Para compilar, puedes ejecutar:
    g++ -Wall -o esqueleto esqueleto.cc `pkg-config opencv --cflags --libs`

*/

#include <iostream>
#include <exception>

//Includes para OpenCV, Descomentar según los módulo utilizados.
#include <opencv2/core/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/calib3d/calib3d.hpp>

#include "common_code.hpp"

const cv::String keys =
    "{help h usage ? |      | print this message.}"
    "{verbose        |      | activate verbose mode.}"
    "{i intrinsics   |      | Calibrate only extrinsics parameters. Using intrinsics from given file (-i=intr-file).}"
    "{s size         |<none>| square size.}"
    "{r rows         |<none>| number of board's rows.}"
    "{c cols         |<none>| number of board's cols.}"
    "{@output        |<none>| filename for output intrinsics file.}"
    "{@input1        |<none>| first board's view.}"
    "{@input2        |      | second board's view.}"
    "{@inputn        |      | ... n-idx board's view.}"
    ;

int
main (int argc, char* const* argv)
{
    int retCode=EXIT_SUCCESS;

    try {        
        cv::CommandLineParser parser(argc, argv, keys);
        parser.about("Calibrate the intrinsics parameters of a camera.");
        if (parser.has("help"))
        {
            parser.printMessage();
            return EXIT_SUCCESS;
        }
        float square_size = parser.get<float>("s");
        int rows = parser.get<int>("r");
        int cols = parser.get<int>("c");
        bool verbose = parser.has("verbose");
        std::string output_fname = parser.get<cv::String>("@output");
        if (!parser.check())
        {
            parser.printErrors();
            return EXIT_FAILURE;
        }

        //Get the inputs.
        //find the second argument without '-' at begin.
        int input = 1;
        bool found = false;
        while (input<argc && !found)
            found = argv[input++][0] != '-';
        CV_Assert(input<argc);
        std::vector<std::string> input_fnames;        
        for (; input<argc; ++input)
            input_fnames.push_back(std::string(argv[input]));

        //TODO
        //cv::namedWindow("INPUT");
        cv::namedWindow("OUTPUT");

        cv::Mat img_input, img_output;

        cv::Mat camera_matrix, dist_coeffs, rvec, tvec;
        cv::Size camera_size, board_size(cols-1, rows-1);
        float error;

        std::vector<cv::Point3f> points3d;
        points3d = fsiv_generate_3d_calibration_points(board_size, square_size);
        std::vector<cv::Point2f> points2d;

        std::vector<std::vector<cv::Point3f>> world_points;
        std::vector<std::vector<cv::Point2f>> camera_points;

        if (parser.has("i"))
        {
            //TODO
            //Make extrinsic calibration.
            //Remenber: only one view is needed.
            img_input = cv::imread(input_fnames[0]);

            std::string intrinsic_fname = parser.get<cv::String> ("i");
            cv::FileStorage fs_input (intrinsic_fname, cv::FileStorage::READ);
            fsiv_load_calibration_parameters(fs_input, camera_size, error, 
                camera_matrix, dist_coeffs, rvec, tvec);
            fs_input.release();
            if(fsiv_find_chessboard_corners(img_input, board_size, points2d)){
                fsiv_compute_camera_pose(points3d, points2d, camera_matrix, 
                    dist_coeffs, rvec, tvec);
            } else {
                std::cout << "Error finding chessboard corners" << std::endl;
                exit(-1);
            }

            cv::FileStorage fs_output (output_fname, cv::FileStorage::WRITE);
            fsiv_save_calibration_parameters(fs_output, camera_size, error, 
            camera_matrix, dist_coeffs, rvec, tvec);
            fs_output.release();

            //
            if (verbose)
            {
                //TODO
                //Show WCS axis.
                img_output = img_input.clone();
                fsiv_draw_axes(img_output, camera_matrix, dist_coeffs,
                    rvec, tvec, square_size, 3);
                cv::imshow("OUTPUT", img_output);
                int k = cv::waitKey() & 0xff;
                //
            }
        }
        else
        {
            //TODO
            //Make an intrisic calibration.
            //Remember: For each view (at least two) you must find the
            //chessboard to get the 3D -> 2D matches.

            if (input_fnames.size() < 2){
                std::cout << "More than 2 input needed" << std::endl;
                exit(-1);
            }

            for (int i = 0; i < input_fnames.size(); i++){
                img_input = cv::imread(input_fnames[i]);
                if (fsiv_find_chessboard_corners(img_input, board_size, points2d)){
                    camera_points.push_back(points2d);
                    world_points.push_back(points3d);
                } else {
                    std::cout << "Error finding chessboard corners" << std::endl;
                    exit(-1);
                }
            }

            std::vector<cv::Mat> rvecs, tvecs;
            camera_size = img_input.size();
            error = fsiv_calibrate_camera(camera_points, world_points, camera_size,
                camera_matrix, dist_coeffs, &rvecs, &tvecs);

            cv::FileStorage fs_output (output_fname, cv::FileStorage::WRITE);
            fsiv_save_calibration_parameters(fs_output, camera_size, error, 
                camera_matrix, dist_coeffs);
            fs_output.release();

            //

            if (verbose)
            {
                //TODO
                //Show WCS axis on each pattern view.
                for (int i = 0; i < input_fnames.size(); i++){
                    img_output = cv::imread(input_fnames[i]);
                    fsiv_draw_axes(img_output, camera_matrix, dist_coeffs,
                        rvecs[i], tvecs[i], square_size, 3) ;
                    cv::imshow("OUTPUT", img_output);
                    int k = cv::waitKey() & 0xff;
                }         
                //
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Capturada excepcion: " << e.what() << std::endl;
        retCode = EXIT_FAILURE;
    }
    return retCode;
}
