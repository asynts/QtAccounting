#pragma once

#include <fstream>
#include <filesystem>
#include <optional>
#include <coroutine>

#include <fmt/format.h>

#include <QString>
#include <QSettings>

#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/core/auth/AWSCredentials.h>
#include <aws/core/client/DefaultRetryStrategy.h>

#include "Persistance/Database.hpp"
#include "Util.hpp"

namespace Accounting::Persistance
{
    inline Aws::S3::S3Client get_s3_client() {
        QSettings settings;

        Aws::Auth::AWSCredentials credentials;
        credentials.SetAWSAccessKeyId(settings.value("AWS/AccessKeyId").value<QString>().toStdString());
        credentials.SetAWSSecretKey(settings.value("AWS/SecretKey").value<QString>().toStdString());

        Aws::Client::ClientConfiguration clientConfiguration;
        clientConfiguration.region = settings.value("AWS/Region").value<QString>().toStdString();
        clientConfiguration.connectTimeoutMs = 1000;
        clientConfiguration.httpRequestTimeoutMs = 1000;
        clientConfiguration.requestTimeoutMs = 1000;
        clientConfiguration.retryStrategy = Aws::MakeShared<Aws::Client::DefaultRetryStrategy>(0);

        auto endpointProvider = Aws::MakeShared<Aws::S3::Endpoint::S3EndpointProvider>(ACCOUNTING_ALLOCATION_TAG);

        return Aws::S3::S3Client{ credentials, endpointProvider, clientConfiguration };
    }

    inline std::filesystem::path generate_local_path(
            std::filesystem::path relativePath,
            std::string_view filename,
            std::string_view extension,
            bool appendVersion)
    {
        std::filesystem::path path{ QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppDataLocation).toStdString() };
        path /= relativePath;

        if (appendVersion) {
            path /= fmt::format("{:016x}_{}_version{}{}", QDateTime::currentMSecsSinceEpoch(), filename, binary_version, extension);
        } else {
            path /= fmt::format("{:016x}_{}{}", QDateTime::currentMSecsSinceEpoch(), filename, extension);
        }

        return path;
    }

    inline std::future<std::optional<std::filesystem::path>> fetch_file_from_s3_async(std::filesystem::path remotePath) {
        auto localPath = generate_local_path("Database", "Database", ".bin", true);

        QSettings settings;

        Aws::S3::Model::GetObjectRequest request;
        request.SetBucket(settings.value("AWS/BucketName").value<QString>().toStdString());
        request.SetKey(remotePath);
        request.SetResponseStreamFactory([localPath] {
            return Aws::New<Aws::FStream>(ACCOUNTING_ALLOCATION_TAG, localPath, std::ios_base::out | std::ios_base::binary);
        });

        return std::async(std::launch::async, [request, localPath] () -> std::optional<std::filesystem::path> {
            auto outcome = get_s3_client().GetObject(request);

            if (outcome.IsSuccess()) {
                return localPath;
            } else {
                return std::nullopt;
            }
        });
    }

    inline std::future<bool> upload_file_to_s3_async(std::filesystem::path localPath, std::filesystem::path remotePath) {
        auto inputStream = Aws::MakeShared<Aws::FStream>(ACCOUNTING_ALLOCATION_TAG, localPath, std::ios_base::in | std::ios_base::binary);

        QSettings settings;

        Aws::S3::Model::PutObjectRequest request;
        request.SetBucket(settings.value("AWS/BucketName").value<QString>().toStdString());
        request.SetKey(remotePath);
        request.SetBody(inputStream);

        return std::async(std::launch::async, [request] {
            auto outcome = get_s3_client().PutObject(request);
            return outcome.IsSuccess();
        });
    }

    inline std::future<std::optional<Database>> load_async() {
        return std::async(std::launch::async, [] () -> std::optional<Database> {
            auto path_opt = fetch_file_from_s3_async("/Database/Database.bin").get();

            if (!path_opt.has_value()) {
                return std::nullopt;
            }

            return read_from_disk(path_opt.value());
        });
    }

    inline std::future<bool> save_async(const Database& database) {
        auto path = generate_local_path("Database", "Database", ".bin", true);
        write_to_disk(database, path);

        return std::async(std::launch::async, [path] {
            if (!upload_file_to_s3_async(path, fmt::format("/Database/{}", path.filename().string())).get()) {
                return false;
            }

            if (!upload_file_to_s3_async(path, "/Database/Database.bin").get()) {
                return false;
            }

            return true;
        });
    }

    using MigrationFunction = void(*)(std::filesystem::path fromPath, std::filesystem::path toPath);

    inline std::future<bool> migrate_async(MigrationFunction migrationFunction) {
        return std::async(std::launch::async, [=] {
            auto fromPath_opt = fetch_file_from_s3_async("/Database/Database.bin").get();
            if (!fromPath_opt.has_value()) {
                return false;
            }

            auto toPath = generate_local_path("Database", "Database", ".bin", true);

            migrationFunction(fromPath_opt.value(), toPath);

            if (!upload_file_to_s3_async(toPath, fmt::format("/Database/{}", toPath.filename().string())).get()) {
                return false;
            }

            if (!upload_file_to_s3_async(toPath, "/Database/Database.bin").get()) {
                return false;
            }

            return true;
        });
    }
}
