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

//#include "stdafx.h"
#include "HtmlFolderStructure.hpp"

#include <boost/algorithm/string.hpp>

namespace fs = boost::filesystem;

namespace Exporter
{
	namespace
	{
		//---------------------------------------------------------------------
		void CopyRecursiveDirectoryContent(
			const fs::path& from,
			const fs::path& to)
		{
			fs::create_directories(to);

			for (fs::recursive_directory_iterator it(from);
				it != fs::recursive_directory_iterator(); ++it)
			{
				const auto& path = it->path();
				std::string destination = path.string();
				
				boost::replace_first(destination, from.string(), to.string());

				if (fs::is_directory(path))
					fs::create_directories(destination);
				else
					fs::copy_file(path, destination, fs::copy_option::overwrite_if_exists);
			}
		}

        // TODO: START FROM HERE
        std::string GetUniquePath(const std::string& prefix)
        {
            std::string uniquePath = prefix;

            for (int i = 2; fs::exists(uniquePath); ++i)
                uniquePath = prefix + '-' + std::to_string(i);

            return uniquePath;
        }

		//---------------------------------------------------------------------
		fs::path CreateUniqueDirectories(const fs::path& initialPath)
		{
            fs::path uniquePath = GetUniquePath(initialPath.string());

            fs::create_directories(uniquePath);

			return uniquePath;
		}
	}

	//-------------------------------------------------------------------------
	const std::wstring HtmlFolderStructure::ThirdParty = L"third-party";
	const std::wstring HtmlFolderStructure::FolderModules = L"Modules";

	//-------------------------------------------------------------------------
	HtmlFolderStructure::HtmlFolderStructure(const boost::filesystem::path& templateFolder)
		: templateFolder_(templateFolder)
	{
	}

	//-------------------------------------------------------------------------
	boost::filesystem::path HtmlFolderStructure::CreateCurrentRoot(const std::string& outputFolder)
	{
		currentRoot_ = CreateUniqueDirectories(fs::absolute(fs::path(outputFolder)));
		CopyRecursiveDirectoryContent(
			templateFolder_ / HtmlFolderStructure::ThirdParty,
			*currentRoot_ / HtmlFolderStructure::ThirdParty);

		return *currentRoot_;
	}

	//-------------------------------------------------------------------------
	HtmlFile HtmlFolderStructure::CreateCurrentModule(const boost::filesystem::path& modulePath)
	{
		if (!currentRoot_)
			throw std::runtime_error("No root is selected");

		boost::filesystem::path moduleFilename = modulePath.filename();
        std::string moduleName = moduleFilename.replace_extension("").string(); // remove extension
		auto modulesPath = *currentRoot_ / HtmlFolderStructure::FolderModules;
		currentModule_ = CreateUniqueDirectories(modulesPath / moduleName);

		std::string moduleHtmlName = moduleName + ".html";
		auto moduleHtmlPath = GetUniquePath(modulesPath.string() + "/" + moduleHtmlName);

		return HtmlFile{ moduleHtmlPath, fs::path{ HtmlFolderStructure::FolderModules } / moduleHtmlName };
	}

	//---------------------------------------------------------------------
	HtmlFile HtmlFolderStructure::GetHtmlFilePath(const boost::filesystem::path& filePath) const
	{
		if (!currentModule_)
			throw std::runtime_error("No root module selected");
		
		auto filename = filePath.filename();
		auto output = filename.wstring() + L".html";
		auto fileHtmlPath = GetUniquePath((*currentModule_ / output).string());

		return HtmlFile{fileHtmlPath, currentModule_->filename() / output};
	}
}
