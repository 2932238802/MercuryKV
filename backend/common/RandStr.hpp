
#pragma once
#include <algorithm>
#include <cstddef>
#include <random>
#include <string>
#include <string_view>

namespace common
{

/**
 * @brief 生成指定 number长度的随机数字 字母串
 *
 * @param number
 * @return std::string
 */
inline std::string RandStr(size_t number)
{
    // 产生的随机数
    std::string_view const_char = "0123456789"
                                  "abcdefghijklmnopqrstuvwxyz"
                                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static std::mt19937 gen = []
    {
        std::random_device rd;
        return std::mt19937(rd());
    }();

    static std::uniform_int_distribution<size_t> dist(0, const_char.length() - 1);

    std::string result(number, '\0');
    // 多次调用这个函数
    std::generate_n(result.begin(), number, [&]() { return const_char[dist(gen)]; });

    return result;
}

} // namespace common