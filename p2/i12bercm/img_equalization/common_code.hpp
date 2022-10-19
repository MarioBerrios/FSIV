#pragma once

//Includes para OpenCV, Descomentar según los módulo utilizados.
#include <opencv2/core/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/**
 * @brief Calcula el histograma de una imagen monocroma.
 * @param in es la imagen con formato 8UC1.
 * @param hist almacena el histograma calculado.
 * @return el histograma calculado.
 * @pre in.type()==CV_8UC1
 * @pre hist.empty()||(hist.type()==CV_32FC1 && hist.rows==256 && hist.cols==1)
 * @post hist.type()==CV_32FC1
 * @post hist.rows==256 && hist.cols==1
 */
cv::Mat fsiv_compute_histogram(const cv::Mat& in, cv::Mat& hist);

/**
 * @brief Normaliza el histogram de forma que su suma sea 1.0
 * @param hist el histograma a normalizar.
 * @pre hist.type()==CV_32FC1
 * @pre hist.rows==256 && hist.cols==1
 * @post hist.type()==CV_32FC1
 * @post hist.rows==256 && hist.cols==1
 * @post sum(hist)[0]==0.0 || abs(sum(hist)[0]-1.0)<=1.0e-6
 */
void fsiv_normalize_histogram(cv::Mat& hist);

/**
 * @brief acumula el histograma.
 * @param hist el histograma a acumular.
 * @pre hist.type()==CV_32FC1
 * @pre hist.rows==256 && hist.cols==1
 * @post hist.type()==CV_32FC1
 * @post hist.rows==256 && hist.cols==1
 * @post sum(old.hist)==0.0 || abs(sum(old.hist)-hist[255])/sum(old.hist) <= 1.0e-5
 */
void fsiv_accumulate_histogram(cv::Mat& hist);

/**
 * @brief Crea una tabla para realizar la ecualización de una imagen.
 * @param hist es el histograma de la imagen.
 * @param hold_median si es true, la mediana no se transformará.
 * @return la tabla creada.
 * @pre hist.type()==CV_32FC1
 * @pre hist.rows==256 && hist.cols==1
 * @post retval.type()==CV_8UC1
 * @post retval.rows==256 && retval.cols==1 
 */
cv::Mat fsiv_create_equalization_lookup_table(const cv::Mat& hist,
                                              bool hold_median=false);

/**
 * @brief Aplica una "lookup table"
 * @param in la imagen de entrada.
 * @param lkt la tabla.
 * @param out la imgen de salida.
 * @return la imagen de salida.
 * @pre in.type()==CV_8UC1
 * @pre lkt.type()==CV_8UC1
 * @pre lkt.rows==256 && lkt.cols==1
 * @pre out.empty() || (out.type()==CV_8UC1 && out.rows==in.rows && out.cols==in.cols)
 * @post out.rows ==in.rows && out.cols==in.cols && out.type()==in.type()
 */
cv::Mat fsiv_apply_lookup_table(const cv::Mat&in, const cv::Mat& lkt,
                        cv::Mat& out);

/**
 * @brief Ecualiza una imagen.
 * @param in es la imagen a ecualizar.
 * @param out es la imagen ecualizada.
 * @param hold_median si es cierto la mediana se transformá al mismo valor.
 * @param radius si es >0, se aplica ecualización local con ventanas de radio r.
 * @return la imagen ecualizada.
 * @warning si se aplica procesado local, el área de la imagen de entrada que
 * no puede ser procesada se copia directamente en la salida.
 */
cv::Mat fsiv_image_equalization(const cv::Mat& in, cv::Mat& out,
                            bool hold_median=false, int radius=0);
