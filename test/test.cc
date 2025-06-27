#include <iostream>
#include <string>
using namespace std;

class MyString {
public:
  MyString() { std::cout << "MyString() 默认构造" << std::endl; }

  MyString(string &str) { std::cout << "MyString(string &str)" << std::endl; }

  MyString(const string &str) {
    std::cout << "MyString(string &str)" << std::endl;
  }

  MyString(string &&str) { std::cout << "MyString(string &&str)" << std::endl; }

  MyString(const MyString &str) {
    std::cout << "MyString(const string &str)" << std::endl;
  }

  MyString(MyString &&str) {
    std::cout << "MyString(MyString &&str)" << std::endl;
  }

  void operator=(MyString &&str) {
    std::cout << "operator=(MyString &&str)" << std::endl;
  }

  void operator=(const MyString &str) {
    std::cout << "operator=(const MyString &str)" << std::endl;
  }

  void operator=(const string &str) {
    std::cout << "operator=(const MyString &str)" << std::endl;
  }
  void operator=(string &&str) {
    std::cout << "void operator=(string &&str)" << std::endl;
  }
  void operator=(string &str) {
    std::cout << "void operator=(string &str)" << std::endl;
  }
  //   void operator=(const char *str) {
  //     std::cout << "void operator=(const char *str)" << std::endl;
  //   }

  ~MyString() { std::cout << "~MyString() 销毁调用" << std::endl; }
};

int main() {

  // 这个str 是左值
  MyString str_1("for the test");
  MyString str_2(str_1);

  //   MyString str_3 = "111"; 需要两次用户定义的转换 (const char* ->
  //   std::string，然后 std::string -> MyString)

  // operator= 是赋值运算符，
  // 它只能用于已经存在的对象。而 MyString str_3="111";
  // 是在声明一个新对象，这个过程叫做初始化
  // (Initialization)，它只会调用构造函数 (Constructor)
  //   MyString str_3 = "111";
  str_2 = "123m";

  return 0;
}