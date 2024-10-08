#pragma once

#include "sink.hh"
#include "common/utilities.hh"
#include "common/dimension.hh"
#include "common/thread.pool.hh"
#include "common/s3.connection.hh"

#include <optional>
#include <memory>
#include <unordered_map>

namespace acquire::sink::zarr {
class SinkCreator final
{
  public:
    SinkCreator() = delete;
    SinkCreator(std::shared_ptr<common::ThreadPool> thread_pool_,
                std::shared_ptr<common::S3ConnectionPool> connection_pool);
    ~SinkCreator() noexcept = default;

    /// @brief Create a sink from a URI and a path.
    /// @param[in] base_uri The base URI for the sink.
    /// @param[in] path The path for the sink.
    /// @return The sink created, or nullptr if the URI is invalid.
    std::unique_ptr<Sink> make_sink(std::string_view base_uri,
                                    std::string_view path);

    /// @brief Create a collection of data sinks, either chunk or shard.
    /// @param[in] base_uri The base URI for the sinks.
    /// @param[in] dimensions The dimensions of the data.
    /// @param[in] parts_along_dimension Function for computing the number of
    ///            parts (either chunk or shard) along each dimension.
    /// @param[out] part_sinks The sinks created.
    /// @return True iff all data sinks were created successfully.
    /// @throws std::runtime_error if @p base_uri is not valid, if the number of
    ///         parts along a dimension cannot be computed, or if, for S3 sinks,
    ///         the bucket does not exist.
    [[nodiscard]] bool make_data_sinks(
      const std::string& base_uri,
      const std::vector<Dimension>& dimensions,
      const std::function<size_t(const Dimension&)>& parts_along_dimension,
      std::vector<std::unique_ptr<Sink>>& part_sinks);

    /// @brief Create a collection of metadata sinks for a Zarr dataset.
    /// @param[in] version The Zarr version.
    /// @param[in] base_uri The base URI for the dataset.
    /// @param[out] metadata_sinks The sinks created, keyed by path.
    /// @return True iff all metadata sinks were created successfully.
    /// @throws std::runtime_error if @p base_uri is not valid, or if, for S3
    ///         sinks, the bucket does not exist.
    [[nodiscard]] bool make_metadata_sinks(
      ZarrVersion version,
      const std::string& base_uri,
      std::unordered_map<std::string, std::unique_ptr<Sink>>& metadata_sinks);

  private:
    std::shared_ptr<common::ThreadPool> thread_pool_;
    std::shared_ptr<common::S3ConnectionPool> connection_pool_; // could be null

    /// @brief Parallel create a collection of directories.
    /// @param[in] dir_paths The directories to create.
    /// @return True iff all directories were created successfully.
    [[nodiscard]] bool make_dirs_(std::queue<std::string>& dir_paths);

    /// @brief Parallel create a collection of files.
    /// @param[in,out] file_paths The files to create. Unlike `make_dirs_`,
    /// this function drains the queue.
    /// @param[out] files The files created.
    /// @return True iff all files were created successfully.
    [[nodiscard]] bool make_files_(std::queue<std::string>& file_paths,
                                   std::vector<std::unique_ptr<Sink>>& sinks);

    /// @brief Parallel create a collection of files, keyed by path.
    /// @param[in] base_dir The base directory for the files.
    /// @param[in] file_paths Paths to the files to create, relative to @p base_dir.
    /// @param[out] sinks The sinks created, keyed by path.
    /// @return True iff all files were created successfully.
    [[nodiscard]] bool make_files_(
      const std::string& base_dir,
      const std::vector<std::string>& file_paths,
      std::unordered_map<std::string, std::unique_ptr<Sink>>& sinks);

    /// @brief Check whether an S3 bucket exists.
    /// @param[in] bucket_name The name of the bucket to check.
    /// @return True iff the bucket exists.
    bool bucket_exists_(std::string_view bucket_name);

    /// @brief Create a collection of S3 objects.
    /// @param[in] bucket_name The name of the bucket.
    /// @param[in,out] object_keys The keys of the objects to create.
    /// @param[out] sinks The sinks created.
    /// @return True iff all S3 objects were created successfully.
    [[nodiscard]] bool make_s3_objects_(
      std::string_view bucket_name,
      std::queue<std::string>& object_keys,
      std::vector<std::unique_ptr<Sink>>& sinks);

    /// @brief Create a collection of S3 objects, keyed by object key.
    /// @param[in] bucket_name The name of the bucket.
    /// @param[in] object_keys The keys of the objects to create.
    /// @param[out] sinks The sinks created, keyed by object key.
    /// @return True iff all S3 objects were created successfully.
    [[nodiscard]] bool make_s3_objects_(
      std::string_view bucket_name,
      std::vector<std::string>& object_keys,
      std::unordered_map<std::string, std::unique_ptr<Sink>>& sinks);
};
} // namespace acquire::sink::zarr
