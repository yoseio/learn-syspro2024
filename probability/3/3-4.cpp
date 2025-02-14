#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <vector>

const int N = 1000;
const int M = 10000;

std::vector<double> dist_uniform(const int n, const double max) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dist(0.0, max);

  std::vector<double> rnd(n);
  std::generate(rnd.begin(), rnd.end(), [&]() { return dist(gen); });
  return rnd;
}

std::vector<double> dist_exp(const int n, const double λ) {
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

std::vector<double> dist_poisson(const int n, const double λ) {
  const std::vector<double> rnd = dist_exp(n, λ);
  std::vector<double> poisson(n);

  double t = 0;
  for (int i = 0; i < n; i++) {
    t += rnd[i];
    poisson[i] = t;
  }

  return poisson;
}

std::vector<double> simulate(const double λ) {
  const auto poisson = dist_poisson(N, λ);
  const auto uniform = dist_uniform(M, poisson.back());

  std::vector<double> wm(M, 0);
  for (int i = 0; i < M; i++) {
    const auto time = uniform[i];
    const auto wait = std::lower_bound(poisson.begin(), poisson.end(), time);
    if (wait != poisson.end()) {
      wm[i] = *wait - time;
    } else {
      wm[i] = 0;
    }
  }
  for (int i = 0; i < M - 1; i++) {
    wm[i + 1] += wm[i];
  }
  for (int i = 0; i < M; i++) {
    wm[i] /= (i + 1);
  }

  return wm;
}

int main() {
  const auto wm1 = simulate(1.0);
  const auto wm2 = simulate(1.5);
  const auto wm3 = simulate(2.0);
  const auto wm4 = simulate(2.5);

  std::ofstream file("3-4.csv");
  file << "m,1.0,1.5,2.0,2.5" << std::endl;
  for (int i = 0; i < M; i++) {
    file << i;
    file << "," << wm1[i];
    file << "," << wm2[i];
    file << "," << wm3[i];
    file << "," << wm4[i];
    file << std::endl;
  }
}
