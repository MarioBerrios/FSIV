#include <iostream>
#include "common_code.hpp"

cv::Mat
fsiv_compute_histogram(const cv::Mat& in, cv::Mat& hist)
{
    CV_Assert(in.type()==CV_8UC1);
    CV_Assert(hist.empty() ||(hist.type()==CV_32FC1 &&
                              hist.rows==256 && hist.cols==1));
    //TODO
    //Tienes dos alternativas:
    //1- Implementar un recorrido por la imagen y calcular el histograma.
    //2- Usar la función cv::calcHist.
    //Sugerencia: implementa las dos para comparar.

    int histSize = 256;
    float range[] = {0, 256};
    const float* histRange = {range};
    cv::calcHist(&in, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);

    //

    CV_Assert(hist.type()==CV_32FC1);
    CV_Assert(hist.rows==256 && hist.cols==1);
    return hist;
}

void
fsiv_normalize_histogram(cv::Mat& hist)
{
    CV_Assert(hist.type()==CV_32FC1);
    CV_Assert(hist.rows==256 && hist.cols==1);

    //TODO

    cv::normalize(hist, hist, 1.0, 0.0, cv::NORM_L1);

    //
    CV_Assert(hist.type()==CV_32FC1);
    CV_Assert(hist.rows==256 && hist.cols==1);
    CV_Assert(cv::sum(hist)[0]==0.0 || cv::abs(cv::sum(hist)[0]-1.0)<=1.0e-6);
}

void
fsiv_accumulate_histogram(cv::Mat& hist)
{
    CV_Assert(hist.type()==CV_32FC1);
    CV_Assert(hist.rows==256 && hist.cols==1);
#ifndef NDEBUG
    double old_sum = cv::sum(hist)[0];
#endif


    //TODO

    /*cv::Mat aux = cv::Mat::zeros(hist.rows, hist.cols, hist.type());
    for(int i = 0 ; i < hist.rows; i++){
        aux.at<float>(i) = hist.at<float>(i) + aux.at<float>(i - 1);
    }

    hist = aux;
    */   
   
    for(int i = 1 ; i < hist.rows; i++){
        hist.at<float>(i) = hist.at<float>(i) + hist.at<float>(i - 1);
    }

    //

    CV_Assert(hist.type()==CV_32FC1);
    CV_Assert(hist.rows==256 && hist.cols==1);
    CV_Assert((old_sum<=0.0) ||
              ((std::abs(old_sum-hist.at<float>(255))/old_sum)<=1.0e-5));
}

cv::Mat
fsiv_create_equalization_lookup_table(const cv::Mat& hist,
                                      bool hold_median)
{
    CV_Assert(hist.type()==CV_32FC1);
    CV_Assert(hist.rows==256 && hist.cols==1);
    cv::Mat lkt;
    //TODO
    //Usa las funciones fsiv_normalize_histogram y fsiv_accumulate_histogram
    //para construir la tabla.

    lkt = hist;
    fsiv_normalize_histogram(lkt);
    fsiv_accumulate_histogram(lkt);

    if (hold_median){
        cv::Mat h1, h2;

        int pos = 0;
        for (int i = 0; i < lkt.rows; i++){
            if (lkt.at<float>(i)>=0.5){
                pos = i;
                break;
            }
        }

        h1 = lkt(cv::Rect(0, 0, 1, pos));
        h2 = lkt(cv::Rect(0, pos, 1, 256-pos));

        h1 = h1 * (pos/lkt.at<float>(pos));
        h2 = (h2 - lkt.at<float>(pos)) * (255 - pos)/(1 - lkt.at<float>(pos)) + pos;

        cv::vconcat(h1, h2, lkt);

    }

    lkt.convertTo(lkt, CV_8U, 255.0, 0.0);

    //

    CV_Assert(lkt.type()==CV_8UC1);
    CV_Assert(lkt.rows==256 && lkt.cols==1);
    return lkt;
}

cv::Mat
fsiv_apply_lookup_table(const cv::Mat&in, const cv::Mat& lkt,
                        cv::Mat& out)
{
    CV_Assert(in.type()==CV_8UC1);
    CV_Assert(lkt.type()==CV_8UC1);
    CV_Assert(lkt.rows==256 && lkt.cols==1);
    CV_Assert(out.empty() || (out.type()==CV_8UC1 &&
                              out.rows==in.rows && out.cols==in.cols));

    //TODO
    //Hint: you can use the cv::LUT function.

    cv::LUT(in, lkt, out);

    //
    CV_Assert(out.rows ==in.rows && out.cols==in.cols && out.type()==in.type());
    return out;
}

cv::Mat
fsiv_image_equalization(const cv::Mat& in, cv::Mat& out,
                    bool hold_median, int radius)
{
    CV_Assert(in.type()==CV_8UC1);
    //TODO
    //Utiliza las funciones fsiv_compute_histogram,
    //fsiv_create_equalization_lookup_table y fsiv_apply_lookup_table
    //Recuerda: cuando radius>0, la zona no procesada de la entrada se deja
    //   con los mismos valores en la salida.

    cv::Mat hist = cv::Mat::zeros(256, 1, CV_32FC1);
    cv::Mat lkt, window;
    if (radius > 0){
        out = in.clone();
        for (int i = 0; i <= in.rows - (2 * radius + 1); i++){
            for (int j = 0; j <= in.cols - (2 * radius + 1); j++){
                window = in(cv::Rect(j, i, 2 * radius + 1, 2 * radius + 1)).clone();
                fsiv_compute_histogram(window, hist);
                lkt = fsiv_create_equalization_lookup_table(hist);
                out.at<uchar>(i + radius, j + radius) = lkt.at<uchar>(window.at<uchar>(radius, radius));
            }
        }

    } else {
        fsiv_compute_histogram(in, hist);
        lkt = fsiv_create_equalization_lookup_table(hist);
        fsiv_apply_lookup_table(in, lkt, out);
    }
    
    //
    CV_Assert(out.rows==in.rows && out.cols==in.cols && out.type()==in.type());
    return out;
}
