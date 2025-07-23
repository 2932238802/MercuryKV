#include <bits/stdc++.h>
using namespace std;

long long l, r, k;
long long ans = 0;

vector<long long> primes;
bool is_composite[1000005];
bool is_not_kprime[10000005];

void sieve(long long n)
{
    is_composite[0] = is_composite[1] = true;
    for (long long i = 2; i * i <= n; i++)
    {
        if (!is_composite[i])
        {
            for (long long j = i * i; j <= n; j += i)
            {
                is_composite[j] = true;
            }
        }
    }
    for (long long i = 2; i <= n; i++)
    {
        if (!is_composite[i])
        {
            primes.push_back(i);
        }
    }
}

long long xor_sum(long long n) // 这个细节处理一下
{
    if (n < 0)
        return 0;
    long long rem = n % 4;
    if (rem == 0)
        return n;
    if (rem == 1)
        return 1;
    if (rem == 2)
        return n + 1;
    return 0; // rem == 3
}

signed main()
{
    cin >> l >> r >> k;
    if (k == 1)
    {
        long long res = xor_sum(r) ^ xor_sum(l - 1);
        if (l == 1)
        {
            res ^= 1;
        }
        cout << res << endl;
        return 0;
    }

    sieve(1000005 - 1);

    // 区间筛法
    for (long long p : primes)
    {
        // 只用 <= k 的素数去筛 这样会快很多
        if (p > k)
        {
            break;
        }
        // 找到 p 在 [l, r] 中的第一个倍数
        long long start = (l + p - 1) / p * p;
        for (long long j = start; j <= r; j += p)
        {
            if (j >= l)
            {
                is_not_kprime[j - l] = true; // 将 p 的倍数标记为“不是荤数” 这样下次 直接跳过
            }
        }
    }
    // 上一步会把素数 p 自己也标记掉，这里要修正回来 对
    for (long long p : primes)
    {
        if (p > k)
            break;
        if (p >= l && p <= r)
        {
            is_not_kprime[p - l] = false;
        }
    }

    // 这个是 最后的 异或和
    for (long long i = l; i <= r; i++)
    {
        if (i == 1)
            continue; // 1 不是荤数
        if (!is_not_kprime[i - l])
        {
            ans ^= i;
        }
    }
    cout << ans << endl;
    return 0;
}
