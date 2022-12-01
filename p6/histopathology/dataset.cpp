/**
 *  @file dataset.hpp
 *  (C) 2022- FJMC fjmadrid@uco.es
 */
#include <iostream>
#include <exception>
#include <fstream>
#include <random>
#include <algorithm>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "dataset.hpp"


void split_path_name(const std::string& pathname, std::string& dirname,
                     std::string& fname)
{
    size_t pos;
    pos = pathname.find_last_of('/');
    dirname = "";
    fname = pathname;
    if (pos != std::string::npos)
    {
        dirname = pathname.substr(0, pos);
        fname = pathname.substr(pos+1);
    }
}

float
compute_file_size(std::string const& fname, const long units)
{
    float size = -1.0;
    std::ifstream file (fname);
    if (file)
    {
        file.seekg (0, file.end);
        long length = file.tellg();
        size = static_cast<float>(length) / static_cast<float>(units);
    }
    return size;
}

Dataset::Dataset()
{
    class_label_names_ = {"Normal", "Serrated", "Adenocarcinoma", "Adenoma"};
}

int
Dataset::num_samples() const
{
    int num = 0;
    if (!X_.empty())
        num = X_.rows;
    return num;
}

int
Dataset::load(const std::string& dataset_file, std::uint64_t rseed)
{
    int ret_code = 0;
    std::string dt_dirname, dt_fname;
    split_path_name(dataset_file, dt_dirname, dt_fname);
    std::ifstream in (dataset_file);
    if (in)
    {
        std::string line;
        std::vector<std::string> lines;
        while (std::getline(in, line))
            lines.push_back(line);
        if (rseed != 0)
        {
            auto rng = std::default_random_engine(rseed);
            std::shuffle(lines.begin(), lines.end(), rng);
        }
        if (in.eof())
        {
            X_ = cv::Mat(lines.size(), 128*128, CV_8UC3);
            y_ = cv::Mat(lines.size(), 1, CV_32S);
            std::istringstream line;
            std::string fname;
            cv::Mat img;
            int label;
            for (size_t i = 0; i<lines.size() && ret_code==0; ++i)
            {
                line.clear();
                line.str(lines[i]);
                line >> fname >> label;
                if (!line)
                    ret_code = 4+i;
                else
                {
                    img = cv::imread(dt_dirname+'/'+fname, cv::IMREAD_COLOR);
                    if (img.empty())
                        ret_code = 4+i;
                    else
                    {
                        img.reshape(3, 1).copyTo(X_.row(i));
                        y_.at<int>(i) = label;
                    }
                }
            }
        }
        else
            ret_code = -2;
    }
    else
        ret_code = -1;
    return ret_code;
}

void
Dataset::split(float train_p, cv::Mat& train_images, cv::Mat& train_labels,
               cv::Mat& validation_images, cv::Mat& validation_labels) const
{
    CV_Assert(num_samples()>1);
    int train_size = (X_.rows * train_p) /100.0;
    train_images = X_.rowRange(0, train_size);
    train_labels = y_.rowRange(0, train_size);
    validation_images = X_.rowRange(train_size, X_.rows);
    validation_labels = y_.rowRange(train_size, y_.rows);
}

size_t
Dataset::memory_size() const
{
    size_t size = 0;
    if (num_samples()>0)
    {
        size = X_.elemSize()*X_.rows*X_.cols;
        size += y_.elemSize()*y_.rows;
    }
    return size;
}

const std::vector<std::string>&
Dataset::class_label_names() const
{
    return class_label_names_;
}

const cv::Mat&
Dataset::X() const
{
    return X_;
}

cv::Mat&
Dataset::X()
{
    return X_;
}

const cv::Mat&
Dataset::y() const
{
    return y_;
}

cv::Mat&
Dataset::y()
{
    return y_;
}

