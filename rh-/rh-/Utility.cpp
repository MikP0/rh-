#include "pch.h"
#include "Utility.h"
#include <algorithm>
#include <exception>
#include <Shlwapi.h>
#include <fstream>

void Utility::LoadBinaryFile(const std::wstring& filename, std::vector<char>& data)
{
	std::ifstream file(filename.c_str(), std::ios::binary);
	if (file.bad())
	{
		throw std::exception("Could not open file.");
	}

	file.seekg(0, std::ios::end);
	UINT size = (UINT)file.tellg();

	if (size > 0)
	{
		data.resize(size);
		file.seekg(0, std::ios::beg);
		file.read(&data.front(), size);
	}

	file.close();
}

void Utility::ToWideString(const std::string& source, std::wstring& dest)
{
	dest.assign(source.begin(), source.end());
}