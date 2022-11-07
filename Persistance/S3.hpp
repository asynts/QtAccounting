#pragma once

#include <fstream>
#include <filesystem>

#include <fmt/format.h>

#include <QString>
#include <QSettings>

#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/core/auth/AWSCredentials.h>

#include "Persistance/Database.hpp"

// FIXME: Find a better name for this file.

namespace Accounting::Persistance
{
    constexpr const char *AWS_ALLOCATION_TAG = "AccountingTag";

    inline Aws::S3::S3Client get_s3_client() {
        static std::optional<Aws::S3::S3Client> s3_client;

        if (s3_client.has_value()) {
            return s3_client.value();
        }

        QSettings settings;

        Aws::Auth::AWSCredentials credentials;
        credentials.SetAWSAccessKeyId(settings.value("AWS/AccessKeyId").value<QString>().toStdString());
        credentials.SetAWSSecretKey(settings.value("AWS/SecretKey").value<QString>().toStdString());

        Aws::Client::ClientConfiguration clientConfiguration;
        clientConfiguration.region = settings.value("AWS/Region").value<QString>().toStdString();

        s3_client.emplace(credentials, clientConfiguration);
        return s3_client.value();
    }

    inline std::filesystem::path generate_local_path(std::filesystem::path relativePath, std::string filename) {
        std::filesystem::path path{ QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppDataLocation).toStdString() };
        path /= relativePath;
        path /= fmt::format("{:016x}_{}", QDateTime::currentMSecsSinceEpoch(), filename);

        return path;
    }

    inline std::optional<std::filesystem::path> fetch_file_from_s3(std::filesystem::path remotePath) {
        auto localPath = generate_local_path("/Database", "Database.bin");

        Aws::S3::Model::GetObjectRequest request;
        request.SetBucket("accounting-23fbf6ce-ff25-4f6a-a75a-b50bf814fc62");
        request.SetKey(remotePath);
        request.SetResponseStreamFactory([localPath] {
            return Aws::New<Aws::FStream>(AWS_ALLOCATION_TAG, localPath, std::ios_base::out | std::ios_base::binary);
        });

        // FIXME: Async.
        // FIXME: Error handling.
        // FIXME: If not exists return 'std::nullopt'.
        auto outcome = get_s3_client().GetObject(request);

        if (outcome.IsSuccess()) {
            return localPath;
        }

        if (outcome.GetError().GetErrorType() == Aws::S3::S3Errors::NO_SUCH_KEY) {
            return std::nullopt;
        }

        Q_UNREACHABLE();
    }

    inline void upload_file_to_s3(std::filesystem::path localPath, std::filesystem::path remotePath) {
        auto inputStream = Aws::MakeShared<Aws::FStream>(AWS_ALLOCATION_TAG, localPath, std::ios_base::in | std::ios_base::binary);

        Aws::S3::Model::PutObjectRequest request;
        request.SetBucket("accounting-23fbf6ce-ff25-4f6a-a75a-b50bf814fc62");
        request.SetKey(remotePath);
        request.SetBody(inputStream);

        // FIXME: Async.
        // FIXME: Error handling.
        auto outcome = get_s3_client().PutObject(request);
        Q_ASSERT(outcome.IsSuccess());
    }

    inline std::optional<Database> load() {
        auto path_opt = fetch_file_from_s3("/Database/Database.bin");

        if (!path_opt.has_value()) {
            return std::nullopt;
        }

        return read_from_disk(path_opt.value());
    }

    inline void save(const Database& database) {
        auto path = generate_local_path("/Database", "Database.bin");
        write_to_disk(database, path);
        upload_file_to_s3(path, fmt::format("/Database/{}", path.filename().string()));
        upload_file_to_s3(path, "/Database/Database.bin");
    }
}
