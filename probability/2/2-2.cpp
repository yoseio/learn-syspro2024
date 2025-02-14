#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

#define N 10000

std::vector<double> rnd_exp(const double λ, const int n) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dist(0.0, 1.0);

  std::vector<double> rnd(n);
  std::generate(rnd.begin(), rnd.end(), [&]() {
    const double u = dist(gen);
    return -1 / λ * std::log(1 - u);
  });
  return rnd;
}

double mean(const std::vector<double>& v) {
  return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}

double dist(const std::vector<double>& v) {
  const double m = mean(v);
  return std::accumulate(
             v.begin(), v.end(), 0.0,
             [&](double acc, double x) { return acc + std::pow(x - m, 2); }) /
         v.size();
}

int main() {
  std::cout << std::fixed;

  const auto rnd1 = rnd_exp(1.0, N);
  std::cout << "λ: " << 1.0 << ", mean: " << mean(rnd1)
            << ", dist: " << dist(rnd1) << std::endl;

  const auto rnd2 = rnd_exp(1.5, N);
  std::cout << "λ: " << 1.5 << ", mean: " << mean(rnd2)
            << ", dist: " << dist(rnd2) << std::endl;

  const auto rnd3 = rnd_exp(2.0, N);
  std::cout << "λ: " << 2.0 << ", mean: " << mean(rnd3)
            << ", dist: " << dist(rnd3) << std::endl;

  std::ofstream file("2-2.csv");
  file << "n,1.0,1.5,2.0" << std::endl;
  for (int i = 0; i < N; ++i) {
    file << i + 1;
    file << "," << rnd1[i];
    file << "," << rnd2[i];
    file << "," << rnd3[i];
    file << std::endl;
  }
}
