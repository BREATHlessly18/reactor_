#include "json.hh"

#include <iostream>

#include "assert.h"

namespace jackson {

void parse(std::string const &fileName, nlohmann::json &json) {
  try {
    std::ifstream file(fileName);
    file >> json;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
}

}  // namespace jackson