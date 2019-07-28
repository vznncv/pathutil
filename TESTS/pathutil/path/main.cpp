/**
 * Tests for functions that modify path, and don't require real file system.
 */
#include "string.h"

#include "greentea-client/test_env.h"
#include "mbed.h"
#include "rtos.h"
#include "unity.h"
#include "utest.h"

#include "pathutil.h"

using namespace utest::v1;
using namespace pathutil;

//--------------------------------------------------------------------------------
// Test normpath function
//--------------------------------------------------------------------------------
void assert_normpath_result(const char *input_path, const char *expected)
{
    char path[128];
    strcpy(path, input_path);
    int ret_code = normpath(path);
    TEST_ASSERT_EQUAL(0, ret_code);
    TEST_ASSERT_EQUAL_STRING(expected, path);
}

void test_normpath_simple_1()
{
    assert_normpath_result("/some/path/test.txt", "/some/path/test.txt");
}

void test_normpath_collapse_dot_1()
{
    assert_normpath_result("./file.txt", "file.txt");
}

void test_normpath_collapse_dot_2()
{
    assert_normpath_result("/path/./a/././test", "/path/a/test");
}

void test_normpath_collapse_sep_1()
{
    assert_normpath_result("///pp///sdf///", "/pp/sdf");
}

void test_normpath_collapse_sep_2()
{
    assert_normpath_result("ff//", "ff");
}

void test_normpath_collapse_dot_sep_1()
{
    assert_normpath_result("/path//./test.txt", "/path/test.txt");
}

void test_normpath_collapse_dot_sep_2()
{
    assert_normpath_result("/path//.//test.txt//.//.///", "/path/test.txt");
}

void test_normpath_collapse_dot_sep_3()
{
    assert_normpath_result(".//./", ".");
}

void test_normpath_collapse_dot_sep_4()
{
    assert_normpath_result("./", ".");
}

void test_normpath_collapse_dot_sep_5()
{
    assert_normpath_result(".//.", ".");
}

void test_normpath_collapse_dotdot_1()
{
    assert_normpath_result("/test/abc/../cdb", "/test/cdb");
}

void test_normpath_collapse_dotdot_2()
{
    assert_normpath_result("/test/..", "/");
}

void test_normpath_collapse_dotdot_3()
{
    assert_normpath_result("/test/test/../..", "/");
}
void test_normpath_collapse_dotdot_4()
{
    assert_normpath_result("test/abc/../..", ".");
}

void test_normpath_collapse_dotdot_5()
{
    assert_normpath_result("/..", "/");
}
void test_normpath_collapse_dotdot_6()
{
    assert_normpath_result("/../..", "/");
}

void test_normpath_collapse_dotdot_7()
{
    assert_normpath_result("/test/../..", "/");
}

void test_normpath_collapse_dotdot_8()
{
    assert_normpath_result("test/../..", "..");
}

void test_normpath_collapse_dotdot_9()
{
    assert_normpath_result("/../...", "/...");
}

void test_normpath_collapse_mix_1()
{
    assert_normpath_result("/path//.//test.txt//.//..///", "/path");
}

void test_normpath_empty_1()
{
    assert_normpath_result("", "");
}

//--------------------------------------------------------------------------------
// Test joinpath and isasb functions
//--------------------------------------------------------------------------------
void assert_isabs_result(const char *path, bool expected_res)
{
    bool res = isabs(path);
    TEST_ASSERT_EQUAL(expected_res, res);
}

void test_isabs_1()
{
    assert_isabs_result("/some/path", true);
}

void test_isabs_2()
{
    assert_isabs_result("some/path", false);
}

void test_isabs_3()
{
    assert_isabs_result("../some/path", false);
}

void test_isabs_4()
{
    assert_isabs_result("", false);
}

void test_isabs_5()
{
    assert_isabs_result("/", true);
}

void assert_join_paths_res(const char *path_l, const char *path_r, const char *expected_path)
{
    char buff[128];
    int ret_code = join_paths(buff, path_l, path_r);
    TEST_ASSERT_EQUAL(ret_code, 0);
    TEST_ASSERT_EQUAL_STRING(expected_path, buff);
}

void test_join_paths_1()
{
    assert_join_paths_res("/some/dir", "abc/1", "/some/dir/abc/1");
}

void test_join_paths_2()
{
    assert_join_paths_res("/some/dir/", "abc/1", "/some/dir/abc/1");
}

void test_join_paths_3()
{
    assert_join_paths_res("/some/dir", "/abc/1", "/abc/1");
}

void test_join_paths_4()
{
    assert_join_paths_res("", "abc/1", "abc/1");
}

void assert_join_paths_n_res(const char *path_l, const char *path_r, size_t buff_len, const char *expected_path = NULL, bool ret_error = false)
{
    char buff[128];
    int ret_code = join_paths(buff, buff_len, path_l, path_r);
    if (ret_error != 0) {
        TEST_ASSERT_NOT_EQUAL(ret_code, 0);
    } else {
        TEST_ASSERT_EQUAL_STRING(expected_path, buff);
    }
}

void test_join_paths_n_1()
{
    assert_join_paths_n_res("/some/dir", "abc/1", 16, "/some/dir/abc/1", false);
}

void test_join_paths_n_2()
{
    assert_join_paths_n_res("/some/dir", "abc/1", 15, "/some/dir/abc/1", true);
}

void test_join_paths_n_3()
{
    assert_join_paths_n_res("/some/dir/", "abc/1", 16, "/some/dir/abc/1", false);
}

void test_join_paths_n_4()
{
    assert_join_paths_n_res("/some/dir/", "abc/1", 15, "/some/dir/abc/1", true);
}

void test_join_paths_n_5()
{
    assert_join_paths_n_res("/some/dir", "/abc/1", 7, "/abc/1", false);
}

void test_join_paths_n_6()
{
    assert_join_paths_n_res("/some/dir", "/abc/1", 6, "/abc/1", true);
}

void test_join_paths_n_7()
{
    assert_join_paths_n_res("", "abc/1", 6, "abc/1", false);
}

void test_join_paths_n_8()
{
    assert_join_paths_n_res("", "abc/1", 5, "abc/1", true);
}

void assert_append_paths_res(const char *path, const char *append, const char *expected_path)
{
    char buff[128];
    strcpy(buff, path);
    int ret_code = append_path(buff, append);
    TEST_ASSERT_EQUAL(ret_code, 0);
    TEST_ASSERT_EQUAL_STRING(expected_path, buff);
}

void test_append_paths_1()
{
    assert_append_paths_res("/some/dir", "abc/1", "/some/dir/abc/1");
}

void test_append_paths_2()
{
    assert_append_paths_res("/some/dir/", "abc/1", "/some/dir/abc/1");
}

void test_append_paths_3()
{
    assert_append_paths_res("/some/dir", "/abc/1", "/abc/1");
}

void test_append_paths_4()
{
    assert_append_paths_res("", "abc/1", "abc/1");
}

void assert_append_paths_n_res(const char *path, const char *append, size_t buff_len, const char *expected_path = NULL, bool ret_error = false)
{
    char buff[128];
    strcpy(buff, path);
    int ret_code = append_path(buff, buff_len, append);
    if (ret_error) {
        TEST_ASSERT_NOT_EQUAL(ret_code, 0);
    } else {
        TEST_ASSERT_EQUAL_STRING(expected_path, buff);
    }
}

void test_append_paths_n_1()
{
    assert_append_paths_n_res("/some/dir", "abc/1", 16, "/some/dir/abc/1", false);
}

void test_append_paths_n_2()
{
    assert_append_paths_n_res("/some/dir", "abc/1", 15, "/some/dir/abc/1", true);
}

void test_append_paths_n_3()
{
    assert_append_paths_n_res("/some/dir/", "abc/1", 16, "/some/dir/abc/1", false);
}

void test_append_paths_n_4()
{
    assert_append_paths_n_res("/some/dir/", "abc/1", 15, "/some/dir/abc/1", true);
}

void test_append_paths_n_5()
{
    assert_append_paths_n_res("/some/dir", "/abc/1", 7, "/abc/1", false);
}

void test_append_paths_n_6()
{
    assert_append_paths_n_res("/some/dir", "/abc/1", 6, "/abc/1", true);
}

void test_append_paths_n_7()
{
    assert_append_paths_n_res("", "abc/1", 6, "abc/1", false);
}

void test_append_paths_n_8()
{
    assert_append_paths_n_res("", "abc/1", 5, "abc/1", true);
}

//--------------------------------------------------------------------------------
// Test basename and dirname functions
//--------------------------------------------------------------------------------
void assert_basename_res(const char *expected, const char *path, bool ret_error = false)
{
    char name[32];
    int err = basename(name, path);
    if (ret_error) {
        TEST_ASSERT_NOT_EQUAL(0, err);
    } else {
        TEST_ASSERT_EQUAL(0, err);
        TEST_ASSERT_EQUAL_STRING(expected, name);
    }
}

void test_basename_1()
{
    assert_basename_res("file.txt", "/sd/storage/file.txt");
}

void test_basename_2()
{
    assert_basename_res("", "/sd/storage/file.txt/");
}

void test_basename_3()
{
    assert_basename_res("storage", "/sd/storage");
}

void test_basename_4()
{
    assert_basename_res("", "/sd/storage/");
}

void test_basename_5()
{
    assert_basename_res("", "/");
}

void test_basename_6()
{
    assert_basename_res("", "");
}

void assert_basename_n_res(const char *expected, const char *path, size_t n, bool ret_error = false)
{
    char name[32];
    int err = basename(name, n, path);
    if (ret_error) {
        TEST_ASSERT_NOT_EQUAL(0, err);
    } else {
        TEST_ASSERT_EQUAL(0, err);
        TEST_ASSERT_EQUAL_STRING(expected, name);
    }
}

void test_basename_n_1()
{
    assert_basename_n_res("file.txt", "/sd/storage/file.txt", 9, false);
}

void test_basename_n_2()
{
    assert_basename_n_res("file.txt", "/sd/storage/file.txt", 8, false);
}

void test_basename_n_3()
{
    assert_basename_n_res("file.txt", "/sd/storage/file.txt", 7, true);
}

void assert_dirname_same_res(const char *expected, const char *path, bool ret_error = false)
{
    char tmp_path[64];
    strcpy(tmp_path, path);
    int err = dirname(tmp_path);
    if (ret_error) {
        TEST_ASSERT_NOT_EQUAL(0, err);
    } else {
        TEST_ASSERT_EQUAL(0, err);
        TEST_ASSERT_EQUAL_STRING(expected, tmp_path);
    }
}

void test_dirname_same_1()
{
    assert_dirname_same_res("/sd/storage", "/sd/storage/file.txt");
}

void test_dirname_same_2()
{
    assert_dirname_same_res("/sd/storage", "/sd/storage/");
}

void test_dirname_same_3()
{
    assert_dirname_same_res("/", "/some_dir");
}

void test_dirname_same_4()
{
    assert_dirname_same_res("", "some_dir");
}

void test_dirname_same_5()
{
    assert_dirname_same_res("/", "/");
}

void test_dirname_same_6()
{
    assert_dirname_same_res("/fs", "/fs///some_dir");
}

void assert_dirname_res(const char *expected, const char *path, bool ret_error = false)
{
    char tmp_path[64];
    int err = dirname(tmp_path, path);
    if (ret_error) {
        TEST_ASSERT_NOT_EQUAL(0, err);
    } else {
        TEST_ASSERT_EQUAL(0, err);
        TEST_ASSERT_EQUAL_STRING(expected, tmp_path);
    }
}

void test_dirname_1()
{
    assert_dirname_res("/sd/storage", "/sd/storage/file.txt");
}

void test_dirname_2()
{
    assert_dirname_res("/sd/storage", "/sd/storage/");
}

void test_dirname_3()
{
    assert_dirname_res("/", "/some_dir");
}

void test_dirname_4()
{
    assert_dirname_res("", "some_dir");
}

void test_dirname_5()
{
    assert_dirname_res("/", "/");
}

void test_dirname_6()
{
    assert_dirname_res("/fs", "/fs///some_dir");
}

void assert_dirname_n_res(const char *expected, const char *path, size_t n, bool ret_error = false)
{
    char tmp_path[64];
    int err = dirname(tmp_path, n, path);
    if (ret_error) {
        TEST_ASSERT_NOT_EQUAL(0, err);
    } else {
        TEST_ASSERT_EQUAL(0, err);
        TEST_ASSERT_EQUAL_STRING(expected, tmp_path);
    }
}

void test_dirname_n_1()
{
    assert_dirname_n_res("/sd/storage", "/sd/storage/file.txt", 12, false);
}

void test_dirname_n_2()
{
    assert_dirname_n_res("/sd/storage", "/sd/storage/file.txt", 11, false);
}

void test_dirname_n_3()
{
    assert_dirname_n_res("/sd/storage", "/sd/storage/file.txt", 10, true);
}

//--------------------------------------------------------------------------------
// Test readdir assistant functions
//--------------------------------------------------------------------------------
void assert_is_child_dirent_name(bool expected, const char *name)
{
    bool res = is_child_dirent(name);
    if (expected) {
        TEST_ASSERT_TRUE(res);
    } else {
        TEST_ASSERT_FALSE(res);
    }
}

void assert_is_child_dirent_dirent(bool expected, const char *name)
{
    struct dirent dir_ent;
    strcpy(dir_ent.d_name, name);
    dir_ent.d_type = DT_UNKNOWN;

    bool res = is_child_dirent(&dir_ent);
    if (expected) {
        TEST_ASSERT_TRUE(res);
    } else {
        TEST_ASSERT_FALSE(res);
    }
}

void test_is_child_dirent_1()
{
    assert_is_child_dirent_name(true, "ABC");
    assert_is_child_dirent_dirent(true, "ABC");
    assert_is_child_dirent_name(true, "test.txt");
    assert_is_child_dirent_dirent(true, "test.txt");
    assert_is_child_dirent_name(true, ".env");
    assert_is_child_dirent_dirent(true, ".env");
    assert_is_child_dirent_name(true, "12");
    assert_is_child_dirent_dirent(true, "12");
    assert_is_child_dirent_name(true, "?");
    assert_is_child_dirent_dirent(true, "?");
    assert_is_child_dirent_name(true, "...");
    assert_is_child_dirent_dirent(true, "...");
    assert_is_child_dirent_name(true, "");
    assert_is_child_dirent_dirent(true, "");
}

void test_is_child_dirent_2()
{
    assert_is_child_dirent_name(false, ".");
    assert_is_child_dirent_dirent(false, ".");
    assert_is_child_dirent_name(false, "..");
    assert_is_child_dirent_dirent(false, "..");
}

// test cases description
#define SimpleCase(test_fun) Case(#test_fun, test_fun, greentea_case_failure_continue_handler)
Case cases[] = {
    SimpleCase(test_normpath_simple_1),
    SimpleCase(test_normpath_collapse_dot_1),
    SimpleCase(test_normpath_collapse_dot_2),
    SimpleCase(test_normpath_collapse_sep_1),
    SimpleCase(test_normpath_collapse_sep_2),
    SimpleCase(test_normpath_collapse_dotdot_1),
    SimpleCase(test_normpath_collapse_dotdot_2),
    SimpleCase(test_normpath_collapse_dotdot_3),
    SimpleCase(test_normpath_collapse_dotdot_4),
    SimpleCase(test_normpath_collapse_dotdot_5),
    SimpleCase(test_normpath_collapse_dotdot_6),
    SimpleCase(test_normpath_collapse_dotdot_7),
    SimpleCase(test_normpath_collapse_dotdot_8),
    SimpleCase(test_normpath_collapse_dotdot_9),
    SimpleCase(test_normpath_collapse_dot_sep_1),
    SimpleCase(test_normpath_collapse_dot_sep_2),
    SimpleCase(test_normpath_collapse_dot_sep_3),
    SimpleCase(test_normpath_collapse_dot_sep_4),
    SimpleCase(test_normpath_collapse_dot_sep_5),
    SimpleCase(test_normpath_collapse_mix_1),
    SimpleCase(test_normpath_empty_1),

    SimpleCase(test_isabs_1),
    SimpleCase(test_isabs_2),
    SimpleCase(test_isabs_3),
    SimpleCase(test_isabs_4),
    SimpleCase(test_isabs_5),
    SimpleCase(test_join_paths_1),
    SimpleCase(test_join_paths_2),
    SimpleCase(test_join_paths_3),
    SimpleCase(test_join_paths_4),
    SimpleCase(test_join_paths_n_1),
    SimpleCase(test_join_paths_n_2),
    SimpleCase(test_join_paths_n_3),
    SimpleCase(test_join_paths_n_4),
    SimpleCase(test_join_paths_n_5),
    SimpleCase(test_join_paths_n_6),
    SimpleCase(test_join_paths_n_7),
    SimpleCase(test_join_paths_n_8),
    SimpleCase(test_append_paths_1),
    SimpleCase(test_append_paths_2),
    SimpleCase(test_append_paths_3),
    SimpleCase(test_append_paths_4),
    SimpleCase(test_append_paths_n_1),
    SimpleCase(test_append_paths_n_2),
    SimpleCase(test_append_paths_n_3),
    SimpleCase(test_append_paths_n_4),
    SimpleCase(test_append_paths_n_5),
    SimpleCase(test_append_paths_n_6),
    SimpleCase(test_append_paths_n_7),
    SimpleCase(test_append_paths_n_8),

    SimpleCase(test_basename_1),
    SimpleCase(test_basename_2),
    SimpleCase(test_basename_3),
    SimpleCase(test_basename_4),
    SimpleCase(test_basename_5),
    SimpleCase(test_basename_6),
    SimpleCase(test_basename_n_1),
    SimpleCase(test_basename_n_2),
    SimpleCase(test_basename_n_3),
    SimpleCase(test_dirname_same_1),
    SimpleCase(test_dirname_same_2),
    SimpleCase(test_dirname_same_3),
    SimpleCase(test_dirname_same_4),
    SimpleCase(test_dirname_same_5),
    SimpleCase(test_dirname_same_6),
    SimpleCase(test_dirname_1),
    SimpleCase(test_dirname_2),
    SimpleCase(test_dirname_3),
    SimpleCase(test_dirname_4),
    SimpleCase(test_dirname_5),
    SimpleCase(test_dirname_6),
    SimpleCase(test_dirname_n_1),
    SimpleCase(test_dirname_n_2),
    SimpleCase(test_dirname_n_3),

    SimpleCase(test_is_child_dirent_1),
    SimpleCase(test_is_child_dirent_2),
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
