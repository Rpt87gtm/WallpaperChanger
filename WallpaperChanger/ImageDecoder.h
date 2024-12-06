#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

class ImageDecoder {
public:
    static cv::Mat Decode(const std::vector<char>& buffer);
};