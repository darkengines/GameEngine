#include "Common.hpp"
#include <iostream>
#include <fstream>

namespace drk::Common {
	std::vector<char> ReadFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}
		auto size = file.tellg();
		std::vector<char> buffer(size);
		file.seekg(0);
		file.read(buffer.data(), size);
		file.close();
		return buffer;
	}
}