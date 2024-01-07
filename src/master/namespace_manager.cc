#include "namespace_manager.h"

#include <bits/types/FILE.h>

#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <utility>
#include <vector>

namespace gfs {

FileTreeNode::FileTreeNode() {}

Namespace_Manager::Namespace_Manager()
    : root(std::make_shared<FileTreeNode>()), serialCt(0) {
  root->IsDir = true;
  root->chunks = 0;
}

Namespace_Manager::~Namespace_Manager() {
  if (root) {
    root->unlock();
  }
}

void Namespace_Manager::split_path(const std::string& path,
                                   std::vector<std::string>* all_paths) {
  size_t start = 0;
  size_t end = path.find("/");
  // 单独处理开头为"/"的路径
  if (start == end) {
    start = 1;
    end = path.find("/", start);
  }
  while (end != std::string::npos) {
    all_paths->push_back(path.substr(start, end - start));
    start = end + 1;
    end = path.find("/", start);
  }
  if (start == path.size()) return;
  all_paths->push_back(path.substr(start, end));
}

status_code Namespace_Manager::lockparent(
    const std::vector<std::string>& path,
    std::shared_ptr<FileTreeNode>& father) {
  FileTreeNode* current = root.get();
  std::shared_lock<std::shared_mutex> gurad(current->lock_);
  for (const auto& dir : path) {
    auto it = current->children.find(dir);
    if (it == current->children.end()) {
      std::cerr << "Error: Path not found." << std::endl;
      return PATH_NOT_FOUND;
    }
    current = it->second.get();
    std::shared_lock<std::shared_mutex> lock(current->lock_);
    father.reset(current, [current](FileTreeNode*) {});
  }
  return SUCCESS;
}

status_code Namespace_Manager::unlockparent(
    const std::vector<std::string>& path) {
  FileTreeNode* current = root.get();
  for (const auto& dir : path) {
    auto it = current->children.find(dir);
    if (it == current->children.end()) {
      std::cerr << "Error: Path not found." << std::endl;
      return PATH_NOT_FOUND;
    }
    current = it->second.get();
    current->unlock();
  }
  return SUCCESS;
}

status_code Namespace_Manager::createfile(const std::string& path) {
  std::vector<std::string> dir_path;
  split_path(path, &dir_path);
  std::string file_name = dir_path.back();
  dir_path.pop_back();
  std::shared_ptr<FileTreeNode> father_node = nullptr;
  if (dir_path.size() != 0) {
    auto s = lockparent(dir_path, father_node);
    if (s != SUCCESS) {
      unlockparent(dir_path);
      return s;
    }
  } else {
    father_node = root;
  }
  if (father_node->children.find(file_name) != father_node->children.end()) {
    unlockparent(dir_path);
    return FILE_ALREADY_EXIST;
  }
  std::shared_ptr<FileTreeNode> new_file_node =
      std::make_shared<FileTreeNode>();
  new_file_node->IsDir = false;
  father_node->children.insert(std::make_pair(file_name, new_file_node));
  auto s = unlockparent(dir_path);
  return s;
}

status_code Namespace_Manager::createdir(const std::string& path) {
  std::vector<std::string> dir_path;
  split_path(path, &dir_path);
  std::shared_ptr<FileTreeNode> father_node = nullptr;
  std::string dir_name;
  dir_name = dir_path.back();
  dir_path.pop_back();
  if (dir_path.size() != 0) {
    auto s = lockparent(dir_path, father_node);
    if (s != SUCCESS) {
      unlockparent(dir_path);
      return s;
    }
  } else {
    father_node = root;
  }
  if (father_node->children.find(dir_name) != father_node->children.end()) {
    unlockparent(dir_path);
    return FILE_ALREADY_EXIST;
  }
  std::shared_ptr<FileTreeNode> new_dir_node = std::make_shared<FileTreeNode>();
  new_dir_node->IsDir = true;
  father_node->children.insert(std::make_pair(dir_name, new_dir_node));
  auto s = unlockparent(dir_path);
  return SUCCESS;
}

status_code Namespace_Manager::deletedir(const std::string& path) {
  std::vector<std::string> dir_path;
  split_path(path, &dir_path);
  std::shared_ptr<FileTreeNode> father_node = nullptr;
  std::string dir_name;
  dir_name = dir_path.back();
  dir_path.pop_back();
  if (dir_path.size() != 0) {
    auto s = lockparent(dir_path, father_node);
    if (s != SUCCESS) {
      unlockparent(dir_path);
      return s;
    }
  } else {
    father_node = root;
  }
  auto it = father_node->children.find(dir_name);
  if (it == father_node->children.end()) {
    unlockparent(dir_path);
    return PATH_NOT_FOUND;
  }
  father_node->children.erase(it);
  auto s = unlockparent(dir_path);
  return s;
}

}  // namespace gfs