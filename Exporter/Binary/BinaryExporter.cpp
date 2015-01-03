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
#include "BinaryExporter.hpp"

#include <fstream>

#include <boost/filesystem.hpp>

#include "CoverageDataSerializer.hpp"

#include "Tools/Tool.hpp"

namespace Exporter
{
	//-------------------------------------------------------------------------
	boost::filesystem::path BinaryExporter::GetDefaultPath(const std::wstring& runningCommandFilename) const
	{
		boost::filesystem::path path{ runningCommandFilename };

		path += "Binary.data";

		return path;
	}

	//-------------------------------------------------------------------------
	void BinaryExporter::Export(
		const CppCoverage::CoverageData& coverageData, 
		const boost::filesystem::path& output) const
	{
		CoverageDataSerializer coverageDataSerializer;

		coverageDataSerializer.Serialize(coverageData, output);
		Tools::ShowOutputMessage(L"Coverage binary generated in file: ", output);
	}
}
