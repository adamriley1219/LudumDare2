#include "Engine/Core/EngineUtils.hpp"
#include <fstream>
#include <thread>


//--------------------------------------------------------------------------
/**
* GetSystemCoreCount
*/
uint GetSystemCoreCount()
{
	return std::thread::hardware_concurrency();
}


//--------------------------------------------------------------------------
/**
* LoadMap
*/
unsigned long int LoadFile(const char* filePath, char*& output)
{
	std::ifstream file;
	file.open(filePath, std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		unsigned long int size = static_cast<unsigned long int>(file.tellg());
		file.seekg(std::ios::beg);
		output = new char[size];
		file.read(output, size);
		return size;
	}
	else
	{
		ERROR_RECOVERABLE("Could not open file.");
		return 0;
	}
}
