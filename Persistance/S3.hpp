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
    constexpr const char *ALLOCATION_TAG = "AccountingTag";

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

    /*
    inline void upload_file(QString localPath, QString remotePath) {
        std::string remotePath_std = remotePath.toStdString();
        std::string localPath_std = localPath.toStdString();

        auto client = s3_client();

        Aws::S3::Model::PutObjectRequest request;
        request.SetBucket("accounting-23fbf6ce-ff25-4f6a-a75a-b50bf814fc62");
        request.SetKey(remotePath_std);

        std::shared_ptr<Aws::IOStream> inputData = Aws::MakeShared<Aws::FStream>("S3", localPath_std, std::ios_base::in | std::ios_base::binary);
        Q_ASSERT(*inputData);

        request.SetBody(inputData);

        // FIXME: Do this asynchronous
        // FIXME: Deal with errors
        auto outcome = client.PutObject(request);
        Q_ASSERT(outcome.IsSuccess());
    }
    */

    inline void update_symbolic_link(std::filesystem::path path) {
        // FIXME
    }

    inline std::filesystem::path fetch_latest_from_s3() {
        std::filesystem::path localPath{ QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppDataLocation).toStdString() };
        localPath /= "Database";
        localPath /= fmt::format("{:016x}_Database.bin", QDateTime::currentMSecsSinceEpoch());

        Aws::S3::Model::GetObjectRequest request;
        request.SetBucket("accounting-23fbf6ce-ff25-4f6a-a75a-b50bf814fc62");
        request.SetKey("/Database/Database.bin");
        request.SetResponseStreamFactory([localPath] {
            return Aws::New<Aws::FStream>(ALLOCATION_TAG, localPath, std::ios_base::out | std::ios_base::binary);
        });

        // FIXME: Async.
        // FIXME: Error handling.
        auto outcome = get_s3_client().GetObject(request);
        Q_ASSERT(outcome.IsSuccess());

        return localPath;
    }

    inline void upload_latest_to_s3(std::filesystem::path localPath) {
        auto inputStream = Aws::MakeShared<Aws::FStream>(ALLOCATION_TAG, localPath, std::ios_base::in | std::ios_base::binary);

        std::string remotePath = fmt::format("/Database/{}", localPath.filename().string());

        Aws::S3::Model::PutObjectRequest request;
        request.SetBucket("accounting-23fbf6ce-ff25-4f6a-a75a-b50bf814fc62");
        request.SetKey(remotePath);
        request.SetBody(inputStream);

        // FIXME: Async.
        // FIXME: Error handling.
        auto outcome = get_s3_client().PutObject(request);
        Q_ASSERT(outcome.IsSuccess());
    }
}
