#pragma once
#include <boost/noncopyable.hpp>
#include <Storages/ObjectStorageQueue/ObjectStorageQueueSettings.h>
#include <Storages/ObjectStorageQueue/ObjectStorageQueueMetadata.h>

namespace DB
{

class ObjectStorageQueueMetadataFactory final : private boost::noncopyable
{
public:
    using FilesMetadataPtr = std::shared_ptr<ObjectStorageQueueMetadata>;

    static ObjectStorageQueueMetadataFactory & instance();

    using MetadataCreatorFunc = std::function<std::shared_ptr<ObjectStorageQueueMetadata>()>;
    FilesMetadataPtr getOrCreate(const std::string & zookeeper_path, MetadataCreatorFunc creator_func);

    void remove(const std::string & zookeeper_path);

    std::unordered_map<std::string, FilesMetadataPtr> getAll();

private:
    struct Metadata
    {
        explicit Metadata(std::shared_ptr<ObjectStorageQueueMetadata> metadata_) : metadata(metadata_), ref_count(1) {}

        std::shared_ptr<ObjectStorageQueueMetadata> metadata;
        /// TODO: the ref count should be kept in keeper, because of the case with distributed processing.
        size_t ref_count = 0;
    };
    using MetadataByPath = std::unordered_map<std::string, Metadata>;

    MetadataByPath metadata_by_path;
    std::mutex mutex;
};

}
