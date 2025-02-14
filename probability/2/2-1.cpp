#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

#define N 1000

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

int main() {
  const auto rnd = rnd_exp(0.5, N);
  std::ofstream file("2-1.csv");

  file << "n,rnd" << std::endl;
  for (int i = 0; i < N; ++i) {
    file << i + 1;
    file << "," << rnd[i];
    file << std::endl;
  }
}
