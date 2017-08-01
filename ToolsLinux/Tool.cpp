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

#include "Tool.hpp"

#include <boost/optional/optional.hpp>
#include <iostream>
#include <codecvt>

#include <ToolsCross/Log.hpp>
#include <ToolsCross/ToolsException.hpp>
#include <ToolsCross/ScopedAction.hpp>

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

namespace fs = boost::filesystem;

namespace Tools
{
	namespace
	{
		std::string GetModuleFileName()
		{
			char buff[40 * 1000];
			ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);
			if (len != -1)
			{
				buff[len] = '\0';
				return std::string(buff);
			}
			THROW("Cannot get current executable path.");
		}

		//-----------------------------------------------------------------------------
		fs::path GetExecutablePath()
		{
			return fs::path { GetModuleFileName() };
		}

		//-----------------------------------------------------------------------------
		fs::path GetExecutableFolder()
		{
			fs::path executablePath = GetExecutablePath();

			return executablePath.parent_path();
		}

		//-------------------------------------------------------------------------
		std::string ToString(const std::wstring& str)
		{
			if (str.empty())
				return{};

            const wchar_t* pt;
            char buffer [MB_CUR_MAX];
            int i, length;
            std::vector<char> resString;

            pt = str.c_str();
            while (*pt)
            {
                if ((length = wctomb(buffer, *pt)) < 1)
                {
                    break;
                }
                for (i = 0; i < length; ++i)
                {
                    resString.push_back(buffer[i]);
                }
                ++pt;
            }

            return {resString.begin(), resString.end()};
		}

		//-------------------------------------------------------------------------
		std::wstring ToWString(const std::string& str)
		{
			if (str.empty())
				return{};

            const char* pt = str.c_str();
            size_t max = sizeof(str);
            std::vector<wchar_t> resString(max);
            wchar_t dest;
            int length;
            mbtowc(NULL, NULL, 0); /* reset mbtowc */

            while (max > 0) {
                length = mbtowc(&dest, pt, max);
                if (length < 1) {
                    break;
                }
                resString.push_back(dest);
                pt += length;
                max -= length;
            }

			return std::wstring{resString.begin(), resString.end()};
		}
	}

	//-------------------------------------------------------------------------
	std::string ToLocalString(const std::wstring& str)
	{
		return ToString(str);
	}

	//-------------------------------------------------------------------------
	std::string ToUtf8String(const std::wstring& str)
	{
		return ToString(str);
	}

	//-------------------------------------------------------------------------
	std::wstring LocalToWString(const std::string& str)
	{
		return ToWString(str);
	}

	//-------------------------------------------------------------------------
	std::wstring Utf8ToWString(const std::string& str)
	{
		return ToWString(str);
	}

	//-------------------------------------------------------------------------
	boost::filesystem::path GetTemplateFolder()
	{
		return GetExecutableFolder() / "Template";
	}

	//-------------------------------------------------------------------------
	boost::optional<std::wstring> Try(std::function<void()> action)
	{
		try
		{
			action();
		}
		catch (const std::exception& e)
		{
			return Tools::LocalToWString(e.what());
		}
		catch (...)
		{
			return boost::optional<std::wstring>(L"Unkown exception");
		}

		return boost::none;
	}

	//-------------------------------------------------------------------------
	void ShowOutputMessage(const std::wstring& message, const boost::filesystem::path& path)
	{
		LOG_INFO << GetSeparatorLine();
		LOG_INFO << message << path.wstring();
		LOG_INFO << GetSeparatorLine();
	}

	//-------------------------------------------------------------------------
	std::wstring GetSeparatorLine()
	{
		return L"----------------------------------------------------";
	}
	
	//---------------------------------------------------------------------
	void CreateParentFolderIfNeeded(const boost::filesystem::path& path)
	{
		if (path.has_parent_path())
		{
			auto parentPath = path.parent_path();
			boost::system::error_code er;

			boost::filesystem::create_directories(parentPath, er);
			if (er)
			{
				THROW(L"Error when creating folder " << parentPath.wstring()
					<< L" Error code:" << er.value());
			}
		}
	}
}