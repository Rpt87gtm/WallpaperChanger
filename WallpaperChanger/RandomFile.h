#ifndef WALLPAPERCHANGER_H
#define WALLPAPERCHANGER_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <random>
#include "IRandomGenerator.h"
#include "RandomGenerator.h"

template<typename T>
class RandomFile
{
public:
    RandomFile(const std::filesystem::path& folderPath, IRandomGenerator* generator = nullptr)
        : folderPath(folderPath), generator(generator ? generator : new RandomGenerator()) {
        Next();
    }

    ~RandomFile() {
        delete generator;
    }

    virtual std::string Path() const {
        return currentFilePath.string();
    }


    virtual void Next() {
        std::vector<std::filesystem::path> files;
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path());
            }
        }

        if (files.empty()) {
            throw std::runtime_error("No files found in the folder: " + folderPath.string());
        }

        int randomIndex = generator->GetRandomIndex(files.size() - 1);
        currentFilePath = files[randomIndex].string();
    };

    virtual std::vector<T> Data() {
        std::ifstream file(currentFilePath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open the file: " + currentFilePath.string());
        }

        std::streamsize size = file.tellg();
        if (size <= 0) {
            throw std::runtime_error("File size is invalid: " + currentFilePath.string());
        }

        file.seekg(0, std::ios::beg);

        std::vector<T> buffer(size / sizeof(T));

        if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
            throw std::runtime_error("Could not read the file: " + currentFilePath.string());
        }

        return buffer;
    }
protected:
    std::filesystem::path folderPath;
    std::filesystem::path currentFilePath;
    IRandomGenerator* generator;
};

class RandomImageFile : public RandomFile<char>
{
public:
    RandomImageFile(const std::filesystem::path& folderPath, IRandomGenerator* generator = nullptr)
        : RandomFile<char>(folderPath, generator ? generator : new RandomGenerator()) {}

    void Next() override {
        std::vector<std::filesystem::path> imageFiles;
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && (entry.path().extension() == ".jpg" || entry.path().extension() == ".png")) {
                imageFiles.push_back(entry.path());
            }
        }

        if (imageFiles.empty()) {
            throw std::runtime_error("No image files found in the folder: " + folderPath.string());
        }

        int randomIndex = generator->GetRandomIndex(imageFiles.size() - 1);
        currentFilePath = imageFiles[randomIndex].string();
    }
};
#endif WALLPAPERCHANGER_H
