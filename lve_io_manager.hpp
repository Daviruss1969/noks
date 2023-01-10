#pragma once

#include "lve_game_object.hpp"

#include <Windows.h>

#include <string>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <vector>

namespace lve{
	struct GameObjectPath {
		std::string path;
		bool isDirectory;
	};

	class LveIoManager{
	public:
		LveIoManager();
		~LveIoManager();

		void saveProjectAs(std::string path/*, const LveDataToSave& dataToSave*/);
		std::vector<lve::GameObjectPath> updateObjectsPath();

	private:
		std::string currentProjectPath = "";
		std::string directoryAppPath = "";
		std::vector<lve::GameObjectPath> gameObjectsPaths;
	};
}

