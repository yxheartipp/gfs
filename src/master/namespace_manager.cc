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

FileTreeNode::FileTreeNode() { children = {}; }

Namespace_Manager::Namespace_Manager()
    : root(std::make_shared<FileTreeNode>()), serialCt(0) {
  root->IsDir = true;
  root->chunks = 0;
}

Namespace_Manager::~Namespace_Manager() {}

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

status_code Namespace_Manager::lockparent(const std::vector<std::string>& path,
                                          FileTreeNode* father) {
  std::shared_lock<std::shared_mutex> lock(root->lock_);
  FileTreeNode* current = root.get();

  for (const auto& dir : path) {
    auto it = current->children.find(dir);
    if (it == current->children.end()) {
      std::cerr << "Error: Path not found." << std::endl;
      return PATH_NOT_FOUND;
    }
    current = it->second.get();
    current->lock();
    father = current;
  }
  return SUCCESS;
  // auto tmp_node = root;
  // for (auto name : all_paths) {
  //   auto node = tmp_node->children.find(name);
  //   if (node != tmp_node->children.end()) {
  //     tmp_node = node->second;
  //     tmp_node->lock();
  //     if (name == all_paths.back()) {
  //       father = tmp_node;
  //     }
  //   } else {
  //     s.value = -1;
  //     s.exception = "don't exist path";
  //     return s;
  //   }
  // }
  // s.exception = "lock successed\n"
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
  // status_code s;
  // std::string file_name;
  // std::vector<std::string> dir_path;
  // FileTreeNode* father_node;
  // FileTreeNode* new_file_node;
  // split_path(path, &dir_path);
  // file_name = dir_path.back();
  // dir_path.pop_back();
  // s = lockparent(path, father_node);
  // if (s.value != 0 && dir_path.size() != 0) {
  //   return s;
  // }
  // if (dir_path.size() == 0) {
  //   father_node = root;
  // }
  // if (father_node->children.find(file_name) != father_node->children.end()) {
  //   s.value = -1;
  //   s.exception = "file all ready exist";
  //   return s;
  // }
  // new_file_node = new FileTreeNode();
  // new_file_node->IsDir = false;
  // father_node->children.insert(std::make_pair(file_name, new_file_node));
  // s = unlockparent(path);
  // if (s.value != 0) {
  //   return s;
  // }
  // s.exception = "create file success";
  // return s;
  std::vector<std::string> dir_path;
  split_path(path, &dir_path);
  std::string file_name = dir_path.back();
  dir_path.pop_back();
  std::shared_ptr<FileTreeNode> father_node;
  if (dir_path.size() != 0) {
    auto s = lockparent(dir_path, father_node.get());
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

// status_code Namespace_Manager::createdir(const std::string& path) {
//   status_code s;
//   std::vector<std::string> dir_path;
//   split_path(path, &dir_path);
//   FileTreeNode* father_node;
//   // s = lockparent(path, father_node);
//   if (s.exception != "don't exist path") {
//     return s;
//   }
//   if (father_node == nullptr) {
//     father_node = root;
//   }
//   for (const auto p : dir_path) {
//     if (father_node->children.find(p) != father_node->children.end()) {
//       if (p == dir_path.back()) {
//         s.value = -1;
//         s.exception = "alreday exist";
//         return s;
//       }
//       continue;
//     }
//     FileTreeNode* new_dir_node = new FileTreeNode();
//     new_dir_node->IsDir = true;
//     father_node->children.insert(std::make_pair(p, new_dir_node));
//   }
//   // s = unlockparent(path);
//   return s;
// }
}  // namespace gfs