cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_STANDARD 14)

find_package(Protobuf REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}
        ${PROTOBUF_INCLUDE_DIRS}
        /home/nicklas/Downloads/ctemplate-ctemplate-2.3/src)

PROTOBUF_GENERATE_CPP(PROTO_SRC PROTO_HEADER Binary/CoverageData.proto)

set_source_files_properties(${PROTO_SRC} ${PROTO_HEADER} PROPERTIES GENERATED TRUE)

add_library(Exporter
        ${PROTO_SRC}
        ${PROTO_HEADER}
        CoberturaExporter.cpp
        CoberturaExporter.hpp
        ExporterException.cpp
        ExporterException.hpp
        ExporterExport.hpp
        IExporter.hpp
        stdafx.cpp
        stdafx.h
        Binary/BinaryExporter.cpp
        Binary/BinaryExporter.hpp
        Binary/CoverageDataDeserializer.cpp
        Binary/CoverageDataDeserializer.hpp
        Binary/CoverageDataSerializer.cpp
        Binary/CoverageDataSerializer.hpp
        Binary/ProtoBuff.hpp
        Html/CTemplate.hpp
        Html/HtmlExporter.cpp
        Html/HtmlExporter.hpp
        Html/HtmlFile.cpp
        Html/HtmlFile.hpp
        Html/HtmlFileCoverageExporter.cpp
        Html/HtmlFileCoverageExporter.hpp
        Html/HtmlFolderStructure.cpp
        Html/HtmlFolderStructure.hpp
        Html/TemplateHtmlExporter.cpp
        Html/TemplateHtmlExporter.hpp
        )

link_directories(${CMAKE_SOURCE_DIR} ${PROTOBUF_LIBRARIES})