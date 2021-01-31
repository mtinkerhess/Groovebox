#include "Disk.h"

#include <filesystem>
#include <fstream>
#include <sstream>

using namespace std;
using namespace std::filesystem;

void Disk::CreateDirIfNotExists(const std::string& dirName)
{
	create_directory(path(dirName));
}

void Disk::WriteToFile(const std::wstring& pathToFile, const std::string& fileContents)
{
    ofstream os;
    os.open(pathToFile);
    os << fileContents;
    os.close();
}

std::string Disk::ReadFromFile(const std::wstring& pathToFile)
{
    stringstream contents;
    ifstream is;
    is.open(pathToFile);
    
    string line;
    while (getline(is, line))
    {
        contents << line << endl;
    }

    is.close();
    return contents.str();
}