set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

cmake_policy(PUSH)

if(POLICY CMP0135)
    # Allow calling FetchContent_Populate directly.
    cmake_policy(SET CMP0135 OLD)
endif()

include(FetchContent)

message(STATUS "Pulling and configuring DynCall")

FetchContent_Declare(
        DynCall
        URL      https://dyncall.org/r1.4/dyncall-1.4.tar.gz
        URL_HASH MD5=3a82364439b9e0ca5477677823421c1f
)

FetchContent_MakeAvailable(DynCall)

cmake_policy(POP)