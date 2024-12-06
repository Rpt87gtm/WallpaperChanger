#pragma once
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "RandomFile.h"
class Wallpaper
{
public:
	Wallpaper(const std::filesystem::path& outputDir);
	~Wallpaper();
	void createIntermediateImages(RandomImageFile& imageFile, int numIntermediates);
	void setWallpaper(const std::string& imagePath);
	void darkenImage(cv::Mat& image, float darknessFactor);
	void smoothWallpaperTransition( int delayMilliseconds);
protected:
	bool isFirstSet;
	std::filesystem::path outputDir;
	std::vector<std::string> intermediateShowPaths;
	std::vector<std::string> intermediateHidePaths;
};

