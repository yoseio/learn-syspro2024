#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

#define N 1000

std::vector<double> rnd_exp(const int n) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dist(0.0, 1.0);

  std::vector<double> rnd(n);
  std::generate(rnd.begin(), rnd.end(), [&]() { return dist(gen); });
  return rnd;
}

int main() {
  const auto rnd = rnd_exp(N);

  const auto mean = std::accumulate(rnd.begin(), rnd.end(), 0.0) / N;
  std::cout << "平均:\t" << mean << std::endl;

  const auto var_fn = [&mean](double acc, const double& v) {
    return acc + std::pow(v - mean, 2);
  };
  const auto var = std::accumulate(rnd.begin(), rnd.end(), 0.0, var_fn) / N;
  std::cout << "分散:\t" << var << std::endl;

  return 0;
}
