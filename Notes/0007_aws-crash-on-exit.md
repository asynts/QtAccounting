### Notes

-   Initially there was a little issue where the AWS API seems to have changed.
    I quickly updated a code which introduced a `Aws::MakeShared` which seems to cause another issue.

-   Somehow we crash after trying to upload the database to S3.

-   I receive the following error:

    ```none
    Fatal error condition occurred in /build/aws-c-common/src/aws-c-common-0.8.6/source/allocator.c:209: allocator != ((void *)0)
    Exiting Application
    No call stack information available
    ```

-   I suspect this happens when the object in `get_s3_client` is destroyed:

    ```none
    #0  0x00007ffff58a164c in  () at /usr/lib/libc.so.6
    #1  0x00007ffff5851958 in raise () at /usr/lib/libc.so.6
    #2  0x00007ffff583b53d in abort () at /usr/lib/libc.so.6
    #3  0x00007ffff69a270d in  () at /usr/lib/libaws-c-common.so.1
    #4  0x00007ffff69a30bf in  () at /usr/lib/libaws-c-common.so.1
    #5  0x00007ffff69a17d5 in  () at /usr/lib/libaws-c-common.so.1
    #6  0x00007ffff69a17be in  () at /usr/lib/libaws-c-common.so.1
    #7  0x00007ffff69e2da7 in  () at /usr/lib/libaws-c-sdkutils.so.1.0.0
    #8  0x00007ffff69abcc1 in aws_ref_count_release () at /usr/lib/libaws-c-common.so.1
    #9  0x00007ffff69e0677 in aws_endpoints_ruleset_release () at /usr/lib/libaws-c-sdkutils.so.1.0.0
    #10 0x00007ffff69e5447 in  () at /usr/lib/libaws-c-sdkutils.so.1.0.0
    #11 0x00007ffff69abcc1 in aws_ref_count_release () at /usr/lib/libaws-c-common.so.1
    #12 0x00007ffff69e04e7 in aws_endpoints_rule_engine_release () at /usr/lib/libaws-c-sdkutils.so.1.0.0
    #13 0x00007ffff7795d01 in Aws::Crt::Endpoints::RuleEngine::~RuleEngine() () at /usr/lib/libaws-crt-cpp.so
    #14 0x0000555555579ee2 in Aws::Endpoint::DefaultEndpointProvider<Aws::S3::S3ClientConfiguration, Aws::S3::Endpoint::S3BuiltInParameters, Aws::S3::Endpoint::S3ClientContextParameters>::~DefaultEndpointProvider() (this=0x555555cb9560, __in_chrg=<optimized out>) at /usr/include/aws/core/endpoint/DefaultEndpointProvider.h:63
    #15 0x000055555557a0e2 in Aws::S3::Endpoint::S3EndpointProvider::~S3EndpointProvider() (this=0x555555cb9560, __in_chrg=<optimized out>) at /usr/include/aws/s3/S3EndpointProvider.h:110
    #16 0x000055555559b86a in std::destroy_at<Aws::S3::Endpoint::S3EndpointProvider>(Aws::S3::Endpoint::S3EndpointProvider*) (__location=0x555555cb9560) at /usr/include/c++/12.2.0/bits/stl_construct.h:88
    #17 0x000055555559b7a8 in std::allocator_traits<std::allocator<Aws::S3::Endpoint::S3EndpointProvider> >::destroy<Aws::S3::Endpoint::S3EndpointProvider>(std::allocator<Aws::S3::Endpoint::S3EndpointProvider>&, Aws::S3::Endpoint::S3EndpointProvider*) (__a=..., __p=0x555555cb9560) at /usr/include/c++/12.2.0/bits/alloc_traits.h:537
    #18 0x000055555559b5dd in std::_Sp_counted_ptr_inplace<Aws::S3::Endpoint::S3EndpointProvider, std::allocator<Aws::S3::Endpoint::S3EndpointProvider>, (__gnu_cxx::_Lock_policy)2>::_M_dispose() (this=0x555555cb9550) at /usr/include/c++/12.2.0/bits/shared_ptr_base.h:613
    #19 0x00007ffff7497777 in  () at /usr/lib/libaws-cpp-sdk-s3.so
    #20 0x00007ffff74977d0 in Aws::S3::S3Client::~S3Client() () at /usr/lib/libaws-cpp-sdk-s3.so
    #21 0x000055555559058f in std::_Optional_payload_base<Aws::S3::S3Client>::_M_destroy() (this=0x5555555e4120 <Accounting::Persistance::get_s3_client()::s3_client>) at /usr/include/c++/12.2.0/optional:287
    #22 0x000055555558b6d7 in std::_Optional_payload_base<Aws::S3::S3Client>::_M_reset() (this=0x5555555e4120 <Accounting::Persistance::get_s3_client()::s3_client>) at /usr/include/c++/12.2.0/optional:318
    #23 0x0000555555584400 in std::_Optional_payload<Aws::S3::S3Client, false, false, false>::~_Optional_payload() (this=0x5555555e4120 <Accounting::Persistance::get_s3_client()::s3_client>, __in_chrg=<optimized out>) at /usr/include/c++/12.2.0/optional:439
    #24 0x000055555557aacc in std::_Optional_base<Aws::S3::S3Client, false, false>::~_Optional_base() (this=0x5555555e4120 <Accounting::Persistance::get_s3_client()::s3_client>, __in_chrg=<optimized out>) at /usr/include/c++/12.2.0/optional:510
    #25 0x000055555557aae8 in std::optional<Aws::S3::S3Client>::~optional() (this=0x5555555e4120 <Accounting::Persistance::get_s3_client()::s3_client>, __in_chrg=<optimized out>) at /usr/include/c++/12.2.0/optional:705
    #26 0x00007ffff5853fa5 in  () at /usr/lib/libc.so.6
    #27 0x00007ffff5854120 in  () at /usr/lib/libc.so.6
    #28 0x00007ffff583c297 in  () at /usr/lib/libc.so.6
    #29 0x00007ffff583c34a in __libc_start_main () at /usr/lib/libc.so.6
    #30 0x000055555556b375 in _start () at ../sysdeps/x86_64/start.S:115    
    ```

    -   The `__libc_start_main` suggestes that we are cleaning up global objects, but that will run after `Aws::ShutdownAPI` in `main.cpp`.

-   My next approach is to simply create a new object every time.

-   I resolved the problem by creating a new object every time.

### Ideas

-   I currently define a `static` variable in a header, that shouldn't work?

-   Try to create a new stacktrace with the new cleanup change.
