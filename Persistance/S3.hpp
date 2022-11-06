#pragma once

#include <QString>

#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>

namespace Accounting::Persistance
{
    void upload_file(QString localPath, QString remotePath) {
        std::string remotePath_std = remotePath.toStdString();
        std::string localPath_std = localPath.toStdString();

        // FIXME: Reuse.
        Aws::S3::S3Client s3_client;

        Aws::S3::Model::PutObjectRequest request;
        request.SetBucket("accounting-23fbf6ce-ff25-4f6a-a75a-b50bf814fc62");
        request.SetKey(remotePath_std);

        std::shared_ptr<Aws::IOStream> inputData = Aws::MakeShared<Aws::FStream>("S3", localPath_std, std::ios_base::in | std::ios_base::binary);
        Q_ASSERT(*inputData);

        request.SetBody(inputData);

        // FIXME: Do this asynchronous
        // FIXME: Deal with errors
        auto outcome = s3_client.PutObject(request);
        Q_ASSERT(outcome.IsSuccess());
    }
}
