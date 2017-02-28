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
#include "HtmlExporter.hpp"

#include "CTemplate.hpp"

#include <iomanip>
#include <sstream>

#include <CppCoverageCross/CoverageData.hpp>
#include <CppCoverageCross/ModuleCoverage.hpp>
#include <CppCoverageCross/FileCoverage.hpp>

#include "Tools/Log.hpp"
#include <ToolsCross/Tool.hpp>

#include "HtmlFolderStructure.hpp"

#pragma warning(disable:4996)

namespace cov = CppCoverage;

namespace Exporter
{

    namespace
    {
        //---------------------------------------------------------------------
        void ShowOutputMessage(const fs::path &outputFolder)
        {
            auto separators = L"----------------------------------------------------";
            LOG_INFO << separators;
            LOG_INFO << L"Coverage generated in Folder " << outputFolder.wstring();
            LOG_INFO << separators;
        }

        //-------------------------------------------------------------------------
        std::wstring GetMainMessage(const CppCoverage::CoverageData &coverageData)
        {
            auto exitCode = coverageData.GetExitCode();

            if (exitCode)
                return HtmlExporter::WarningExitCodeMessage + std::to_wstring(exitCode);
            return L"";
        }

        std::wstring filename(const std::wstring& file_path)
        {
            std::wstring file_name;

            std::wstring::const_reverse_iterator it = std::find(file_path.rbegin(), file_path.rend(), '\\');
            if (it != file_path.rend())
            {
                file_name.assign(file_path.rbegin(), it);
                std::reverse(file_name.begin(), file_name.end());
            }
            return file_name;
        }
    }

    //-------------------------------------------------------------------------
    const std::wstring HtmlExporter::WarningExitCodeMessage = L"Warning: Your program has exited with error code: ";

    //-------------------------------------------------------------------------
    HtmlExporter::HtmlExporter(const std::wstring &templateFolder)
            : exporter_(templateFolder), fileCoverageExporter_(), templateFolder_(templateFolder)
    {
    }

    //-------------------------------------------------------------------------
    boost::filesystem::path HtmlExporter::GetDefaultPath(const std::wstring&) const
    {
        auto now = std::time(nullptr);
        auto localNow = std::localtime(&now);
        std::ostringstream ostr;

        ostr << "CoverageReport-" << std::put_time(localNow, "%Y-%m-%d-%Hh%Mm%Ss");

        return ostr.str();
    }

    //-------------------------------------------------------------------------
    void HtmlExporter::Export(
            const CppCoverage::CoverageData &coverageData,
            const std::wstring &outputFolderPrefix) const
    {
        HtmlFolderStructure htmlFolderStructure{templateFolder_};
        CppCoverage::CoverageRateComputer coverageRateComputer{coverageData};

        auto mainMessage = GetMainMessage(coverageData);

        auto projectDictionary = exporter_.CreateTemplateDictionary(coverageData.GetName(),
                                                                    mainMessage);
        auto outputFolder = htmlFolderStructure.CreateCurrentRoot(outputFolderPrefix);

        for (const auto &module : coverageRateComputer.SortModulesByCoverageRate())
        {
            const auto &moduleCoverageRate = coverageRateComputer.GetCoverageRate(*module);

            if (moduleCoverageRate.GetTotalLinesCount())
            {
                const auto &modulePath = module->GetPath();
                std::wstring moduleFilename = filename(module->GetPath());
                auto moduleTemplateDictionary = exporter_.CreateTemplateDictionary(
                        moduleFilename, L"");

                auto htmlModulePath = htmlFolderStructure.CreateCurrentModule(modulePath);
                ExportFiles(coverageRateComputer, *module, htmlFolderStructure,
                            *moduleTemplateDictionary);

                exporter_.GenerateModuleTemplate(*moduleTemplateDictionary,
                                                 htmlModulePath.GetAbsolutePath());
                exporter_.AddModuleSectionToDictionary(Tools::ToUtf8String(module->GetPath()),
                                                       moduleCoverageRate,
                                                       htmlModulePath.GetRelativeLinkPath(),
                                                       *projectDictionary);
            }
        }

        exporter_.GenerateProjectTemplate(*projectDictionary, outputFolder / L"index.html");
        ShowOutputMessage(outputFolder);
    }

    //---------------------------------------------------------------------
    void HtmlExporter::ExportFiles(
            cov::CoverageRateComputer &coverageRateComputer,
            const cov::ModuleCoverage &module,
            const HtmlFolderStructure &htmlFolderStructure,
            ctemplate::TemplateDictionary &moduleTemplateDictionary) const
    {
        for (const auto &file : module.GetFiles())
        {
            const auto &fileCoverageRate = coverageRateComputer.GetCoverageRate(*file);
            boost::optional<fs::path> generatedOutput = ExportFile(htmlFolderStructure, *file);
            exporter_.AddFileSectionToDictionary(Tools::ToUtf8String(file->GetPath()), fileCoverageRate,
                                                 generatedOutput.get_ptr(),
                                                 moduleTemplateDictionary);
        }
    }

    //---------------------------------------------------------------------
    boost::optional<fs::path> HtmlExporter::ExportFile(
            const HtmlFolderStructure &htmlFolderStructure,
            const cov::FileCoverage &fileCoverage) const
    {
        auto htmlFilePath = htmlFolderStructure.GetHtmlFilePath(fileCoverage.GetPath());
        std::wostringstream ostr;

        if (!fileCoverageExporter_.Export(fileCoverage, ostr))
            return boost::optional<fs::path>();

        auto title = filename(fileCoverage.GetPath());
        exporter_.GenerateSourceTemplate(
                title, ostr.str(), htmlFilePath.GetAbsolutePath());

        return htmlFilePath.GetRelativeLinkPath();
    }
}
