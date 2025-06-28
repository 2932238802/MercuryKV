// // #include <iostream>
// // #include <string>
// // using namespace std;

// // class MyString {
// // public:
// //   MyString() { std::cout << "MyString() 默认构造" << std::endl; }

// //   MyString(string &str) { std::cout << "MyString(string &str)" <<
// std::endl;
// //   }

// //   MyString(const string &str) {
// //     std::cout << "MyString(string &str)" << std::endl;
// //   }

// //   MyString(string &&str) { std::cout << "MyString(string &&str)" <<
// //   std::endl; }

// //   MyString(const MyString &str) {
// //     std::cout << "MyString(const string &str)" << std::endl;
// //   }

// //   MyString(MyString &&str) {
// //     std::cout << "MyString(MyString &&str)" << std::endl;
// //   }

// //   void operator=(MyString &&str) {
// //     std::cout << "operator=(MyString &&str)" << std::endl;
// //   }

// //   void operator=(const MyString &str) {
// //     std::cout << "operator=(const MyString &str)" << std::endl;
// //   }

// //   void operator=(const string &str) {
// //     std::cout << "operator=(const MyString &str)" << std::endl;
// //   }
// //   void operator=(string &&str) {
// //     std::cout << "void operator=(string &&str)" << std::endl;
// //   }
// //   void operator=(string &str) {
// //     std::cout << "void operator=(string &str)" << std::endl;
// //   }
// //   //   void operator=(const char *str) {
// //   //     std::cout << "void operator=(const char *str)" << std::endl;
// //   //   }

// //   ~MyString() { std::cout << "~MyString() 销毁调用" << std::endl; }
// // };

// // int main() {

// //   // 这个str 是左值
// //   MyString str_1("for the test");
// //   MyString str_2(str_1);

// //   //   MyString str_3 = "111"; 需要两次用户定义的转换 (const char* ->
// //   //   std::string，然后 std::string -> MyString)

// //   // operator= 是赋值运算符，
// //   // 它只能用于已经存在的对象。而 MyString str_3="111";
// //   // 是在声明一个新对象，这个过程叫做初始化
// //   // (Initialization)，它只会调用构造函数 (Constructor)
// //   //   MyString str_3 = "111";
// //   str_2 = "123m";

// //   return 0;
// // }

// #include <iomanip>
// #include <openssl/evp.h>
// #include <sstream>
// #include <stdexcept>

// std::string sha256(const std::string &input) {
//   // 1. 创建并初始化哈希上下文
//   EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
//   if (mdctx == nullptr) {
//     throw std::runtime_error("Failed to create EVP_MD_CTX");
//   }

//   // 2. 指定哈希算法 (这里是 SHA-256)
//   const EVP_MD *md = EVP_sha256();
//   if (EVP_DigestInit_ex(mdctx, md, NULL) != 1) {
//     EVP_MD_CTX_free(mdctx);
//     throw std::runtime_error("Failed to initialize digest");
//   }

//   // 3. 喂入要哈希的数据
//   if (EVP_DigestUpdate(mdctx, input.c_str(), input.length()) != 1) {
//     EVP_MD_CTX_free(mdctx);
//     throw std::runtime_error("Failed to update digest");
//   }

//   // 4. 获取哈希结果 (二进制)
//   unsigned char hash[EVP_MAX_MD_SIZE];
//   unsigned int hash_len;
//   if (EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1) {
//     EVP_MD_CTX_free(mdctx);
//     throw std::runtime_error("Failed to finalize digest");
//   }

//   // 5. 释放上下文
//   EVP_MD_CTX_free(mdctx);

//   // 6. 将二进制哈希值转换为十六进制字符串
//   std::stringstream ss;
//   for (unsigned int i = 0; i < hash_len; ++i) {
//     ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
//   }

//   return ss.str();
// }

#include <bits/stdc++.h>
using namespace std;
string a, b, temp;
int cmp(string a, string b) {
  int len1 = a.size();
  int len2 = b.size();

  // 左边大 返回1
  if (len1 > len2) {
    return 1;
  } else {
    return 0;
  }
}
int main() {
  cin >> a >> b;
  if (cmp(a, b)) {
    temp = a;
    a = b;
    b = temp;
  }
  // 使得左边小 右边大
  int len = b.size();
  // 正确

  for (int i = 0; i < len; i++) {
    if (b[i] == a[0]) {
      int len2 = a.size();
      int flag = true;
      for (int j = 0; j < len2; j++) {
        if (b[i + j] == a[j]) {

        } else {
          flag = false;
          break;
        }
      }
      if (flag) {
        cout << a << " is substring of" << b;
        return 0;
      }
    }
  }
  cout << "No substring";
  return 0;
}