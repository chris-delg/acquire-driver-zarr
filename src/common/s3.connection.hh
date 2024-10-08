#pragma once

#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <span>
#include <vector>

#include <miniocpp/client.h>

namespace acquire::sink::zarr::common {
struct S3Connection final
{
    S3Connection() = delete;
    explicit S3Connection(const std::string& endpoint,
                          const std::string& access_key_id,
                          const std::string& secret_access_key);

    ~S3Connection() noexcept = default;

    /// @brief Test a connection by listing all buckets at this connection's
    ///        endpoint.
    /// @returns True if the connection is valid, otherwise false.
    bool check_connection();

    // bucket operations
    /// @brief Check whether a bucket exists.
    /// @param bucket_name The name of the bucket.
    /// @returns True if the bucket exists, otherwise false.
    /// @throws std::runtime_error if the bucket name is empty.
    bool bucket_exists(std::string_view bucket_name);

    // object operations
    /// @brief Check whether an object exists.
    /// @param bucket_name The name of the bucket containing the object.
    /// @param object_name The name of the object.
    /// @returns True if the object exists, otherwise false.
    /// @throws std::runtime_error if the bucket name is empty or the object
    ///         name is empty.
    bool object_exists(std::string_view bucket_name,
                       std::string_view object_name);

    /// @brief Put an object.
    /// @param bucket_name The name of the bucket to put the object in.
    /// @param object_name The name of the object.
    /// @param data The data to put in the object.
    /// @returns The etag of the object.
    /// @throws std::runtime_error if the bucket name is empty, the object name
    ///         is empty, or @p data is empty.
    [[nodiscard]] std::string put_object(std::string_view bucket_name,
                                         std::string_view object_name,
                                         std::span<uint8_t> data);

    /// @brief Delete an object.
    /// @param bucket_name The name of the bucket containing the object.
    /// @param object_name The name of the object.
    /// @returns True if the object was successfully deleted, otherwise false.
    /// @throws std::runtime_error if the bucket name is empty or the object
    ///         name is empty.
    [[nodiscard]] bool delete_object(std::string_view bucket_name,
                                     std::string_view object_name);

    // multipart object operations
    /// @brief Create a multipart object.
    /// @param bucket_name The name of the bucket containing the object.
    /// @param object_name The name of the object.
    /// @returns The upload id of the multipart object. Nonempty if and only if
    ///          the operation succeeds.
    /// @throws std::runtime_error if the bucket name is empty or the object
    ///         name is empty.
    [[nodiscard]] std::string create_multipart_object(
      std::string_view bucket_name,
      std::string_view object_name);

    /// @brief Upload a part of a multipart object.
    /// @param bucket_name The name of the bucket containing the object.
    /// @param object_name The name of the object.
    /// @param upload_id The upload id of the multipart object.
    /// @param data The data to upload.
    /// @param part_number The part number of the object.
    /// @returns The etag of the uploaded part. Nonempty if and only if the
    ///          operation is successful.
    /// @throws std::runtime_error if the bucket name is empty, the object name
    ///         is empty, @p data is empty, or @p part_number is 0.
    [[nodiscard]] std::string upload_multipart_object_part(
      std::string_view bucket_name,
      std::string_view object_name,
      std::string_view upload_id,
      std::span<uint8_t> data,
      unsigned int part_number);

    /// @brief Complete a multipart object.
    /// @param bucket_name The name of the bucket containing the object.
    /// @param object_name The name of the object.
    /// @param upload_id The upload id of the multipart object.
    /// @param parts List of the parts making up the object.
    /// @returns True if the object was successfully completed, otherwise false.
    [[nodiscard]] bool complete_multipart_object(
      std::string_view bucket_name,
      std::string_view object_name,
      std::string_view upload_id,
      const std::list<minio::s3::Part>& parts);

  private:
    std::unique_ptr<minio::s3::Client> client_;
    std::unique_ptr<minio::creds::StaticProvider> provider_;
};

struct S3ConnectionPool final
{
  public:
    S3ConnectionPool(size_t n_connections,
                     const std::string& endpoint,
                     const std::string& access_key_id,
                     const std::string& secret_access_key);
    ~S3ConnectionPool() noexcept;

    std::unique_ptr<S3Connection> get_connection();
    void return_connection(std::unique_ptr<S3Connection>&& conn);

  private:
    std::vector<std::unique_ptr<S3Connection>> connections_;
    mutable std::mutex connections_mutex_;
    std::condition_variable cv_;

    std::atomic<bool> is_accepting_connections_;
};
} // namespace acquire::sink::zarr::common
