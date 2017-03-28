// CoverageTool.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <regex>

#include <boost/optional.hpp>

#include <CppCoverage/FileCoverage.hpp>
#include <CppCoverage/ModuleCoverage.hpp>
#include <CppCoverage/CoverageData.hpp>
#include <CppCoverage/CoverageRateComputer.hpp>
#include <Exporter/Html/HtmlExporter.hpp>

#pragma warning(disable:4996)

void
ParseToken(const std::string &token, char **&argv, std::string &inputFile, std::string &outputPath,
           std::regex& src_filter);

//    Notice the side effect on the string.
//    This is necessary unless we return the new string after the erase.
//    Doing this will introduce a struct I feel not necessary.
//    We delete the characters up to and including the delimiter.
unsigned int
GetToken(std::string& line)
{
    std::string delimiter = ",";
    size_t pos = line.find(delimiter);
    unsigned long token = stoul(line.substr(0, pos));
    line.erase(0, pos + delimiter.length());
    return (unsigned int) token;
}

void
skip_file(std::ifstream& ifstream)
{
    std::string line;
    for (std::getline(ifstream, line); strcmp(line.c_str(), "end_of_record") != 0;
         std::getline(ifstream, line)){}
}

std::string
GetMatchingFileName(std::ifstream& infile, std::regex srcFilter)
{
    std::string line;
    std::getline(infile, line);
    if (line.empty())
    {
        return line;
    }
    std::string fileName = line.substr(3, line.length() - 1);
    while (!std::regex_match(fileName, srcFilter))
    {
        skip_file(infile);
        if (!std::getline(infile, line))
        {
            std::cerr << "End of file";
            return std::string();
        }
        fileName = line.substr(3, line.length() - 1);
    }
    return fileName;
}

CppCoverage::CoverageData
parseFile(std::string path, std::string programName, std::regex srcFilter)
{
    CppCoverage::CoverageData data{L"Results", 0};

    std::ifstream infile(path);
    CppCoverage::ModuleCoverage &module = data.AddModule(programName);

    std::string fileName = GetMatchingFileName(infile, srcFilter);
    //std::cout << fileName << std::endl;
    CppCoverage::FileCoverage *file = &module.AddFile(fileName);

    std::string line;
    while (std::getline(infile, line))
    {
        if (strncmp(line.c_str(), "e", 1) == 0)
        {
            if ((fileName = GetMatchingFileName(infile, srcFilter)).empty()) {
                break;
            }
            //std::cout << fileName << std::endl;
            file = &module.AddFile(fileName);
        }
        else
        {
            std::string tokenLine = line.substr(3, line.length() - 1);
            unsigned int lineNumber = GetToken(tokenLine);
            bool hasBeenExecuted = GetToken(tokenLine) > 0;
            file->AddLine(lineNumber, hasBeenExecuted);
        }
    }
    return data;
}

void
CreateCoverageOutput(CppCoverage::CoverageData data, std::string outputPath)
{
    CppCoverage::CoverageRateComputer computer(data);

    CppCoverage::CoverageRate rate = computer.GetCoverageRate();
    //std::cout << rate.GetPercentRate() << "," << rate.GetTotalLinesCount() << std::endl;
    //for (const auto &module : data.GetModules())
    //{
        //PrintModule(*module, computer);
    //}

    Exporter::HtmlExporter exporter("../Exporter/Html/Template");
    exporter.Export(data, outputPath);
}

std::string
AssignDefaultPath()
{
    auto now = std::time(nullptr);
    auto localNow = std::localtime(&now);
    std::ostringstream ostr;

    ostr << "CoverageReport-" << std::put_time(localNow, "%Y-%m-%d-%Hh%Mm%Ss");

    return ostr.str();
}

int
main(int argc, char *argv[])
{
    std::string inputFile, outputPath, diffPath;
    std::regex src_filter;
    try {
        src_filter = std::regex("[\\w:\\\\]+.(cpp|c|h|hpp)");
    }
    catch (const std::regex_error& e) {
        std::cout << "error" << e.what() << std::endl;
    }

    for (++argv; *argv; ++argv)
    {
        std::string token = *argv;
        ParseToken(token, argv, inputFile, outputPath, src_filter);
    }

    if (inputFile.empty())
    {
        std::cout << "\"somebody\" forgot to declare input. Goodbye" << std::endl;
        return -1;
    }

    if (outputPath.empty())
    {
        outputPath = AssignDefaultPath();
    }

    if (!diffPath.empty())
    {
        // TODO: GenerateDiffReport
        // CreateDiffCoverageReport(/*arguments*/)...
    }
    else
    {
        CreateCoverageOutput(parseFile(inputFile, "Result", src_filter), outputPath);
    }

    return 0;
}

void
ParseToken(const std::string& token, char**& argv, std::string& inputFile, std::string& outputPath,
           std::regex& src_filter)
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
    else if (token.compare("--srcFilter") == 0)
    {
        ++argv;
        src_filter = std::regex(*argv);
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
