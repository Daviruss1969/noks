﻿#include "noks_io_manager.hpp"
#include <iostream>

namespace noks {
	noks::NoksIoManager::NoksIoManager() {
		directoryAppPath = std::filesystem::current_path().string();
	}

	noks::NoksIoManager::~NoksIoManager(){

	}

	void noks::NoksIoManager::saveProjectAs(std::string path/*, const NoksDataToSave& dataToSave*/) {
		std::ofstream file(path);

		// what we want to write
		if (file.is_open()) {
			file << "this is a test\n";
			file.close();
		}
		else {
			std::cout << "unable to open file " << std::endl;
		}

		std::string defaultAssetsPath = directoryAppPath.append("\\models");

		// remove the file name and add the directory
		size_t index = path.rfind('\\');
		currentProjectPath = path.substr(0, index);
		std::string targetAssetsPath = path.substr(0, index).append("\\models");
		std::filesystem::copy(directoryAppPath, targetAssetsPath, std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive);
	}

	std::vector<noks::NoksGameObjectPath> NoksIoManager::updateObjectsPath() {
		gameObjectsPaths.clear();
		NoksGameObjectPath gameObjectPath;
		gameObjectPath.isDirectory = true;
		gameObjectPath.name = "..";
		gameObjectPath.path = std::filesystem::path(currentProjectPath).parent_path().string();
		gameObjectsPaths.push_back(gameObjectPath);
		for (const auto& entry : std::filesystem::directory_iterator(currentProjectPath)) {
			NoksGameObjectPath gameObjectPath;
			if (entry.is_directory() || entry.path().extension().string() == ".obj") {
				gameObjectPath.isDirectory = entry.is_directory();
				gameObjectPath.path = entry.path().string();
				gameObjectPath.name = entry.path().filename().string();
				gameObjectsPaths.push_back(gameObjectPath);
			}
		}

		return gameObjectsPaths;
	}
};
