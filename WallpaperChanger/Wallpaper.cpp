#include "Wallpaper.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "ImageDecoder.h"


Wallpaper::Wallpaper(const std::filesystem::path& outputDir)
    : isFirstSet(true), outputDir(outputDir) {
    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directories(outputDir);
    }
}

Wallpaper::~Wallpaper() {

}

void Wallpaper::createIntermediateImages(RandomImageFile& imageFile, int numIntermediates) {

    cv::Mat image = ImageDecoder::Decode(imageFile.Data());
    if (image.empty()) {
        throw std::runtime_error("Failed to load image: " + imageFile.Path());
    }

    std::string ShowSetName = isFirstSet ? "intermediateSet1_" : "intermediateSet2_";
    std::string HideSetName = !isFirstSet ? "intermediateSet1_" : "intermediateSet2_";

    for (int i = 0; i < numIntermediates; ++i) {
        float darknessFactor = (i + 1) / static_cast<float>(numIntermediates + 1);
        cv::Mat darkenedImage = image.clone();
        darkenImage(darkenedImage, darknessFactor);

        std::string intermediateShowPath = (outputDir / (ShowSetName + std::to_string(i) + ".jpg")).string();
        std::string intermediateHidePath = (outputDir / (HideSetName + std::to_string(numIntermediates - i - 1) + ".jpg")).string();
        cv::imwrite(intermediateShowPath, darkenedImage);

        intermediateShowPaths.push_back(intermediateShowPath);
        intermediateHidePaths.push_back(intermediateHidePath);
    }

    isFirstSet = !isFirstSet;
}

void Wallpaper::setWallpaper(const std::string& imagePath) {
    std::string command = "powershell -command \"Add-Type -TypeDefinition 'using System; using System.Runtime.InteropServices; public class Wallpaper { [DllImport(\\\"C:\\\\Windows\\\\System32\\\\user32.dll\\\", CharSet=CharSet.Auto)] public static extern int SystemParametersInfo(int uAction, int uParam, string lpvParam, int fuWinIni); }'; [Wallpaper]::SystemParametersInfo(20, 0, '" + imagePath + "', 0x01);\"";
    std::cout << imagePath << std::endl;
    system(command.c_str());
}

void Wallpaper::darkenImage(cv::Mat& image, float darknessFactor) {
    image.convertTo(image, -1, darknessFactor, 0);
}

void Wallpaper::smoothWallpaperTransition(int delayMilliseconds) {
    for (const auto& path : intermediateHidePaths) {
        setWallpaper(path);
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMilliseconds));
    }

    for (const auto& path : intermediateShowPaths) {
        setWallpaper(path);
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMilliseconds));
    }

    intermediateShowPaths.clear();
    intermediateHidePaths.clear();
}