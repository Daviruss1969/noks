#pragma once

#include "noks_game_object.hpp"

#include <Windows.h>

#include <string>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <vector>

namespace noks{
	struct NoksGameObjectPath {
		std::string path;
		std::string name;
		bool isDirectory;
	};

	class NoksIoManager{
	public:
		NoksIoManager();
		~NoksIoManager();

		void setCurrentProjectPath(std::string path) { currentProjectPath = path; }

		void saveProjectAs(std::string path/*, const NoksDataToSave& dataToSave*/);
		std::vector<noks::NoksGameObjectPath> updateObjectsPath();

	private:
		std::string currentProjectPath = "";
		std::string directoryAppPath = "";
		std::vector<noks::NoksGameObjectPath> gameObjectsPaths;
	};
}

