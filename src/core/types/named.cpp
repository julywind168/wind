#include "named.h"

namespace wind {

std::unordered_map<std::string, int> NAMED = {};
std::unordered_map<std::string, std::vector<std::shared_ptr<Type>>> STRUCT = {};

}