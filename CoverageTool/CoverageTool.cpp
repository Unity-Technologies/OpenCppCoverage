// CoverageTool.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <regex>

#include <boost/optional.hpp>

#include <CppCoverageCross/FileCoverage.hpp>
#include <CppCoverageCross/ModuleCoverage.hpp>
#include <CppCoverageCross/CoverageData.hpp>
#include <CppCoverageCross/CoverageRateComputer.hpp>
#include <Exporter/Html/HtmlExporter.hpp>
#include <FileFilterCross/UnifiedDiffParser.hpp>
#include <FileFilterCross/File.hpp>

#pragma warning(disable:4996)


void ParseToken(const std::string& token, char**& argv, std::string& inputFile, std::string& outputPath,
                std::regex& src_filter, std::string& diffPath);

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
    return static_cast<unsigned int>( token);
}

void
skip_file(std::ifstream& ifstream)
{
    std::string line;
    for (std::getline(ifstream, line); strcmp(line.c_str(), "end_of_record") != 0;
         std::getline(ifstream, line)){}
}

const FileFilter::File& GetFile(const std::vector<FileFilter::File>& files, const fs::path& path, const std::string defaultPath)
{
    std::string pathStr = path.string().substr(defaultPath.length(), path.string().length() - 1);
    auto it = std::find_if(files.begin(), files.end(),
                           [&](const FileFilter::File& file) { return file.GetPath().string() == pathStr; });
    if (it == files.end()) {
        throw std::runtime_error("Cannot find: " + path.string());
    }

    return *it;
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
            std::cerr << "End of file" << std::endl;
            return std::string();
        }
        fileName = line.substr(3, line.length() - 1);
    }
    return fileName;
}

std::string
GetMatchingAndDiffedFileName(std::ifstream& infile, const std::regex& srcFilter,
                             const std::vector<FileFilter::File>& files, std::string defaultPath) {
    std::string fileName;
    FileFilter::File result = FileFilter::File(boost::filesystem::path(""));
    do
    {
        fileName = GetMatchingFileName(infile, srcFilter);
        if (fileName.empty())
        {
            break;
        }
        const FileFilter::File &dimmer =
                std::find_if(files.begin(), files.end(), [&](const FileFilter::File &file) {
                    std::string tempPath = defaultPath;
                    // Side effect from the string append
                    return tempPath.append(file.GetPath().string()).compare(fileName) == 0;
                }).operator*();

        std::string tempPath = defaultPath;
        // Side effect from the string append
        result.SetPath(tempPath.append(dimmer.GetPath().string()));
    }
    while (fileName.compare(result.GetPath().string()) != 0);
    return fileName;
}

CppCoverage::CoverageData
parseFile(const std::string& path, std::string programName, const std::regex& srcFilter)
{
    CppCoverage::CoverageData data{L"Results", 0};

    std::ifstream infile(path);
    CppCoverage::ModuleCoverage &module = data.AddModule(programName);

    std::string fileName = GetMatchingFileName(infile, srcFilter);
    CppCoverage::FileCoverage *file = &module.AddFile(fileName);

    std::string line;
    while (std::getline(infile, line))
    {
        if (strncmp(line.c_str(), "e", 1) == 0)
        {
            if ((fileName = GetMatchingFileName(infile, srcFilter)).empty())
            {
                break;
            }
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

CppCoverage::CoverageData
parseFileWithDiff(const std::string& path, std::string programName, const std::regex& srcFilter,
                  const std::string& diffPath, const std::string defaultPath) {
    CppCoverage::CoverageData data{L"Results", 0};

    std::ifstream infile(path);
    CppCoverage::ModuleCoverage &module = data.AddModule(programName);

    FileFilter::UnifiedDiffParser unifiedDiffParser;
    std::wifstream diffFile{diffPath};
    diffFile.imbue(std::locale("en_US.UTF-8"));
    std::vector<FileFilter::File> files = unifiedDiffParser.Parse(diffFile);

    std::string fileName = GetMatchingAndDiffedFileName(infile, srcFilter, files, defaultPath);
    std::cout << "Found: " << fileName << std::endl;
    CppCoverage::FileCoverage *file = &module.AddFile(fileName);
    const FileFilter::File *filterFiler = &GetFile(files, fileName, defaultPath);

    std::string line;
    while (std::getline(infile, line))
    {
        if (strncmp(line.c_str(), "e", 1) == 0)
        {
            if ((fileName = GetMatchingAndDiffedFileName(infile, srcFilter, files, defaultPath)).empty())
            {
                break;
            }
            file = &module.AddFile(fileName);
            filterFiler = &GetFile(files, fileName, defaultPath);
        }
        else
        {
            std::string tokenLine = line.substr(3, line.length() - 1);
            unsigned int lineNumber = GetToken(tokenLine);
            bool hasBeenExecuted = GetToken(tokenLine) > 0;
            if (filterFiler->IsLineSelected(lineNumber))
            {
                file->AddLine(lineNumber, hasBeenExecuted);
            }
        }
    }
    return data;
}

void
CreateCoverageOutput(CppCoverage::CoverageData data, std::string outputPath)
{
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
main(int /*argc*/, char *argv[])
{
    std::string inputFile, outputPath, diffPath;
    std::regex src_filter;
    try {
        src_filter = std::regex(R"([\w:\/\\]+.(cpp|c|h|hpp))");
    }
    catch (const std::regex_error& e) {
        std::cout << "error" << e.what() << std::endl;
    }

    for (++argv; *argv != nullptr; ++argv)
    {
        ParseToken(*argv, argv, inputFile, outputPath, src_filter, diffPath);
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
        CreateCoverageOutput(parseFileWithDiff(inputFile, "Result", src_filter, diffPath, "/home/nicklas/unity/"), outputPath);
    }
    else
    {
        CreateCoverageOutput(parseFile(inputFile, "Result", src_filter), outputPath);
    }

    return 0;
}

void
ParseToken(const std::string& token, char**& argv, std::string& inputFile, std::string& outputPath,
           std::regex& src_filter, std::string& diffPath)
{
    if (token.empty())
    {
        std::cout << "TODO Handle empty token, even though I doubt it." << std::endl;
    }
    else if (token == "--input")
    {
        ++argv;
        inputFile = *argv;
    }
    else if (token == "--srcFilter")
    {
        ++argv;
        src_filter = std::regex(*argv);
    }
    else if (token == "--output")
    {
        ++argv;
        outputPath = *argv;
    }
    else if (token == "--diff")
    {
        ++argv;
        diffPath = *argv;
        std::cout << "Parsing diff coverage, not really though." << std::endl;
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
