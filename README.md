## QtAccounting

This is an application I created to get familiar with the Qt framework.
It is designed for my own very specific usecase and not useful to anyone else.

Essentially, this is a GUI to edit the database defined in [Database.hpp](Persistance/Database.hpp).
The database file is fetched from S3 and re-uploaded when exiting the application.

### Installation

 1. Install the following packages:

    ```none
    pacman -S --needed aws-sdk-cpp qtcreator
    ```

 1. Install all the other stuff that I forgot to mention.

 1. Setup an S3 bucket and create the following policy to grant access to it:
 
    ```json
    {
        "Version": "2012-10-17",
        "Statement": [
            {
                "Sid": "VisualEditor0",
                "Effect": "Allow",
                "Action": [
                    "s3:PutObject",
                    "s3:GetObjectAcl",
                    "s3:GetObject",
                    "s3:ListBucket",
                    "s3:PutObjectAcl"
                ],
                "Resource": [
                    "arn:aws:s3:::${BUCKET_NAME}/*",
                    "arn:aws:s3:::${BUCKET_NAME}"
                ]
            }
        ]
    }
    ```

 1. Create a configuration file at `$HOME/.config/Asynts/Accounting-Debug.conf`:
 
    ```ini
    [Receiver]
    Name=Max Musterman
    IBAN=AA00 0000 0000 0000 0000 00
    
    [AWS]
    Region=eu-central-1
    AccessKeyId=${ACCESS_KEY_ID}
    SecretKey=${SECRET_KEY}
    BucketName=${BUCKET_NAME}
    ```

    For production another configuraiton file needs to be created at `$HOME/.config/Asynts/Accounting.conf`.
