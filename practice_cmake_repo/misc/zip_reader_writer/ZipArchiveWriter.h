#ifndef ZIP_ARCHIVE_WRITTER_H
#define ZIP_ARCHIVE_WRITTER_H

#include <experimental/filesystem>
#include <string>
#include <memory>


#include <zip.h>

#ifndef BOOST_FILE_SYSTEM
namespace filesystem = std::experimental::filesystem;
#else
#include <boost/filesystem.hpp>
namespace filesystem = boost::filesystem;
#endif

class zip_archive_writer {
public:
  zip_archive_writer();
  ~zip_archive_writer();
  zip_archive_writer(const filesystem::path  &path);
  void close();


  zip_archive_writer(const zip_archive_writer  &) = delete;
  zip_archive_writer & operator = (const zip_archive_writer  &) = delete;

  //filename is used as relative path for entry name
  void add_filesystem_entry(const filesystem::path  &file, const filesystem::path  &entry);

  //instantinate for complex types
  template<typename SimpleType>
  void add_data(const SimpleType  &typeValue, const std::string  &entry);

  const filesystem::path& get_path() const;

  static bool zip_directory(const filesystem::path  &root, const filesystem::path  &dest);
private:
#ifdef _WINNT
  zip_t  *zip_open_unicode(const std::wstring  &file_name, int flags) const;
#endif
  filesystem::path  path_;
  zip_t  *handle_;
};

#endif //ZIP_ARCHIVE_WRITTER_H
