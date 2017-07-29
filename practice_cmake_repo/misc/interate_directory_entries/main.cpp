#include <iostream>

#include <iostream>
#include <string>
#include <sstream>

//#include <filesystem>
#include <experimental/filesystem>

#include <fstream>

template<class StringType, class UnaryOperation>
void IterateRecursiveFiles(const StringType  &rootDir, UnaryOperation UnOp) {
  for (auto& entry : std::experimental::filesystem::recursive_directory_iterator(rootDir)) {
    //list.push_back(entry.path().string());
    UnOp(entry);
  }
}

/*
template<class StringType>
std::stringstream getZippedFiles(const StringType  &rootDir) {
  std::stringstream original;
  for (auto & entry : std::experimental::filesystem::recursive_directory_iterator(rootDir)) {
    std::ifstream currentFile(entry.path().string().c_str());
    original << currentFile.rdbuf();
  }

  boost::iostreams::filtering_streambuf< boost::iostreams::input > out;
  std::stringstream compressed;
  out.push(boost::iostreams::zlib_compressor());
  out.push(original);
  boost::iostreams::copy(out, compressed);

  return compressed;
}
*/

void printFilePath(const std::experimental::filesystem::path  &path) {
  std::cout << path << std::endl;
}


int main(int argc, char*argv[]) {
  
  std::string root("./");
  IterateRecursiveFiles(root, printFilePath);
  
  return 0;
}
