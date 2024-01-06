#include <string>

#include "gtest/gtest.h"
#include "namespace_manager.h"

namespace gfs {
TEST(Namespace_ManagerTest, Empty) { Namespace_Manager namespace_manager; }

TEST(Namespace_ManagerTest, Split_path) {
  std::string tmp = "/src/root/link";
  std::vector<std::string> ans;
  Namespace_Manager nm;
  nm.split_path(tmp, &ans);
  ASSERT_EQ("src", ans[0]);
  ASSERT_EQ("root", ans[1]);
  ASSERT_EQ("link", ans[2]);
}

TEST(Namespace_ManagerTest, CreateFile) {
  // Namespace_Manager nm;
  // std::string input_path = "/dd/file.txt";

  // status_code s = nm.createfile(input_path);
  // ASSERT_EQ(s.value, -1);
  // ASSERT_EQ(s.exception, "don't exist path");

  // input_path = "/file.txt";
  // s = nm.createfile(input_path);
  // ASSERT_EQ(0, s.value);
  // ASSERT_EQ(s.exception, "create file success");
  Namespace_Manager nm;
  std::string input_path = "/file.txt";
  auto s = nm.createfile(input_path);
  ASSERT_EQ(s, SUCCESS);
  s = nm.createfile(input_path);
  ASSERT_EQ(s, FILE_ALREADY_EXIST);
  input_path = "/dd/file.txt";
  s = nm.createfile(input_path);
  ASSERT_EQ(s, PATH_NOT_FOUND);
}

// TEST(Namespace_ManagerTest, CreateDir) {
//   Namespace_Manager nm;
//   std::string input_dir = "/src/test/";
//   status_code s = nm.createdir(input_dir);
//   ASSERT_EQ(s.value, 0);
//   s = nm.createdir(input_dir);
//   ASSERT_EQ(s.value, -1);
//   std::string input_file_path = "/src/test/file.txt";
//   s = nm.createfile(input_file_path);
//   ASSERT_EQ(s.value, 0);
// }

}  // namespace gfs
