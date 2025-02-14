#include <algorithm>
#include <random>
#include <vector>

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

int main() {}
