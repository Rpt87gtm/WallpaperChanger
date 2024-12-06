#include <iostream>
#include <filesystem>
#include <thread>
#include "RandomFile.h"
#include "Wallpaper.h"

namespace fs = std::filesystem;

void printUsage() {
    std::cout << "Usage: WallpaperChanger <folderPath> <outputDir> <delaySeconds> <numIntermediates> <transitionDelay>" << std::endl;
    std::cout << "Example: WallpaperChanger C:\\Users\\rpt87\\Pictures\\Wallpapers C:\\Users\\rpt87\\Pictures\\IntermediateImages 10 5 10" << std::endl;
}


int main(int argc, char* argv[]) {
    fs::path folderPath = "C:\\Users\\rpt87\\Pictures\\Wallpapers";  
    fs::path outputDir = "C:\\Users\\rpt87\\Pictures\\IntermediateImages";
    int delaySeconds = 10;
    int numIntermediates = 5;  
    int transitionDelay = 10; 

    if (argc > 1) {
        if (argc != 6) {
            printUsage();
            return 1;
        }

        folderPath = argv[1];
        outputDir = argv[2];
        delaySeconds = std::stoi(argv[3]);
        numIntermediates = std::stoi(argv[4]);
        transitionDelay = std::stoi(argv[5]);
    }

    if (!fs::exists(folderPath)) {
        std::cerr << "Error: Folder path does not exist: " << folderPath << std::endl;
        return 1;
    }

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