/**
 * Tests for functions that requires file system.
 */
#include "greentea-client/test_env.h"
#include "mbed.h"
#include "rtos.h"
#include "unity.h"
#include "utest.h"

#include "HeapBlockDevice.h"
#include "LittleFileSystem.h"
#include "pathutil.h"

using namespace pathutil;

using namespace utest::v1;

//--------------------------------------------------------------------------------
// test file system configuration
//--------------------------------------------------------------------------------

static HeapBlockDevice *hb_ptr;
static LittleFileSystem *fs_ptr;

status_t unite_status(status_t s1, status_t s2)
{
    if (s1 == STATUS_ABORT || s2 == STATUS_ABORT) {
        return STATUS_ABORT;
    }
    if (s2 == STATUS_IGNORE || s2 == STATUS_IGNORE) {
        return STATUS_IGNORE;
    }
    return s1;
}

utest::v1::status_t case_setup_handler(const Case *const source, const size_t index_of_case)
{
    status_t status = STATUS_CONTINUE;

    // allocated 8 KB of memory for tests
    hb_ptr = new HeapBlockDevice(64 * 128, 64);
    fs_ptr = new LittleFileSystem("test_bd");

    // create file system and mount it
    fs_ptr->mount(hb_ptr);
    int err = fs_ptr->reformat(hb_ptr);
    if (err) {
        status = STATUS_ABORT;
    }
    errno = 0;
    return unite_status(status, greentea_case_setup_handler(source, index_of_case));
}

utest::v1::status_t case_teardown_handler(const Case *const source, const size_t passed, const size_t failed, const failure_t failure)
{

    fs_ptr->unmount();
    delete fs_ptr;
    delete hb_ptr;

    return greentea_case_teardown_handler(source, passed, failed, failure);
}

static const char *BASE_DIR = "/test_bd";

//--------------------------------------------------------------------------------
// Test helper function to write/read small files
//--------------------------------------------------------------------------------
void test_write_data_1()
{
    char file_path[64];
    join_paths(file_path, BASE_DIR, "test.txt");

    // write test data
    const size_t data_len = 12;
    const uint8_t data[data_len] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    write_data(file_path, data, data_len);

    // read test data
    FILE *file = fopen(file_path, "r");
    TEST_ASSERT_NOT_NULL(file);
    const size_t read_buff_len = 32;
    uint8_t read_buff[read_buff_len];
    size_t read_len = fread(read_buff, 1, read_buff_len, file);
    // check read results
    TEST_ASSERT_EQUAL_UINT(data_len, read_len);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(data, read_buff, data_len);
    TEST_ASSERT_EQUAL(0, errno);
}

void test_read_data_1()
{
    char file_path[64];
    join_paths(file_path, BASE_DIR, "test.txt");

    // write test data
    const size_t data_len = 12;
    const uint8_t data[data_len] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 5 };
    FILE *file = fopen(file_path, "w");
    TEST_ASSERT_NOT_NULL(file);
    fwrite(data, 1, data_len, file);
    fclose(file);
    TEST_ASSERT_EQUAL(0, errno);

    // read test data
    const size_t read_buff_len = 32;
    uint8_t read_buff[read_buff_len];
    int read_len = read_data(file_path, read_buff, read_buff_len);
    // check read results
    TEST_ASSERT_EQUAL(data_len, read_len);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(data, read_buff, data_len);
    TEST_ASSERT_EQUAL(0, errno);
}

void test_write_str_1()
{
    char file_path[64];
    join_paths(file_path, BASE_DIR, "test.txt");

    // write test data
    const char *text = "hello world";
    const size_t text_len = strlen(text);
    write_str(file_path, text);

    // read test data
    FILE *file = fopen(file_path, "r");
    TEST_ASSERT_NOT_NULL(file);
    const size_t read_buff_len = 32;
    char read_buff[read_buff_len];
    size_t read_len = fread(read_buff, 1, read_buff_len, file);
    read_buff[read_len] = '\0';
    // check read results
    TEST_ASSERT_EQUAL(text_len, read_len);
    TEST_ASSERT_EQUAL_STRING(text, read_buff);
    TEST_ASSERT_EQUAL(0, errno);
}

void test_read_str_1()
{
    char file_path[64];
    join_paths(file_path, BASE_DIR, "test.txt");

    // write test data
    const char *text = "hello world";
    const size_t text_len = strlen(text);
    FILE *file = fopen(file_path, "w");
    TEST_ASSERT_NOT_NULL(file);
    fwrite(text, 1, text_len, file);
    fclose(file);
    TEST_ASSERT_EQUAL(0, errno);

    // read test data
    const size_t read_buff_len = 32;
    char read_buff[read_buff_len];
    int read_len = read_str(file_path, read_buff, read_buff_len);
    // check read results
    TEST_ASSERT_EQUAL(text_len, read_len);
    TEST_ASSERT_EQUAL_STRING(text, read_buff);
    TEST_ASSERT_EQUAL(0, errno);
}

//--------------------------------------------------------------------------------
// Test helper function create/delete folders
//--------------------------------------------------------------------------------

void test_makedirs_1()
{
    int ret_code;
    char dir_path[64];
    join_paths(dir_path, BASE_DIR, "abc/some_dir/test");

    // create test directory
    ret_code = makedirs(dir_path, 0777);
    TEST_ASSERT_EQUAL(0, ret_code);
    TEST_ASSERT_EQUAL(true, exists(dir_path));
    TEST_ASSERT_EQUAL(0, errno);

    // try to create directory again
    ret_code = makedirs(dir_path, 0777);
    TEST_ASSERT_NOT_EQUAL(0, ret_code);
    TEST_ASSERT_EQUAL(true, exists(dir_path));
    TEST_ASSERT_NOT_EQUAL(0, errno);
    errno = 0;

    // try to create directory again with corresponding flag
    ret_code = makedirs(dir_path, 0777, true);
    TEST_ASSERT_EQUAL(0, ret_code);
    TEST_ASSERT_EQUAL(true, exists(dir_path));
    TEST_ASSERT_EQUAL(0, errno);
}

void test_rmtree_1()
{
    char path[128];

    // create directory wit test content
    join_paths(path, BASE_DIR, "test/test_dir/dir_abc");
    makedirs(path);
    join_paths(path, BASE_DIR, "test/test_dir/dir_abc/test_file_1.txt");
    write_data(path, (const uint8_t *)"test 1", 5);
    join_paths(path, BASE_DIR, "test/test_dir/test_file_2.txt");
    write_data(path, (const uint8_t *)"test 2", 5);
    TEST_ASSERT_EQUAL(0, errno);

    join_paths(path, BASE_DIR, "test/test_dir");
    TEST_ASSERT_EQUAL(true, exists(path));
    int ret_code = rmtree(path);
    TEST_ASSERT_EQUAL(0, ret_code);
    TEST_ASSERT_EQUAL(0, errno);
    TEST_ASSERT_EQUAL(false, exists(path));

    join_paths(path, BASE_DIR, "test");
    TEST_ASSERT_EQUAL(true, exists(path));
}

void test_rmtree_2()
{
    char path[128];
    join_paths(path, BASE_DIR, "test/test_dir");
    TEST_ASSERT_EQUAL(false, exists(path));

    int ret_code = rmtree(path);
    TEST_ASSERT_NOT_EQUAL(0, ret_code);
    TEST_ASSERT_NOT_EQUAL(0, errno);
}

//--------------------------------------------------------------------------------
// Test helper function to check files
//--------------------------------------------------------------------------------

void test_isdir_1()
{
    char file_path[64];
    join_paths(file_path, BASE_DIR, "some_file.txt");
    write_data(file_path, (const uint8_t *)"abcd", 4);

    char dir_path[64];
    join_paths(dir_path, BASE_DIR, "some_dir/test");
    makedirs(dir_path);

    char nothing_path[64];
    join_paths(nothing_path, BASE_DIR, "some_dir/note_exists.d");

    TEST_ASSERT_EQUAL(false, isdir(file_path));
    TEST_ASSERT_EQUAL(0, errno);
    TEST_ASSERT_EQUAL(true, isdir(dir_path));
    TEST_ASSERT_EQUAL(0, errno);
    TEST_ASSERT_EQUAL(false, isdir(nothing_path));
    TEST_ASSERT_EQUAL(0, errno);
}

void test_isfile_1()
{
    char file_path[64];
    join_paths(file_path, BASE_DIR, "some_file.txt");
    write_data(file_path, (const uint8_t *)"abcd", 4);

    char dir_path[64];
    join_paths(dir_path, BASE_DIR, "some_dir/test");
    makedirs(dir_path);

    char nothing_path[64];
    join_paths(nothing_path, BASE_DIR, "some_dir/note_exists.d");

    TEST_ASSERT_EQUAL(true, isfile(file_path));
    TEST_ASSERT_EQUAL(0, errno);
    TEST_ASSERT_EQUAL(false, isfile(dir_path));
    TEST_ASSERT_EQUAL(0, errno);
    TEST_ASSERT_EQUAL(false, isfile(nothing_path));
    TEST_ASSERT_EQUAL(0, errno);
}

void test_exists_1()
{
    char file_path[64];
    join_paths(file_path, BASE_DIR, "some_file.txt");
    write_data(file_path, (const uint8_t *)"abcd", 4);

    char dir_path[64];
    join_paths(dir_path, BASE_DIR, "some_dir/test");
    makedirs(dir_path);

    char nothing_path[64];
    join_paths(nothing_path, BASE_DIR, "some_dir/note_exists.d");

    TEST_ASSERT_EQUAL(true, exists(file_path));
    TEST_ASSERT_EQUAL(0, errno);
    TEST_ASSERT_EQUAL(true, exists(dir_path));
    TEST_ASSERT_EQUAL(0, errno);
    TEST_ASSERT_EQUAL(false, exists(nothing_path));
    TEST_ASSERT_EQUAL(0, errno);
}

void test_getsize_1()
{
    char file_path[64];
    join_paths(file_path, BASE_DIR, "some_file.txt");
    write_data(file_path, (const uint8_t *)"abcd", 4);

    char dir_path[64];
    join_paths(dir_path, BASE_DIR, "some_dir/test");
    makedirs(dir_path);

    char nothing_path[64];
    join_paths(nothing_path, BASE_DIR, "some_dir/note_exists.d");

    TEST_ASSERT_EQUAL(4, getsize(file_path));
    TEST_ASSERT_EQUAL(0, errno);
    TEST_ASSERT_NOT_EQUAL(-1, getsize(dir_path));
    TEST_ASSERT_EQUAL(0, errno);
    TEST_ASSERT_EQUAL(-1, getsize(nothing_path));
    TEST_ASSERT_NOT_EQUAL(0, errno);
}

//--------------------------------------------------------------------------------
// Test helper function to read directory
//--------------------------------------------------------------------------------

void test_readdir_child_1()
{
    char dir_path[64];
    join_paths(dir_path, BASE_DIR, "test_dir");
    mkdir(dir_path, 0777);

    char file_path[64];
    join_paths(file_path, dir_path, "file_1.txt");
    write_str(file_path, "hello world");
    join_paths(file_path, dir_path, "file_2.txt");
    write_str(file_path, "hello world");
    join_paths(file_path, dir_path, "file_3.txt");
    write_str(file_path, "hello world");

    int num_files = 0;
    DIR *dir_p;
    struct dirent *dirent_p;

    if ((dir_p = opendir(dir_path)) == NULL) {
        TEST_FAIL();
        return;
    }
    while ((dirent_p = readdir_child(dir_p))) {
        num_files += 1;
    }
    if (closedir(dir_p)) {
        TEST_FAIL();
        return;
    }

    TEST_ASSERT_EQUAL(3, num_files);
}

void test_readdir_child_2()
{
    char dir_path[64];
    join_paths(dir_path, BASE_DIR, "test_dir");
    mkdir(dir_path, 0777);

    int num_files = 0;
    DIR *dir_p;
    struct dirent *dirent_p;

    if ((dir_p = opendir(dir_path)) == NULL) {
        TEST_FAIL();
        return;
    }
    while ((dirent_p = readdir_child(dir_p))) {
        num_files += 1;
    }
    if (closedir(dir_p)) {
        TEST_FAIL();
        return;
    }

    TEST_ASSERT_EQUAL(0, num_files);
}

// test cases description
#define FSSimpleCase(test_fun) Case(#test_fun, case_setup_handler, test_fun, case_teardown_handler, greentea_case_failure_continue_handler)
Case cases[] = {
    FSSimpleCase(test_write_data_1),
    FSSimpleCase(test_read_data_1),
    FSSimpleCase(test_write_str_1),
    FSSimpleCase(test_read_str_1),
    FSSimpleCase(test_makedirs_1),
    FSSimpleCase(test_rmtree_1),
    FSSimpleCase(test_rmtree_2),
    FSSimpleCase(test_isdir_1),
    FSSimpleCase(test_isfile_1),
    FSSimpleCase(test_exists_1),
    FSSimpleCase(test_getsize_1),
    FSSimpleCase(test_readdir_child_1),
    FSSimpleCase(test_readdir_child_2),
};
Specification specification(greentea_test_setup_handler, cases, greentea_test_teardown_handler);

// Entry point into the tests
int main()
{
    // host handshake
    // note: should be invoked here or in the test_setup_handler
    GREENTEA_SETUP(40, "default_auto");
    // run tests
    return !Harness::run(specification);
}
