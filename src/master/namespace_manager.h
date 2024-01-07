#ifndef NAMESPACE_MANAGER_H_
#define NAMESPACE_MANAGER_H_

#include <cstdint>
#include <map>
#include <memory>
#include <shared_mutex>
#include <string>
#include <vector>

#include "state_code.h"

namespace gfs {

class FileTreeNode {
 public:
  FileTreeNode();
  ~FileTreeNode() = default;

  bool IsDir;
  // std::map<std::string, std::shared_ptr<FileTreeNode>> children;

  std::map<std::string, std::shared_ptr<FileTreeNode>> children;
  uint64_t length;
  uint64_t chunks;

  void lock() {  // lock_.lock();
    shared_lock_.lock();
  }

  void wlock() { lock_.lock(); }

  void unlock() { lock_.unlock(); }
  std::shared_mutex lock_;
  std::shared_lock<std::shared_mutex> shared_lock_{lock_};
};

class Namespace_Manager {
 private:
  std::shared_ptr<FileTreeNode> root;
  int serialCt;

 public:
  Namespace_Manager();
  ~Namespace_Manager();

  void split_path(const std::string& path, std::vector<std::string>* all_paths);

  status_code lockparent(const std::vector<std::string>& path,
                         std::shared_ptr<FileTreeNode>& father);

  status_code unlockparent(const std::vector<std::string>& path);

  status_code createdir(const std::string& path);

  status_code deletedir(const std::string& path);

  status_code createfile(const std::string& path);

  status_code deletefile(const std::string& path);
};

}  // namespace gfs

#endif