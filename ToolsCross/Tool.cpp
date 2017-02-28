// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "Tool.hpp"

#include <iostream>
#include <codecvt>
#include <vector>

namespace Tools
{
	namespace
	{
		//-------------------------------------------------------------------------
		std::string ToString(unsigned int pageCode, const std::wstring& str)
		{
			if (str.empty())
				return{};

			auto size = WideCharToMultiByte(pageCode, 0, str.c_str(),
				static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);
			std::vector<char> buffer(size);

			if (!WideCharToMultiByte(pageCode, 0, str.c_str(), static_cast<int>(str.size()),
				&buffer[0], static_cast<int>(buffer.size()), nullptr, nullptr))
			{
				throw std::runtime_error("Error in WideCharToMultiByte.");
			}

			return{ buffer.begin(), buffer.end() };
		}

		//-------------------------------------------------------------------------
		std::wstring ToWString(unsigned int pageCode, const std::string& str)
		{
			if (str.empty())
				return{};

			auto size = MultiByteToWideChar(pageCode, 0,
				str.c_str(), static_cast<int>(str.size()), nullptr, 0);
			std::vector<wchar_t> buffer(size);

			if (!MultiByteToWideChar(pageCode, 0, str.c_str(), static_cast<int>(str.size()),
				&buffer[0], static_cast<int>(buffer.size())))
			{
				throw std::runtime_error("Error in MultiByteToWideChar for " + str);
			}

			return{ buffer.begin(), buffer.end() };
		}
	}

	//-------------------------------------------------------------------------
	std::string ToLocalString(const std::wstring& str)
	{
		return ToString(CP_ACP, str);
	}

	//-------------------------------------------------------------------------
	std::string ToUtf8String(const std::wstring& str)
	{
		return ToString(CP_UTF8, str);
	}

	//-------------------------------------------------------------------------
	std::wstring LocalToWString(const std::string& str)
	{
		return ToWString(CP_ACP, str);
	}

	//-------------------------------------------------------------------------
	std::wstring Utf8ToWString(const std::string& str)
	{
		return ToWString(CP_UTF8, str);
	}

	//-------------------------------------------------------------------------
	std::wstring GetSeparatorLine()
	{
		return L"----------------------------------------------------";
	}
}