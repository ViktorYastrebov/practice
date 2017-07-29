#include <stdexcept>

#include "ZipArchiveWriter.h"

zip_archive_writer::zip_archive_writer()
  : path_(), handle_(nullptr)
{}

zip_archive_writer::~zip_archive_writer()
{
  close();
}

zip_archive_writer::zip_archive_writer(const filesystem::path  &path)
  : path_(path)
{
  int errorFlag = ZIP_ER_OK;
  filesystem::path absolute = filesystem::canonical(path_);
#if defined(_WINNT) && defined(_UNICODE)
  //throws exception
  handle_ = zip_open_unicode(absolute.wstring().c_str(), ZIP_CREATE);
#else
  handle_ = zip_open(absolute.string().c_str(), ZIP_CREATE, &errorFlag);
#endif
  if (errorFlag != ZIP_ER_OK) {
    throw std::runtime_error("zip_open has failed");
  }
}

void zip_archive_writer::close() {
  zip_close(handle_);
  handle_ = nullptr;
  path_.clear();
}

void zip_archive_writer::add_filesystem_entry(const filesystem::path  &file, const filesystem::path  &entry) {

  if (filesystem::is_directory(file)) {
    zip_int64_t result = zip_add_dir(handle_, entry.string().c_str());
    if (result < 0) {
      close();
      throw std::runtime_error("zip_add_dir failed with entry < 0");
    }
    return;
  }

  uintmax_t size = size = filesystem::file_size(file);
  zip_source_t  *src = zip_source_file(handle_, file.string().c_str(), 0, size);
  if (src != nullptr) {
    zip_int64_t result = zip_file_add(handle_, entry.string().c_str(), src, ZIP_FL_OVERWRITE);
    if (result < 0) {
      close();
      throw std::runtime_error("zip_file_add failed with entry < 0");
    }
  }
}

template<typename SimpleType>
void zip_archive_writer::add_data(const SimpleType  &typeValue, const std::string  &entry) {

  zip_source* source = zip_source_buffer(handle_, reinterpret_cast<const void*>(typeValue), sizeof(SimpleType), 0);
  if (source != nullptr) {
    zip_int64_t result = zip_file_add(handle_, entry.c_str(), source, ZIP_FL_OVERWRITE);
    if (result < 0) {
      close();
      throw std::runtime_error("zip_file_add failed with entry -1");
    }
  }
}

const filesystem::path& zip_archive_writer::get_path() const {
  return path_;
}

bool zip_archive_writer::zip_directory(const filesystem::path  &root, const filesystem::path  &dest) {
  //have to make relative path to the root.
  try {
    zip_archive_writer zip_writter(dest);
    std::size_t rootLen = root.string().length();
    for (auto &fs_entry : filesystem::recursive_directory_iterator(root)) {
      filesystem::path entry = fs_entry.path().string().substr(rootLen);
      zip_writter.add_filesystem_entry(fs_entry, entry);
    }
  } catch (std::exception  &ex) {
    return false;
  }
  return true;
}

#ifdef _WINNT
zip_t  *zip_archive_writer::zip_open_unicode(const std::wstring  &file_name, int flags) const {

  zip_error_t zip_error;
  zip_error_init(&zip_error);

  zip_source_t *src = zip_source_win32w_create(file_name.c_str(), 0, -1, &zip_error);
  if (src == nullptr) {
    std::string  error(zip_error_strerror(&zip_error));
    zip_error_fini(&zip_error);
    throw std::runtime_error(error.c_str());
  }

  zip_t *za = zip_open_from_source(src, flags, &zip_error);
  if (za == nullptr) {
    std::string error(zip_error_strerror(&zip_error));
    zip_source_free(src);
    zip_error_fini(&zip_error);
    throw std::runtime_error(error.c_str());
  }
  zip_error_fini(&zip_error);

  return za;
}
#endif
