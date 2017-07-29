

#include <iostream>
#include "ZipArchiveWriter.h"

#if 0
#include <fstream>
#include <string>
#include <zip.h>
#include <filesystem>


std::string getData(const std::string  &filePath) {

  std::ifstream ifs(filePath.c_str());
  if (ifs) {
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    return content;
  }
  return std::string();
}



void writeDataToArchive(const std::string  &zipFileName, const std::string  &sourceFile, const std::string  &entryName) {
  int errorFlag = 0;
  zip_t  *zipHandle = zip_open(zipFileName.c_str(), ZIP_CREATE | ZIP_CM_REDUCE_4, &errorFlag);
  if (errorFlag != ZIP_ER_OK) {
    return;
  }

  std::ifstream ifs(sourceFile.c_str(), std::ifstream::in | std::ifstream::binary);
  ifs.seekg(0, std::ifstream::end);
  std::streampos end = ifs.tellg();
  
  zip_source_t  *src = zip_source_file(zipHandle, sourceFile.c_str(), 0, end);
  if (src != nullptr) {
    zip_int64_t result = zip_file_add(zipHandle, entryName.c_str(), src, ZIP_FL_OVERWRITE);
  }
  zip_close(zipHandle);
}
#endif

int main(int argc, char *argv[]) {
  std::string filePath("c:\\Users\\Veleos\\Documents\\temp\\test_folder");
  std::string zipPath("c:\\Users\\Veleos\\Documents\\temp\\test_folder.zip");

  zip_archive_writer::zip_directory(filePath, zipPath);

  return 0;
}