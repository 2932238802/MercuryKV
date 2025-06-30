
#include <iostream>
#include <string>
using namespace std;
string addBinary(string a, string b) {

  // flag = (a[i] - 48) & (b[i]-48);
  // cur = (a[i] - 48) ^ (b[i]-48);

  int a_size = a.size();
  int b_size = b.size();
  string max_str = a.size() > b.size() ? a : b;
  int min = a.size() > b.size() ? a.size() : b.size();
  int max = a.size() + b.size() - min;
  string answer = "";

  int i = 0;
  int flag = 0;
  int cur = 0;
  while (i < min) {
    answer += (static_cast<char>((a[i] - 48) ^ (b[i] - 48)) + flag);
    flag = (a[i] - 48) & (b[i] - 48);
    i++;
  }

  while (i < max) {
    if (i == min) {
      answer += (max_str[i] + flag);
    }
    answer += max_str[i];
    i++;
  }
  return answer;
}

int main() {
  string a = "1010";

  string b = "1011";
  cout << addBinary(a, b);
}