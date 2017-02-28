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

#if defined(__CYGWIN__)
#include <cstring>
#else
#include <string>
#endif

#include <vector>
#include <ToolsCross/Tool.hpp>
#include "FileCoverage.hpp"

namespace CppCoverage
{
    //-------------------------------------------------------------------------
    FileCoverage::FileCoverage(const std::wstring& path)
            : path_(path)
    {
    }

    //    Notice the side effect on the string.
    //    This is necessary unless we return the new string after the erase.
    //    Doing this will introduce a struct I feel not necessary.
    //    We delete the characters up to and including the delimiter.
    unsigned int FileCoverage::GetToken(std::string& line)
    {
        std::string delimiter = ",";
        size_t pos = line.find(delimiter);
        unsigned long token = stoul(line.substr(0, pos));
        line.erase(0, pos + delimiter.length());
        return (unsigned int) token;
    }

    //-------------------------------------------------------------------------
    bool FileCoverage::AddLine(const std::wstring line)
    {
        if (strncmp(Tools::ToUtf8String(line).c_str(), "e", 1) == 0)
        {
            return false;
        }
        else
        {
            std::string tokenLine = Tools::ToUtf8String(line.substr(3, line.length() - 1));
            unsigned int lineNumber = GetToken(tokenLine);
            updateLineNumber(lineNumber);
            bool hasBeenExecuted = GetToken(tokenLine) >= 1;
            LineCoverage lineCov{lineNumber, hasBeenExecuted};

            if (!lines_.emplace(lineNumber, lineCov).second)
            {
                // TODO: Line has already been added before. Wrong format
                // std::cerr << lineNumber << " already exists for " << path.c_str());
                // stop parsing
            }
        }
        return true;
    }

    //-------------------------------------------------------------------------
    void FileCoverage::UpdateLine(unsigned int lineNumber, bool hasBeenExecuted)
    {
        if (!lines_.erase(lineNumber))
            throw std::runtime_error(Tools::ToUtf8String(L"Line " + std::to_wstring(lineNumber)
                                     + L" does not exists and cannot be updated for " + path_));

        LineCoverage lineCov{lineNumber, hasBeenExecuted};
        lines_.emplace(lineNumber, lineCov);
    }

    //-------------------------------------------------------------------------
    const std::wstring &FileCoverage::GetPath() const
    {
        return path_;
    }

    //-------------------------------------------------------------------------
    const LineCoverage *FileCoverage::operator[](unsigned int line) const
    {
        auto it = lines_.find(line);

        return it == lines_.end() ? 0 : &it->second;
    }

    //-------------------------------------------------------------------------
    std::vector<LineCoverage> FileCoverage::GetLines() const
    {
        std::vector<LineCoverage> lines;

        for (const auto &pair : lines_)
        {
            lines.push_back(pair.second);
        }

        return lines;
    }

    const unsigned int FileCoverage::GetLastLineNumber() const
    {
        return lastLine_;
    }

    void FileCoverage::updateLineNumber(unsigned int number)
    {
        if ( lastLine_ < number )
        {
            lastLine_ = number;
        }
    }
}
