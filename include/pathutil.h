#ifndef PATHUTIL_H
#define PATHUTIL_H

#include "mbed.h"

namespace pathutil {

/**
 * Remove directory recursively.
 *
 * @param path directory path
 * @param buff helper buffer to internal operations (it should have size that is enough for longes path in the directory).
 *             If it isn't set, it will be allocated dynamically.
 * @param bull_len length of internal buffer.
 * @return 0, if directory has been deleted successfully, otherwise non-zero value.
 */
int rmtree(const char* path, char* buff = NULL, size_t buff_len = 0);

/**
 * Create directory and parent one, if they are missed.
 *
 * @param path
 * @param exists_ok
 * @param buff - buffer for internal operations. It should have size same as path. If it isn't set, it will be allocated dynamically.
 * @param buff_len - length of internal buffer.
 * @return
 */
int makedirs(const char* path, mode_t mode = 0777, bool exists_ok = false, char* buff = NULL, size_t buff_len = 0);

/**
 * Check if given path is directory.
 *
 * @param path
 * @return
 */
bool isdir(const char* path);

/**
 * Check if give file is regular file.
 *
 * @param path
 * @return
 */
bool isfile(const char* path);

/**
 * Check if given path exists.
 *
 * @param path
 * @return
 */
bool exists(const char* path);

/**
 * Get file size.
 *
 * @param path
 * @return
 */
ssize_t getsize(const char* path);

/**
 * Check if given path is absolute.
 *
 * @param path
 * @return
 */
bool isabs(const char* path);

/**
 * Join paths.
 *
 * @param output_path output path buffer
 * @param n length of output_path buffer
 * @param path_l left part of path
 * @param path_r right path of path
 * @return
 */
int join_paths(char* output_path, size_t n, const char* path_l, const char* path_r);

/**
 * Join paths.
 *
 * @param output_path output path string. It should be large enough for result.
 * @param path_l left part of path
 * @param path_r right path of path
 * @return
 */
int join_paths(char* output_path, const char* path_l, const char* path_r);

/**
 * Append part to current path.
 *
 * @param path current path
 * @param n length of current path buffer
 * @param path_r path that should be appended
 * @return
 */
int append_path(char* path, size_t n, const char* path_r);

/**
 * Append part to current path.
 *
 * @param path current path. It should be large enough for result.
 * @param path_r path that should be appended
 * @return
 */
int append_path(char* path, const char* path_r);
/**
 * Normalize path.
 *
 * @param path
 * @return
 */
int normpath(char* path);

/**
 * Write data to file.
 *
 * @param path file path
 * @param data buffer with data
 * @param len data length
 * @return 0 on success, or negative value on error
 */
int write_data(const char* path, const uint8_t* data, size_t len);

/**
 * Read data from file.
 *
 * @param path file path
 * @param data buffer to save data
 * @param len buffer length
 * @return negative value if buffer is too small, or number of read data
 */
int read_data(const char* path, uint8_t* data, size_t len);
}
#endif // PATHUTIL_H
