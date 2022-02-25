#include <fstream>
#include <string>

#include "nlohmann/json.hpp"

namespace jackson {

void parse(std::string const &fileName, nlohmann::json &json);
}