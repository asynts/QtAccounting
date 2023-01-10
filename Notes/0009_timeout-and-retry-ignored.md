If no network connection is avaliable, it takes forever until the application realizes this.

### Notes

-   I tried setting my own retry strategy and timeouts:

    ```c++
    Aws::Client::ClientConfiguration clientConfiguration;
    clientConfiguration.region = settings.value("AWS/Region").value<QString>().toStdString();
    clientConfiguration.connectTimeoutMs = 1000;
    clientConfiguration.httpRequestTimeoutMs = 1000;
    clientConfiguration.requestTimeoutMs = 1000;
    clientConfiguration.retryStrategy = Aws::MakeShared<Aws::Client::DefaultRetryStrategy>(0);
    ```

-   I get the following warning in the console:

    ```none
    [WARN] 2023-01-09 08:38:43.589 ClientConfiguration [140737258125824] Retry Strategy will use the default max attempts.
    [WARN] 2023-01-09 08:38:43.703 ClientConfiguration [140736047597248] Retry Strategy will use the default max attempts.
    [WARN] 2023-01-09 08:38:50.908 ClientConfiguration [140735466694336] Retry Strategy will use the default max attempts.
    [WARN] 2023-01-09 08:38:51.296 ClientConfiguration [140735466694336] Retry Strategy will use the default max attempts.
    ```

-   I downloaded the source code and started digging:

    -   The message is created in `Aws::Auth::InitRetryStrategy`.

    -   This function is called in the constructor of `Aws::Auth::ClientConfiguration`.
        Thus it will run before I update it by setting my own `retryStrategy`.

-   I tried logging, every time when the retry stratagy was created to corelate it with the AWS output.
    However, it seems that the log output from AWS is cached and not synchronized with my output.

    -   We can count that there are three messages when we call `Aws::Client::ClientConfiguration` but four messages in the
        output of AWS.

-   Somehow, I am not using the version of AWS that I got the source code for.
    The constructor of `ClientConfiguration` clearly takes different arguments.
    When I updated the system library that seems to have caused the issue.

    -   I was able to get the correct source code by manually checking out the revision:
    
        ```none
        git switch --detach 1.10.32
        ```

-   This seems to be the culprit:

    ```c++
    GetObjectOutcome S3Client::GetObject(const GetObjectRequest& request) const
    {
      AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetObject, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
      if (!request.BucketHasBeenSet())
      {
        AWS_LOGSTREAM_ERROR("GetObject", "Required field: Bucket, is not set");
        return GetObjectOutcome(Aws::Client::AWSError<S3Errors>(S3Errors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Bucket]", false));
      }
      if (!request.KeyHasBeenSet())
      {
        AWS_LOGSTREAM_ERROR("GetObject", "Required field: Key, is not set");
        return GetObjectOutcome(Aws::Client::AWSError<S3Errors>(S3Errors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Key]", false));
      }
      ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetObject, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments(request.GetKey());
      return GetObjectOutcome(MakeRequestWithUnparsedResponse(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET));
    }    
    ```

    This doesn't access `m_clientConfiguration` at all.

### Ideas

-   Verify that I am actually passing my client configuration to S3.

### Theories

-   I suspect, that AWS will not use my configuration at all and call the default constructor implicitly.

-   I suspect that AWS has a very large default timeout, possibly with a retry strategy.
