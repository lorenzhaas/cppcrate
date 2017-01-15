# CppCrate

[![Build Status](https://travis-ci.org/lorenzhaas/cppcrate.svg?branch=master)](https://travis-ci.org/lorenzhaas/cppcrate)

## Introduction

CppCrate is a C++ interface for Crate http://crate.io. It supports normal SQL based queries –
including statements with parameter substitution and bulk operations – and provides an interface to
manage blob data.



## Version Notice

CppCrate is in a very early development phase so there are no API guaranties currently. The API may
change with any release.



## API Documentation

Using CppCrate is straight forward: Create a client, connect to a node and make your request:

```cpp
CppCrate::Client client;
if (client.connect("localhost:4200")) {
  CppCrate::Result result = client.exec("SELECT id, name FROM players");
  if (result) {
    for (int i = 0, total = result.recordSize(); i < total; ++i) {
      CppCrate::Record record = result.record(i);
      std::cout << "Result " << (i+1) << ": " 
                << record.value("name").asString()
                << " (" << record.value("id").asString() << ")\n";
    }
  }
}
```

For a complete documentation please refer to https://lorenzhaas.github.io/cppcrate.



## Building CppCrate

CppCrate uses [CMake](https://cmake.org/) and requires that [CURL](https://curl.haxx.se/) is
available. Anything else is standard:

 1. Clone the repository
 2. Run CMake
 3. Run `make`

For CMake CppCrate provides these options:

 - **ENABLE_BLOB_SUPPORT** If enabled, CppCrate also provides an interface to deal with BLOB data.
 - **ENABLE_CPP11_SUPPORT** If enabled, CppCrate uses C++11 features to improve performance. This
   requires a C++11 compatible compiler of course.
