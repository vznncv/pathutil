# Pathutil library

Helper library for mbed-os to manipulate file system paths.

It has helper functions over standard ones to simplify paths, files and directory manipulation.

Available functions:

- `rmtree` - remove directory recursively
- `makedirs` - create directory and it's parent
- `isdir` - check if path is directory
- `isfile` - check if path is regular file
- `exists` - check if path exists
- `getsize` - get size of the file/directory
- `isabs` - check if path is absolute
- `join_paths` - concatenate two paths
- `append_path` - append one path to another
- `normpath` - normalize path
- `write_data` - write data to file from buffer
- `read_data` - read data from file to buffer

## Test

The library has [greentee](https://github.com/ARMmbed/mbed-os-tools/) test. So you can
be run on a MCU, using mbed-cli. Steps to run tests:

- create an empty project mbed for your board/MCU
- add this library to your project: `mbed add <library_url>`
- run tests: `mbed test --greentea --tests-by-name "pathutil-*"`
