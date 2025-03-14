#include "hdn_pipeline.h"

#include <fstream>
#include <stdexcept>
#include <iostream>

namespace hdn {
	HdnPipeline::HdnPipeline(const std::string& vertFilepath, const std::string& fragFilepath)
	{
		createGraphicsPipeline(vertFilepath, fragFilepath);
	}

	std::vector<char> HdnPipeline::readFile(const std::string& filepath)
	{
		std::ifstream file{ filepath, std::ios::ate | std::ios::binary };

		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file: " + filepath);
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

	void HdnPipeline::createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath)
	{
		std::vector<char> vertCode = readFile(vertFilepath);
		std::vector<char> fragCode = readFile(fragFilepath);

		std::cout << "Vertex Shader Code Size: " << vertCode.size() << "\n";
		std::cout << "Fragment Shader Code Size: " << fragCode.size() << "\n";
	}
}
