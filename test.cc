#include <bits/stdc++.h>
using namespace std;

int ball1[5], ball2[5], ball3[5];
int a[1000005], b[1000005], g[1000005];
int cnt1 = 0, cnt2 = 0, cnt3 = 0;

int main()
{
    int n;
    cin >> n;
    for (int i = 1; i <= n; i++)
    {
        cin >> a[i] >> b[i] >> g[i];
    }

    ball1[1] = 1; // 初始化放在循环外
    for (int i = 1; i <= n; i++)
    {
        swap(ball1[a[i]], ball1[b[i]]);
        if (ball1[g[i]] == 1)
        {
            cnt1++;
        }
    }

    ball2[2] = 1; // 初始化放在循环外！
    for (int i = 1; i <= n; i++)
    {
        swap(ball2[a[i]], ball2[b[i]]);
        if (ball2[g[i]] == 1)
        {
            cnt2++;
        }
    }

    ball3[3] = 1; // 初始化放在循环外！
    for (int i = 1; i <= n; i++)
    {
        swap(ball3[a[i]], ball3[b[i]]);
        if (ball3[g[i]] == 1)
        {
            cnt3++;
        }
    }

    cout << max({cnt1, cnt2, cnt3});

    return 0;
}
