#include "ImageDecoder.h"
#include <iostream>

cv::Mat ImageDecoder::Decode(const std::vector<char>& buffer) {
    int size = static_cast<int>(buffer.size());
    cv::Mat rawData(1, size, CV_8UC1, const_cast<char*>(buffer.data()));
    cv::Mat image = cv::imdecode(rawData, cv::IMREAD_COLOR);

    if (image.empty()) {
        std::cerr << "Could not decode the image" << std::endl;
        return cv::Mat();
    }

    return image;
}