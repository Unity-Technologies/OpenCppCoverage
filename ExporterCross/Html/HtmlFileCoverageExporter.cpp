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
#include "HtmlFileCoverageExporter.hpp"

#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/include/classic_tree_to_xml.hpp>

#include <CppCoverageCross/FileCoverage.hpp>
#include <ToolsCross\Tool.hpp>

namespace fs = boost::filesystem;

namespace Exporter
{
	namespace
	{
		//---------------------------------------------------------------------
		std::string UpdateLineColor(const std::string& line, bool codeHasBeenExecuted)
		{
			std::string output;

			if (codeHasBeenExecuted)
				output += HtmlFileCoverageExporter::StyleBackgroundColorExecuted;
			else
				output += HtmlFileCoverageExporter::StyleBackgroundColorUnexecuted;

			output += line;
			output += "</span>";

			return output;
		}

		const std::string StyleBackgroundColor = "<span style = \"background-color:#";
	}

	const std::string HtmlFileCoverageExporter::StyleBackgroundColorExecuted = StyleBackgroundColor + "dfd" + "\">";
	const std::string HtmlFileCoverageExporter::StyleBackgroundColorUnexecuted = StyleBackgroundColor + "fdd" + "\">";
	
	//-------------------------------------------------------------------------
	bool HtmlFileCoverageExporter::Export(const CppCoverage::FileCoverage& fileCoverage,
                                          std::ostream& output) const
	{
		auto filePath = fileCoverage.GetPath();

		if (!fs::exists(filePath))
			return false;

		std::ifstream ifs(filePath.c_str(), std::ifstream::in);
		if (!ifs)
			throw std::runtime_error("Cannot open file : " + filePath);

		std::string line;
		for (int i = 1; std::getline(ifs, line); ++i)
		{
			auto lineCoverage = fileCoverage[i];
			
			line = boost::spirit::classic::xml::encode(line);
				
			if (lineCoverage)
				line = UpdateLineColor(line, lineCoverage->HasBeenExecuted());
				
			output << line << std::endl;
		}
		output.flush();

		return true;
	}
}
