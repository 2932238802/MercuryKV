#pragma once

#include <iomanip>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace MyCryptNS {
/**
 * @brief
 *
 * 1. 处理哈希加密
 */

inline std::vector<char> &HexToByte() {}
inline std::string &ByteToHex() {}

class MyCrypt {
  static const void Hash(std::string password) {}
  static const void DeHash(std::string password) {}
};

} // namespace MyCryptNS
