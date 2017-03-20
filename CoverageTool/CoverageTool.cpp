// CoverageTool.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include <CppCoverageCross/FileCoverage.hpp>
#include <CppCoverageCross/ModuleCoverage.hpp>
#include <CppCoverageCross/CoverageData.hpp>
#include <ExporterCross/Html/HtmlExporter.hpp>
#include <ToolsCross/Tool.hpp>

#pragma warning(disable:4996)

void
ParseToken(const std::string &token, char **&argv, std::string &inputFile, std::string &outputPath);

CppCoverage::CoverageData parseFile(std::string path, std::string programName)
{
    CppCoverage::CoverageData data{"Results", 0};

    std::ifstream infile(path);
    CppCoverage::ModuleCoverage &module = data.AddModule(programName);
    std::string line;
    std::getline(infile, line);
    CppCoverage::FileCoverage *file = &module.AddFile(line.substr(3, line.length() - 1));
    while (std::getline(infile, line))
    {
        if (!file->AddLine(line))
        {
            if (!std::getline(infile, line))
            {
                break;
            }
            file = &module.AddFile(line.substr(3, line.length() - 1));
        }
    }
    return data;
}

void
PrintFile(const CppCoverage::FileCoverage &file,
          const CppCoverage::CoverageRateComputer &computer)
{
    if (file.GetLines().empty())
    {
        std::cout << "empty file" << std::endl;
    }
    CppCoverage::CoverageRate rate = computer.GetCoverageRate(file);
    std::cout << file.GetPath() << " . " << rate.GetExecutedLinesCount() << std::endl;
    std::vector<CppCoverage::LineCoverage> lines = file.GetLines();
    for (unsigned int i = 0; i <= lines.size(); i++)
    {
        try
        {
            // at() instead of [] operator as we don't want to create garbage entries
            CppCoverage::LineCoverage line = lines.at(i);
            std::cout << line.GetLineNumber() << " : " << line.HasBeenExecuted() << std::endl;
        }
        catch (std::out_of_range oof)
        {
            // Skip empty entries and move on.
        }
    }
}

void
PrintModule(const CppCoverage::ModuleCoverage &module,
            const CppCoverage::CoverageRateComputer &computer)
{
    CppCoverage::CoverageRate rate = computer.GetCoverageRate(module);
    std::cout << module.GetPath() << " . " << rate.GetPercentRate() << std::endl;
    for (const auto &file : module.GetFiles())
    {
        PrintFile(*file, computer);
    }
}

void
CreateCoverageOutput(CppCoverage::CoverageData data, std::string outputPath)
{
    CppCoverage::CoverageRateComputer computer(data);

    CppCoverage::CoverageRate rate = computer.GetCoverageRate();
    std::cout << rate.GetPercentRate() << "," << rate.GetTotalLinesCount() << std::endl;
    for (const auto &module : data.GetModules())
    {
        PrintModule(*module, computer);
    }

    Exporter::HtmlExporter exporter("../Exporter/Html/Template");
    exporter.Export(data, outputPath);
}

std::string AssignDefaultPath()
{
    auto now = std::time(nullptr);
    auto localNow = std::localtime(&now);
    std::ostringstream ostr;

    ostr << "CoverageReport-" << std::put_time(localNow, "%Y-%m-%d-%Hh%Mm%Ss");

    return ostr.str();
}

int main(int argc, char *argv[])
{
    std::string inputFile, outputPath, diffPath;
    for (++argv; *argv; ++argv)
    {
        std::string token = *argv;
        ParseToken(token, argv, inputFile, outputPath);
    }

    if (inputFile.empty())
    {
        std::cout << "\"somebody\" forgot to declare input. Goodbye" << std::endl;
        return -1;
    }

    if (outputPath.empty())
    {
        outputPath = "./" + AssignDefaultPath();
    }

    if (!diffPath.empty())
    {
        // TODO: GenerateDiffReport
        // CreateDiffCoverageReport(/*arguments*/)...
    }
    else
    {
        CreateCoverageOutput(parseFile(inputFile, "Result"), outputPath);
    }

    return 0;
}

void
ParseToken(const std::string &token, char **&argv, std::string &inputFile, std::string &outputPath)
{
    if (token.empty())
    {
        std::cout << "TODO Handle empty token, even though I doubt it." << std::endl;
    }
    else if (token.compare("--input") == 0)
    {
        ++argv;
        inputFile = *argv;
    }
    else if (token.compare("--output") == 0)
    {
        ++argv;
        outputPath = *argv;
    }
    else if (token.compare("--diff") == 0)
    {
        std::cout << "Parsing diff coverage, not really thought." << std::endl;
    }
    else
    {
        std::cout << "Undefined command " << token << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "\t--input <path_to_lcov>" << std::endl;
        std::cout << "\t--output <path_to_output>" << std::endl;
        std::cout << "\t--diff <path_to_patch>" << std::endl;
    }
}
