#ifndef NAMESPACE_MANAGER_H_
#define NAMESPACE_MANAGER_H_

#include <cstdint>
#include <map>
#include <shared_mutex>
#include <string>
#include <vector>

namespace gfs {

class FileTreeNode {
 public:
  FileTreeNode();
  ~FileTreeNode() = default;

  bool IsDir;
  std::map<std::string, FileTreeNode*> children;

  uint64_t length;
  uint64_t chunks;

  void lock() { lock_.lock(); }
  void unlock() { lock_.unlock(); }

 private:
  std::shared_mutex lock_;
};

class Namespace_Manager {
 private:
  FileTreeNode* root;
  int serialCt;

 public:
  Namespace_Manager();
  ~Namespace_Manager();

  void split_path(const std::string& path, std::vector<std::string>* all_paths);
};

}  // namespace gfs

#endif