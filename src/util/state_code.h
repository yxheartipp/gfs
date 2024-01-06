#include <cstdint>
#include <string>
#include <vector>

#include "iostream"
#include "string.h"

namespace gfs {

// struct status_code {
//   int value;
//   std::string exception;
//   status_code() {
//     value = 0;
//     exception = "ok";
//   }
// };

enum status_code {
  SUCCESS,
  PATH_NOT_FOUND,
  PATH_ALREADY_EXIST,
  FILE_ALREADY_EXIST,
};

}  // namespace gfs