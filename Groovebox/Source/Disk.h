#pragma once

#include <string>

namespace Disk
{
	void CreateDirIfNotExists(const std::string& dirName);

	void WriteToFile(const std::wstring& pathToFile, const std::string& fileContents);

	std::string ReadFromFile(const std::wstring& pathToFile);
}