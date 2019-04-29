#pragma once

#include "pch.h"

class Utility
{
public:
	static void LoadBinaryFile(const std::wstring& filename, std::vector<char>& data);
	static void ToWideString(const std::string& source, std::wstring& dest);
private:
	Utility();
	Utility(const Utility& rhs);
	Utility& operator=(const Utility& rhs);
};
