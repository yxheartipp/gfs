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

}  // namespace gfs
