#include "namespace_manager.h"

#include <cstddef>

namespace gfs {

FileTreeNode::FileTreeNode() { children = {}; }

Namespace_Manager::Namespace_Manager() {
  root = new FileTreeNode();
  root->IsDir = true;
  root->chunks = 0;
}

Namespace_Manager::~Namespace_Manager() {
  while (!root->children.empty()) {
    auto child = root->children.begin();
    delete child->second;
    root->children.erase(child);
  }
  delete root;
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
  all_paths->push_back(path.substr(start, end));
}
}  // namespace gfs