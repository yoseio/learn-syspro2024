#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

const int N = 100000;
const int K = 7;

class Event {
 public:
  Event(const int i, const double t) : id(i), time(t) {}
  ~Event() {}

  int id;
  double time;
};

std::vector<double> rnd_exp(const int n, const double λ) {
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

std::vector<Event> poisson_exp(const int n, const double λ) {
  const std::vector<double> rnd = rnd_exp(n, λ);
  double t = 0;
  std::vector<Event> events;
  events.reserve(n);

  for (int i = 0; i < n; i++) {
    t += rnd[i];
    events.emplace_back(i, t);
  }

  return events;
}

std::vector<double> simulate(const int n, const double λ) {
  auto events = poisson_exp(n, λ);

  std::sort(events.begin(), events.end(),
            [](const Event& a, const Event& b) { return a.time < b.time; });
  const int max_time = std::ceil(events.back().time);

  std::vector<int> counts(max_time, 0);
  for (const auto& event : events) {
    counts[std::floor(event.time)]++;
  }

  int counter = 0;
  std::vector<int> num_event_lst(K, 0);
  for (const auto& count : counts) {
    if (count < K) {
      counter++;
      num_event_lst[count]++;
    }
  }

  std::vector<double> result(K, 0);
  for (int i = 0; i < K; i++) {
    result[i] = static_cast<double>(num_event_lst[i]) / counter;
  }

  return result;
}

int main() {
  const auto lst1 = simulate(N, 1.0);
  const auto lst2 = simulate(N, 1.5);
  const auto lst3 = simulate(N, 2.0);
  const auto lst4 = simulate(N, 2.5);

  std::ofstream file("3-3.csv");
  file << "i,1.0,1.5,2.0,2.5" << std::endl;
  for (int i = 0; i < K; i++) {
    file << i;
    file << "," << lst1[i];
    file << "," << lst2[i];
    file << "," << lst3[i];
    file << "," << lst4[i];
    file << std::endl;
  }
}
