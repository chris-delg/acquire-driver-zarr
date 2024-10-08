#include "file.sink.hh"
#include "platform.h"
#include "common/utilities.hh"

#include <latch>

namespace zarr = acquire::sink::zarr;

zarr::FileSink::FileSink(const std::string& uri)
  : file_(new struct file, &file_close)
{
    CHECK(file_);
    CHECK(file_create(file_.get(), uri.c_str(), uri.size() + 1));
}

bool
zarr::FileSink::write(size_t offset, const uint8_t* buf, size_t bytes_of_buf)
{
    CHECK(buf);
    CHECK(bytes_of_buf);

    return file_write(file_.get(), offset, buf, buf + bytes_of_buf);
}
