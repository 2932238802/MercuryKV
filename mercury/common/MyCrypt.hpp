#pragma once

#include "MyLog.hpp"
#include <cstddef>
#include <iomanip>
#include <ios>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace MyCryptNS
{
/**
 * @brief
 *
 * 1. 处理哈希加密
 */

using namespace MyLogNS;
class MyCrypt
{
    /** --------------------------------------------------
     * @brief 哈希加密
     * @param input 输入的原生字符串 用来哈希加密用的
     */
  public:
    static const std::string Hash(const std::string &input)
    {
        EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
        if (mdctx == nullptr)
        {
            MY_LOG_ERROR("Failed to create EVP_MD_CTX");
            throw std::runtime_error("Failed to create EVP_MD_CTX");
        }

        const EVP_MD *md = EVP_sha256();
        if (EVP_DigestInit_ex(mdctx, md, NULL) != 1)
        {
            EVP_MD_CTX_free(mdctx);
            MY_LOG_ERROR("Failed to initialize digest");
            throw std::runtime_error("Failed to initialize digest");
        }

        if (EVP_DigestUpdate(mdctx, input.c_str(), input.length()) != 1)
        {
            EVP_MD_CTX_free(mdctx);
            MY_LOG_ERROR("Failed to update digest");
            throw std::runtime_error("Failed to update digest");
        }

        unsigned char hash[EVP_MAX_MD_SIZE];
        unsigned int hash_len;
        if (EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1)
        {
            EVP_MD_CTX_free(mdctx);
            MY_LOG_ERROR("Failed to finalize digest");
            throw std::runtime_error("Failed to finalize digest");
        }

        EVP_MD_CTX_free(mdctx);

        std::stringstream ss;
        for (unsigned int i = 0; i < hash_len; ++i)
        {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        MY_LOG_SUC("哈希加密成功!", ss.str());
        return ss.str();
    }
    // --------------------------------------------------

    /** --------------------------------------------------
     * @brief
     *
     * @param length
     * @return const std::string
     */
  public:
    static const std::string GenerateSalt(size_t length = 16)
    {
        // 生成16位 盐
        std::vector<unsigned char> contain(length);

        int flag = RAND_bytes(contain.data(), length);

        if (flag == 1)
        {
            MY_LOG_SUC("系统支持RAND_bytes操作");
        }
        else
        {
            MY_LOG_ERROR("RAND_bytes 内部发生错误或是系统不支持!");
            return std::string{};
        }

        // 通过之后
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (auto a : contain)
        {
            ss << std::setw(2) << static_cast<unsigned int>(a);
        }
        return ss.str();
    }
    // --------------------------------------------------

    /** --------------------------------------------------
     * @brief 验证用户输入的密码是否正确
     * @param plainPassword     用户登录时输入的明文密码
     * @param salt              从盐
     * @param storedHash        从哈希值
     * @return                  bool 密码匹配返回 true 否则返回 false
     */
  public:
    static bool VerifyPassword(const std::string &plain_password, const std::string &salt,
                               const std::string &stored_hash)
    {
        try
        {
            std::string newhash = Hash((plain_password + salt));
            return newhash == stored_hash;
        }
        catch (const std::exception &e)
        {
            MY_LOG_ERROR("由于", e.what(), "解密过程发生错误");
            return false;
        }
    }
    // --------------------------------------------------
};
} // namespace MyCryptNS
