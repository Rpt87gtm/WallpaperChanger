#include <iostream>
#include <fstream>
#include <filesystem>
#include <random>
#include <thread>
#include <opencv2/opencv.hpp>
#include "RandomFile.h"
#include "ImageDecoder.h"

namespace fs = std::filesystem;

void darkenImage(cv::Mat& image, float darknessFactor = 0.5) {
    image.convertTo(image, -1, darknessFactor,0);
}

void setWallpaper(const std::string& imagePath) {
    std::string command = "powershell -command \"Add-Type -TypeDefinition 'using System; using System.Runtime.InteropServices; public class Wallpaper { [DllImport(\\\"C:\\\\Windows\\\\System32\\\\user32.dll\\\", CharSet=CharSet.Auto)] public static extern int SystemParametersInfo(int uAction, int uParam, string lpvParam, int fuWinIni); }'; [Wallpaper]::SystemParametersInfo(20, 0, '" + imagePath + "', 0x01);\"";
    std::cout << imagePath << std::endl;
    system(command.c_str());
}





void createIntermediateImages(RandomImageFile& imageFile, std::vector<std::string>& intermediateShowPaths, std::vector<std::string>& intermediateHidePaths, int numIntermediates, const fs::path& outputDir, const bool isFirstSet) {

    cv::Mat image = ImageDecoder::Decode(imageFile.Data());
    if (image.empty()) {
        throw std::runtime_error("Failed to load image: " + imageFile.Path());
    }

    std::string ShowSetName = isFirstSet ? "intermediateSet1_" : "intermediateSet2_";
    std::string HideSetName = !isFirstSet ? "intermediateSet1_" : "intermediateSet2_";

    for (int i = 0; i < numIntermediates; ++i) {
        float darknessFactor =  (i + 1) / static_cast<float>(numIntermediates + 1);
        cv::Mat darkenedImage = image.clone();
        darkenImage(darkenedImage, darknessFactor);

        std::string intermediateShowPath = (outputDir / (ShowSetName + std::to_string(i) + ".jpg")).string();
        std::string intermediateHidePath = (outputDir / (HideSetName + std::to_string(numIntermediates - i - 1) + ".jpg")).string();
        cv::imwrite(intermediateShowPath, darkenedImage);

        intermediateShowPaths.push_back(intermediateShowPath);
        intermediateHidePaths.push_back(intermediateHidePath);
    }
}

void smoothWallpaperTransition(const std::vector<std::string>& intermediateShowPaths, const std::vector<std::string>& intermediateHidePaths, int delayMilliseconds) {
    for (const auto& path : intermediateHidePaths) {
        setWallpaper(path);
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMilliseconds));
    }

    for (const auto& path : intermediateShowPaths) {
        setWallpaper(path);
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMilliseconds));
    }
}

int main() {
    const fs::path folderPath = "C:\\Users\\rpt87\\Pictures\\Wallpapers";  
    const fs::path outputDir = "C:\\Users\\rpt87\\Pictures\\IntermediateImages";
    const int delaySeconds = 10;
    const int numIntermediates = 5;  
    const int transitionDelay = 10; 

    if (!fs::exists(outputDir)) {
        fs::create_directories(outputDir);
    }

    bool isFirstSet = true;
    RandomImageFile randomFile = RandomImageFile(folderPath);

    while (true) {
        try {
            
            std::cout << "Selected image: " << randomFile.Path() << std::endl;

            std::vector<std::string> intermediateShowPaths;
            std::vector<std::string> intermediateHidePaths;
            createIntermediateImages(randomFile, intermediateShowPaths, intermediateHidePaths, numIntermediates, outputDir, isFirstSet);
            isFirstSet = !isFirstSet;

            smoothWallpaperTransition(intermediateShowPaths, intermediateHidePaths, transitionDelay);

            setWallpaper(randomFile.Path());
            std::this_thread::sleep_for(std::chrono::seconds(delaySeconds));
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}