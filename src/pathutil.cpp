#include "string.h"

#include "pathutil.h"
using namespace pathutil;

#define SEP '/'

static int rmtree_impl(char* path_buff, size_t path_len, size_t buff_len)
{
    DIR* dir;
    struct dirent* dir_entity;
    int ret_code = 0;
    int tmp_ret_code;
    int origin_errno;
    size_t sub_path_len;
    uint8_t de_type;

    if ((dir = opendir(path_buff)) == NULL) {
        return -1;
    }

    origin_errno = errno;
    errno = 0;
    // remove directory content
    while ((dir_entity = readdir(dir)) != NULL) {
        // ignore special entries "." and ".."
        if (!strcmp(dir_entity->d_name, ".") || !strcmp(dir_entity->d_name, "..")) {
            continue;
        }
        // delete content
        de_type = dir_entity->d_type;
        sub_path_len = path_len + strlen(dir_entity->d_name) + 1;
        // check that buffer can store full path of directory entry
        if (sub_path_len + 1 > buff_len) {
            errno = ENOBUFS;
            ret_code = -1;
            break;
        }
        path_buff[path_len] = SEP;
        strcpy(path_buff + path_len + 1, dir_entity->d_name);
        path_buff[sub_path_len] = '\0';
        // remove directory item
        switch (de_type) {
        case DT_DIR:
            ret_code = rmtree_impl(path_buff, sub_path_len, buff_len);
            break;
        case DT_REG:
        case DT_LNK:
            ret_code = remove(path_buff);
            break;
        default:
            // unsupported type
            errno = EPERM;
            ret_code = -1;
            break;
        }
    }
    // "restore" buffer
    path_buff[path_len] = '\0';
    if (errno == 0) {
        errno = origin_errno;
    } else {
        ret_code = -1;
    }

    tmp_ret_code = closedir(dir);
    if (tmp_ret_code && !ret_code) {
        ret_code = tmp_ret_code;
    }

    // remove directory itself
    if (!ret_code) {
        ret_code = remove(path_buff);
    }

    return ret_code;
}

#define DEFAULT_RMTREE_BUFF_SIZE 256

int pathutil::rmtree(const char* path, char* buff, size_t buff_len)
{
    bool cleanup_buff = false;
    int ret_code = 0;

    size_t path_len = strlen(path);

    if (buff == NULL) {
        // check that buffer can store current path
        if (path_len + 1 > DEFAULT_RMTREE_BUFF_SIZE) {
            errno = ENOBUFS;
            return -1;
        }
        buff = new char[DEFAULT_RMTREE_BUFF_SIZE];
        buff_len = DEFAULT_RMTREE_BUFF_SIZE;
    } else {
        // check that buffer can store current path
        if (path_len + 1 > buff_len) {
            errno = ENOBUFS;
            return -1;
        }
    }
    strcpy(buff, path);

    ret_code = rmtree_impl(buff, path_len, buff_len);

    if (cleanup_buff) {
        delete[] buff;
    }
    return ret_code;
}

int pathutil::makedirs(const char* path, mode_t mode, bool exists_ok, char* buff, size_t buff_len)
{
    bool cleanup_buff = false;
    int ret_code = 0;
    char* pos;
    char* first_existed_pos;
    char* buff_end;
    char sym;
    bool is_dir_flag;
    // note due mbed-os implementation, we cannot explicitly create directories in the root "/",
    // so we should ignore them
    bool top_dir_flag = false;
    size_t path_len = strlen(path);

    if (!isabs(path)) {
        // relative paths aren't supported
        errno = ENOENT;
        return -1;
    }

    if (buff == NULL) {
        buff = new char[path_len + 1];
        cleanup_buff = true;
    } else if (path_len + 1 > buff_len) {
        errno = ENOBUFS;
        return -1;
    }

    // copy and normalize path
    strcpy(buff, path);
    normpath(buff);
    path_len = strlen(buff);
    buff_end = buff + path_len;

    // determine which directory aren't exists
    first_existed_pos = buff_end;
    while (true) {
        sym = *first_existed_pos;
        *first_existed_pos = '\0';
        is_dir_flag = isdir(buff);
        *first_existed_pos = sym;
        if (is_dir_flag) {
            break;
        }

        do {
            first_existed_pos--;
        } while (first_existed_pos > buff && *first_existed_pos != SEP);
        if (first_existed_pos == buff) {
            top_dir_flag = true;
            break;
        }
    }

    if (first_existed_pos == buff_end) {
        // given directory exists
        if (!exists_ok) {
            errno = EEXIST;
            ret_code = -1;
        }
    } else {
        // create directories
        pos = first_existed_pos + 1;
        while (pos <= buff_end) {
            sym = *pos;
            if (sym == SEP || sym == '\0') {
                *pos = '\0';
                if (!top_dir_flag) {
                    ret_code = mkdir(buff, mode);
                } else {
                    ret_code = 0;
                    top_dir_flag = false;
                }
                *pos = sym;
                if (ret_code) {
                    break;
                }
            }
            pos++;
        }
    }

    if (cleanup_buff) {
        delete[] buff;
    }
    return ret_code;
}

bool pathutil::isdir(const char* path)
{
    struct stat path_stat;
    if (stat(path, &path_stat)) {
        errno = 0;
        return false;
    } else {
        return (path_stat.st_mode & S_IFMT) == S_IFDIR;
    }
}

bool pathutil::isfile(const char* path)
{
    struct stat path_stat;
    if (stat(path, &path_stat)) {
        errno = 0;
        return false;
    } else {
        return (path_stat.st_mode & S_IFMT) == S_IFREG;
    }
}

bool pathutil::exists(const char* path)
{
    struct stat path_stat;
    if (stat(path, &path_stat)) {
        errno = 0;
        return false;
    } else {
        return true;
    }
}

ssize_t pathutil::getsize(const char* path)
{
    struct stat path_stat;
    if (stat(path, &path_stat)) {
        return -1;
    } else {
        return path_stat.st_size;
    }
}

bool pathutil::isabs(const char* path)
{
    if (path[0] == '\0') {
        return false;
    } else {
        return path[0] == SEP;
    }
}

int pathutil::join_paths(char* output_path, size_t n, const char* path_l, const char* path_r)
{
    size_t part_l_len = strlen(path_l);
    size_t part_r_len = strlen(path_r);
    size_t out_len;
    int ret_code = 0;

    if (isabs(path_r) || *path_l == '\0') {
        if (part_r_len + 1 > n) {
            ret_code = -1;
            errno = ENOBUFS;
        } else {
            join_paths(output_path, path_l, path_r);
        }
    } else {
        out_len = part_l_len + part_r_len;
        if (path_l[part_l_len - 1] != SEP) {
            out_len += 1;
        }
        if (out_len + 1 > n) {
            ret_code = -1;
            errno = ENOBUFS;
        } else {
            join_paths(output_path, path_l, path_r);
        }
    }

    return ret_code;
}

int pathutil::join_paths(char* output_path, const char* path_l, const char* path_r)
{
    char* out_pos;
    const char* pos;
    int ret_code = 0;

    if (isabs(path_r) || *path_l == '\0') {
        strcpy(output_path, path_r);
    } else {
        for (out_pos = output_path, pos = path_l; *pos != '\0'; pos++, out_pos++) {
            *out_pos = *pos;
        }
        if (out_pos[-1] != SEP) {
            *out_pos = SEP;
            out_pos++;
        }
        for (pos = path_r; *pos != '\0'; pos++, out_pos++) {
            *out_pos = *pos;
        }
        *out_pos = '\0';
    }

    return ret_code;
}

int pathutil::append_path(char* path, size_t n, const char* path_r)
{
    int ret_code = 0;
    char* pos;
    size_t path_r_len = strlen(path_r);
    size_t path_len;

    if (isabs(path_r) || path[0] == '\0') {
        if (path_r_len + 1 > n) {
            ret_code = -1;
            errno = ENOBUFS;
        } else {
            append_path(path, path_r);
        }
    } else {
        path_len = strlen(path);
        if (path[path_len - 1] != SEP) {
            path_len += 1;
        }
        path_len += path_r_len;
        if (path_len + 1 > n) {
            ret_code = -1;
            errno = ENOBUFS;
        } else {
            append_path(path, path_r);
        }
    }

    return ret_code;
}

int pathutil::append_path(char* path, const char* path_r)
{
    int ret_code = 0;
    char* pos;

    if (isabs(path_r) || path[0] == '\0') {
        strcpy(path, path_r);
    } else {
        pos = path + strlen(path);
        if (pos[-1] != SEP) {
            pos[0] = SEP;
            pos++;
        }
        strcpy(pos, path_r);
    }

    return ret_code;
}

int pathutil::normpath(char* path)
{
    char* pos = path - 1;
    bool stop_flag = false;
    bool abs_flag = false;
    char sym;
    char* new_pos = path;
    char* prev_sep = path - 1;
    ssize_t sep_dist;
    char* prev_prev_sep;

    sym = *path;
    // skip empty path
    if (sym == '\0') {
        return 0;
    } else if (sym == SEP) {
        abs_flag = true;
    }

    while (!stop_flag) {
        sym = *(++pos);
        if (sym == '\0') {
            stop_flag = true;
            sym = SEP;
        }
        new_pos[0] = sym;

        if (sym == SEP) {
            sep_dist = new_pos - prev_sep;

            if (sep_dist == 1 && pos != path) {
                // collapse separators
                new_pos -= 1;
            } else if (sep_dist == 2 && new_pos[-1] == '.') {
                // collapse "/./"
                new_pos -= 2;
            } else if (sep_dist == 3 && new_pos[-1] == '.' && new_pos[-2] == '.') {
                // try to collapse "somedir/../"
                if (prev_sep == path) {
                    // collapse "/../" to "/"
                    new_pos = prev_sep;
                } else {

                    prev_prev_sep = prev_sep - 1;
                    while (prev_prev_sep >= path && *prev_prev_sep != SEP) {
                        prev_prev_sep--;
                    }
                    if (prev_sep < path) {
                        // "../" - nothing to collapse
                    } else if ((prev_sep - prev_prev_sep) == 3 && prev_sep[-1] == '.' && prev_sep[-2] == '.') {
                        // "../../" - nothing to collapse
                    } else {
                        // collapse "<some_path>/abc/../" to "<some_path>/"
                        prev_sep = prev_prev_sep;
                        new_pos = prev_sep;
                    }
                }
            } else {
                // the part is normalized. Skip ...
                prev_sep = new_pos;
            }
        }
        new_pos++;
    }

    if (new_pos == path) {
        // path is reduced to empty path
        path[0] = '.';
        path[1] = '\0';
    } else {
        // remove trailing "/"
        new_pos[-1] = '\0';
        // process path collapse case
        if (abs_flag && path[0] == '\0') {
            path[0] = SEP;
            path[1] = '\0';
        }
    }

    return 0;
}

int pathutil::write_data(const char* path, const uint8_t* data, size_t len)
{
    FILE* file;
    int ret_code = 0;
    int close_ret_code = 0;
    size_t write_res;

    if ((file = fopen(path, "wb")) == NULL) {
        return -1;
    }

    write_res = fwrite(data, sizeof(uint8_t), len, file);
    if (write_res != len) {
        ret_code = -1;
        errno = EIO;
    }

    close_ret_code = fclose(file);
    if (close_ret_code && !ret_code) {
        ret_code = close_ret_code;
    }
    return ret_code;
}

int pathutil::read_data(const char* path, uint8_t* data, size_t len)
{
    FILE* file;
    int ret_code = 0;
    int close_ret_code = 0;
    int read_size;
    int file_size;

    if ((file = fopen(path, "rb")) == NULL) {
        return -1;
    }

    // get file size
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    if (file_size > len) {
        ret_code = -1;
        errno = ENOBUFS;
    } else {
        read_size = fread(data, sizeof(uint8_t), len, file);
        if (read_size != file_size) {
            ret_code = -1;
            errno = EIO;
        }
    }

    close_ret_code = fclose(file);
    if (close_ret_code && !ret_code) {
        ret_code = close_ret_code;
    }

    return ret_code ? ret_code : read_size;
}
