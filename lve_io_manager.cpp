#include "lve_io_manager.hpp"
#include <iostream>

namespace lve {
	lve::LveIoManager::LveIoManager() {
		std::cout << "--------------" << std::endl;;
		directoryAppPath = std::filesystem::current_path().string();
		std::cout << "Current path is " << directoryAppPath << std::endl;
	}

	lve::LveIoManager::~LveIoManager(){

	}

	void lve::LveIoManager::saveProjectAs(std::string path/*, const LveDataToSave& dataToSave*/) {
		std::ofstream file(path);

		// what we want to write
		if (file.is_open()) {
			file << "this is a test\n";
			file.close();
		}
		else {
			std::cout << "unable to open file " << std::endl;
		}

		std::cout << "avant : " << path << std::endl;
		std::string defaultAssetsPath = directoryAppPath.append("\\models");

		// remove the file name and add the directory
		size_t index = path.rfind('\\');
		std::cout << "index : " << index << std::endl;
		currentProjectPath = path.substr(0, index);
		std::string targetAssetsPath = path.substr(0, index).append("\\models");
		std::cout << "Project path is : " << currentProjectPath << std::endl;
		std::cout << "Assets path is : " << defaultAssetsPath << std::endl;
		std::cout << "Target assets path is : " << targetAssetsPath << std::endl;
		std::filesystem::copy(directoryAppPath, targetAssetsPath, std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive);
	}

	std::vector<lve::GameObjectPath> LveIoManager::updateObjectsPath() {
		gameObjectsPaths.clear();
		std::cout << "avant" << std::endl;
		GameObjectPath gameObjectPath;
		gameObjectPath.isDirectory = true;
		gameObjectPath.name = "..";
		gameObjectPath.path = std::filesystem::path(currentProjectPath).parent_path().string();
		std::cout << "hehehehea ;" << gameObjectPath.path << std::endl;
		gameObjectsPaths.push_back(gameObjectPath);
		for (const auto& entry : std::filesystem::directory_iterator(currentProjectPath)) {
			GameObjectPath gameObjectPath;
			if (entry.is_directory() || entry.path().extension().string() == ".obj") {
				gameObjectPath.isDirectory = entry.is_directory();
				gameObjectPath.path = entry.path().string();
				gameObjectPath.name = entry.path().filename().string();
				std::cout << entry << std::endl;
				gameObjectsPaths.push_back(gameObjectPath);
			}
		}

		return gameObjectsPaths;
	}
};
