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

-   It seems that `DefaultEndpointProvider` makes use of the configuration:

    ```c++
    void InitBuiltInParameters(const ClientConfigurationT& config) override
    {
        m_builtInParameters.SetFromClientConfiguration(config);
    }
    ```

-   `S3Client::GetObject` ultimately calls `AWSClient::MakeRequestWithUnparsedResponse`:

    ```c++
    StreamOutcome AWSClient::MakeRequestWithUnparsedResponse(const Aws::Http::URI& uri,
        const Aws::AmazonWebServiceRequest& request,
        Http::HttpMethod method,
        const char* signerName,
        const char* signerRegionOverride,
        const char* signerServiceNameOverride) const
    {
        HttpResponseOutcome httpResponseOutcome = AttemptExhaustively(uri, request, method, signerName, signerRegionOverride, signerServiceNameOverride);
        if (httpResponseOutcome.IsSuccess())
        {
            return StreamOutcome(AmazonWebServiceResult<Stream::ResponseStream>(
                httpResponseOutcome.GetResult()->SwapResponseStreamOwnership(),
                httpResponseOutcome.GetResult()->GetHeaders(), httpResponseOutcome.GetResult()->GetResponseCode()));
        }
    
        return StreamOutcome(std::move(httpResponseOutcome));
    }    
    ```

-   `AWSClient::MakeRequestWithUnparsedResponse` then calls `AWSClient::AttemptExhaustively` which retries based on it's `m_retryStrategy`:

    ```c++
    for (long retries = 0;; retries++)
    {
        if(!m_retryStrategy->HasSendToken())

        // ...
    }
    ```
    
-   This `m_retryStrategy` is extracted from the `clientConfiguration` passed to the `AWSClient` constructor:

    ```c++
    AWSClient::AWSClient(const Aws::Client::ClientConfiguration& configuration,
        const std::shared_ptr<Aws::Client::AWSAuthSigner>& signer,
        const std::shared_ptr<AWSErrorMarshaller>& errorMarshaller) :
        // ...
        m_retryStrategy(configuration.retryStrategy),    
        // ...
    ```
    
-   Since `S3Client` inherits from `AWSXMLClient` and thus inherits from `AWSClient`, the configuration is initialized correctly.

-   I build the AWS SDK from source using the following command:

    ```none
    cmake .. -GNinja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=/home/me/.local/lib/custom-aws-sdk-cpp    
    ```
    
    Unfortunately, this seemed to run tests as well.
    Next time, I should try to disable this.

-   I changed the command the next time when I rebuilt it:

    ```none
    cmake .. \
        -GNinja \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_PREFIX_PATH=/home/me/.local/lib/custom-aws-sdk-cpp \
        -DENABLE_TESTING=OFF
        
    ninja
    ```
    
    -   I had to delete a `-Werror` in `set_gcc_warnings` inside of `compiler_settings.cmake`.

    -   If I build it again, I should specify exactly which sub-modules need to be build.

-   Somehow, I am not able to install the SDK after building it:

    ```none
    $ ninja install
    [0/1] Install the project...
    -- Install configuration: "Debug"
    CMake Error at cmake_install.cmake:46 (file):
      file cannot create directory: /usr/local/lib/cmake/AWSSDK.  Maybe need
      administrative privileges.
    
    
    FAILED: CMakeFiles/install.util 
    cd /home/me/src/github.com/aws/aws-sdk-cpp/build && /usr/bin/cmake -P cmake_install.cmake
    ninja: build stopped: subcommand failed.
    ```

-   I updated the command again:

    ```none
    cmake .. \
        -GNinja \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_PREFIX_PATH=/home/me/.local/lib/custom-aws-sdk-cpp \
        -DENABLE_TESTING=OFF \
        -DBUILD_ONLY="s3"
    ```
    
-   I found this command in `build_custom_client_linux.yml`:

    ```none
    cmake $SDK_ROOT/custom-service/aws-cpp-sdk-custom-service \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_PREFIX_PATH="$SDK_ROOT/install" \
        -DAWSSDK_ROOT_DIR="$SDK_ROOT/install" \
        -DBUILD_SHARED_LIBS=ON
    ```    

    Maybe this `AWSSDK_ROOT_DIR` is the issue?

-   This is my new command:

    ```none
    cmake .. \
        -GNinja \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_PREFIX_PATH="/home/me/.local/lib/custom-aws-sdk-cpp" \
        -DENABLE_TESTING=OFF \
        -DBUILD_ONLY="s3" \
        -DAWSSDK_ROOT_DIR="/home/me/.local/lib/custom-aws-sdk-cpp"
        
    ninja
    ```
    
    However, I get a warning that the new variable is not use.

-   I can't figure this out.
    For now, I'll put this on hold since this is only a minor issue.

### Ideas

-   Try to put a debug print in `AWSClient::AttemptExhaustively`.

    -   This requires building the AWS SDK from source.

### Theories
