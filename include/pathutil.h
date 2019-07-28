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
int rmtree(const char *path, char *buff = NULL, size_t buff_len = 0);

/**
 * Create directory and parent one, if they are missed.
 *
 * @param path
 * @param exists_ok
 * @param buff - buffer for internal operations. It should have size same as path. If it isn't set, it will be allocated dynamically.
 * @param buff_len - length of internal buffer.
 * @return 0 on success, otherwise non-zero value
 */
int makedirs(const char *path, mode_t mode = 0777, bool exists_ok = false, char *buff = NULL, size_t buff_len = 0);

/**
 * Check if given path is directory.
 *
 * @param path
 * @return
 */
bool isdir(const char *path);

/**
 * Check if give file is regular file.
 *
 * @param path
 * @return
 */
bool isfile(const char *path);

/**
 * Check if given path exists.
 *
 * @param path
 * @return
 */
bool exists(const char *path);

/**
 * Get file size.
 *
 * @param path
 * @return file size on success, otherwise negative value
 */
ssize_t getsize(const char *path);

/**
 * Check if given path is absolute.
 *
 * @param path
 * @return
 */
bool isabs(const char *path);

/**
 * Join paths.
 *
 * @param output_path output path buffer
 * @param n length of output_path buffer
 * @param path_l left part of path
 * @param path_r right path of path
 * @return  0 on success, otherwise non-zero value (the output buffer is too small)
 */
int join_paths(char *output_path, size_t n, const char *path_l, const char *path_r);

/**
 * Join paths.
 *
 * @param output_path output path string. It should be large enough for result.
 * @param path_l left part of path
 * @param path_r right path of path
 * @return 0
 */
int join_paths(char *output_path, const char *path_l, const char *path_r);

/**
 * Append part to current path.
 *
 * @param path current path
 * @param n length of current path buffer
 * @param path_r path that should be appended
 * @return 0 on success, otherwise non-zero value (the output buffer is too small)
 */
int append_path(char *path, size_t n, const char *path_r);

/**
 * Append part to current path.
 *
 * @param path current path. It should be large enough for result.
 * @param path_r path that should be appended
 * @return 0
 */
int append_path(char *path, const char *path_r);

/**
 * Normalize path.
 *
 * @param path
 * @return 0
 */
int normpath(char *path);

/**
 * Get base name of the path.
 *
 * @param basename base name buffer
 * @param n maximal length of \p basename buffer
 * @param path
 * @return 0 on success, otherwise non-zero value (the output buffer is too small)
 */
int basename(char *basename, size_t n, const char *path);

/**
 * Get base name of the path.
 *
 * @param basename base name buffer
 * @param path
 * @return 0
 */
int basename(char *basename, const char *path);

/**
 * Get parent directory name.
 *
 * @param path
 * @return 0
 */
int dirname(char *path);

/**
 * Get parent directory name.
 *
 * @param dirpath
 * @param n maximal length of \p dirpath buffer
 * @param path
 * @return 0 on success, otherwise non-zero value (the output buffer is too small)
 */
int dirname(char *dirpath, size_t n, const char *path);

/**
 * Get parent directory name.
 *
 * @param dirpath
 * @param path
 * @return 0
 */
int dirname(char *dirpath, const char *path);

/**
 * Check if a \p name is name of the subdirectory of file in the directory.
 *
 * I.e. it returns \c false if \p name is name of the current directory "." or parent one "..".
 *
 * @param dir_ent
 * @return true or false
 */
bool is_child_dirent(const char *name);

/**
 * Check if \c dir_ent contains name of the subdirectory of file in the directory.
 *
 * I.e. it returns \c false if \p dir_ent contains name of the current directory "." or parent one "..".
 *
 * @param dir_ent
 * @return true or false
 */
bool is_child_dirent(struct dirent *dir_ent);

/**
 * Helper wrapper around \c readdir, that has the same behaviour, but skips current "." and parent ".." directories.
 *
 * @param dirp
 * @return
 */
struct dirent *readdir_child(DIR *dirp);

/**
 * Write data to file.
 *
 * @param path file path
 * @param data buffer with data
 * @param len data length
 * @return 0 on success, or negative value on error
 */
int write_data(const char *path, const uint8_t *data, size_t len);

/**
 * Read data from file.
 *
 * @param path file path
 * @param data buffer to save data
 * @param len buffer length
 * @return negative value if buffer is too small, or number of read data
 */
int read_data(const char *path, uint8_t *data, size_t len);

/**
 * Write ordinary \c char string to a file.
 *
 * @param path file path
 * @param text string to save
 * @return 0 on success, or negative value on error
 */
int write_str(const char *path, const char *text);

/**
 * Read ordinary \c char string from a file.
 *
 * note: string terminator symbol \c '\0' is added automatically.
 *
 * @param path file path
 * @param text buffer to be written a text from the file
 * @param len maximal string length not including zero terminal character
 * @return real string length, or negative value on error of if buffer is too small
 */
int read_str(const char *path, char *text, size_t len);
}
#endif // PATHUTIL_H
