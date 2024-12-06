#include <iostream>
#include <fstream>
#include <filesystem>
#include <random>
#include <thread>
#include <opencv2/opencv.hpp>

namespace fs = std::filesystem;

void darkenImage(cv::Mat& image, float darknessFactor = 0.5) {
    image.convertTo(image, -1, darknessFactor,0);
}

void setWallpaper(const std::string& imagePath) {
    std::string command = "powershell -command \"Add-Type -TypeDefinition 'using System; using System.Runtime.InteropServices; public class Wallpaper { [DllImport(\\\"C:\\\\Windows\\\\System32\\\\user32.dll\\\", CharSet=CharSet.Auto)] public static extern int SystemParametersInfo(int uAction, int uParam, string lpvParam, int fuWinIni); }'; [Wallpaper]::SystemParametersInfo(20, 0, '" + imagePath + "', 0x01);\"";
    std::cout << imagePath << std::endl;
    system(command.c_str());
}

std::string getRandomImage(const fs::path& folderPath) {
    std::vector<std::string> imagePaths;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.path().extension() == ".jpg" || entry.path().extension() == ".png") {
            imagePaths.push_back(entry.path().string());
        }
    }

    if (imagePaths.empty()) {
        throw std::runtime_error("No images found in the specified folder.");
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, imagePaths.size() - 1);
    return imagePaths[dis(gen)];
}

cv::Mat loadImageFromRawData(const std::string& imagePath) {
    std::ifstream file(imagePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Could not open the file: " << imagePath << std::endl;
        return cv::Mat();
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);

    if (!file.read(buffer.data(), size)) {
        std::cerr << "Could not read the file: " << imagePath << std::endl;
        return cv::Mat();
    }

    cv::Mat rawData(1, size, CV_8UC1, buffer.data());
    cv::Mat image = cv::imdecode(rawData, cv::IMREAD_COLOR);

    if (image.empty()) {
        std::cerr << "Could not decode the image" << std::endl;
        return cv::Mat();
    }

    return image;
}


void createIntermediateImages(const std::string& imagePath, std::vector<std::string>& intermediateShowPaths, std::vector<std::string>& intermediateHidePaths, int numIntermediates, const fs::path& outputDir, const bool isFirstSet) {
    cv::Mat image = loadImageFromRawData(imagePath);
    if (image.empty()) {
        throw std::runtime_error("Failed to load image: " + imagePath);
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
    while (true) {
        try {
            std::string imagePath = getRandomImage(folderPath);
            std::cout << "Selected image: " << imagePath << std::endl;

            std::vector<std::string> intermediateShowPaths;
            std::vector<std::string> intermediateHidePaths;
            createIntermediateImages(imagePath, intermediateShowPaths, intermediateHidePaths, numIntermediates, outputDir, isFirstSet);
            isFirstSet = !isFirstSet;

            smoothWallpaperTransition(intermediateShowPaths, intermediateHidePaths, transitionDelay);

            setWallpaper(imagePath);
            std::this_thread::sleep_for(std::chrono::seconds(delaySeconds));
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}