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

### Theories

-   I suspect that AWS has a very large default timeout, possibly with a retry strategy.
