#include <iostream>
#include <fstream>
#include <filesystem>
#include <random>
#include <thread>
#include <opencv2/opencv.hpp>
#include "RandomFile.h"
#include "ImageDecoder.h"
#include "Wallpaper.h"

namespace fs = std::filesystem;


int main() {
    const fs::path folderPath = "C:\\Users\\rpt87\\Pictures\\Wallpapers";  
    const fs::path outputDir = "C:\\Users\\rpt87\\Pictures\\IntermediateImages";
    const int delaySeconds = 10;
    const int numIntermediates = 5;  
    const int transitionDelay = 10; 


    RandomImageFile randomFile = RandomImageFile(folderPath);
    Wallpaper wallpaper(outputDir);
    wallpaper.createIntermediateImages(randomFile, numIntermediates);
    while (true) {
        try {
            randomFile.Next();
            std::cout << "Selected image: " << randomFile.Path() << std::endl;

            
            wallpaper.smoothWallpaperTransition(transitionDelay);
            wallpaper.createIntermediateImages(randomFile, numIntermediates);
            std::this_thread::sleep_for(std::chrono::seconds(delaySeconds));
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}