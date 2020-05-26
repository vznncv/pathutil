# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Changed

- Refactor `write_data`, `read_data`, `write_str`, `read_str` functions to use system calls `open`,
  `read`, `write`, `close` instead of `fopen`, `fread`, `fwrite`, `fclose` to prevent dynamic memory
  allocation/deallocation by a C library.
  
## [0.2.0] - 2020-03-09
### Added

- Add `dirname`, `basename`, `cleartree`, `is_child_dirent`, `readdir_children`, `write_str`, `read_str` functions.

## [0.1.0] - 2019-01-31
### Added

- Add `rmtree`, `makedirs`, `isdir`, `isfile`, `exists`, `getsize`, `isabs`, `join_paths`,
  `append_path`, `normpath`, `write_data`, `read_data` functions.
