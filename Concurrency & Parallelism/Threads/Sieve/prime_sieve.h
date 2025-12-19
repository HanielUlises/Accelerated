#pragma once
#include <vector>
#include <thread>
#include <cmath>
#include <algorithm>

std::vector<long long> parallel_sieve(long long n, int num_threads = 0);