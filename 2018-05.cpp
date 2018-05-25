#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <complex>
#include <condition_variable>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits.h>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <valarray>
#include <vector>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

template<typename T>
class _DisplayType;

template<typename T>
void _displayType(T&& t);

#define PEEK(x) LOG(INFO) << #x << ": [" << (x) << "]"

constexpr int N = 11;
constexpr int C = N * (N - 1) / 2;
constexpr auto Mask = (1<<N)  - 1;
std::pair<int, int> Table[C];

using Map = std::map<int, std::vector<int64_t>>;

struct Config {
  int conf;
  int cnt;
  int64_t sets[3];
};

int64_t iterateAll(int a, int b, std::function<bool(int)> f) {
  int64_t ret = 0;
  for (int i = 0; i < N; ++i) {
    for (int j = i + 1; j < N; ++j) {
      int mk = (1 << i) ^ (1 << j);
      if (f(mk)) {
        int idx = (i * N - (i * (i + 1) / 2) + j - i - 1);
        CHECK_LT(idx, C);
        ret ^= int64_t(1) << idx;
        Table[idx] = {i, j};
      }
    }
  }
  return ret;
}

int64_t feasibleSetEqual(int a, int b) {
  return iterateAll(a, b, [=](int mk) {
      return __builtin_popcountl(a & mk) == __builtin_popcountl(b & mk);
  });
}

int64_t feasibleSetLess(int a, int b) {
  return iterateAll(a, b, [=](int mk) {
      return __builtin_popcountl(a & mk) > __builtin_popcountl(b & mk);
  });
}

std::vector<Config> genAllConfigs() {
  std::vector<Config> configs;
  int filtered = 0;
  for (int a = Mask; a > 0; a = (a - 1) & Mask) {
    for (int b = a; b > 0; b = (b - 1) & Mask) {
      if (((a & b) == 0) && __builtin_popcountl(a) == __builtin_popcountl(b)) {
        int64_t outcomes[3] = {
          feasibleSetLess(b, a),
          feasibleSetEqual(b, a),
          feasibleSetLess(a, b),
        };
        CHECK_EQ(0, outcomes[0] & outcomes[1]);
        CHECK_EQ(0, outcomes[1] & outcomes[2]);
        CHECK_EQ(0, outcomes[0] & outcomes[2]);
        if (
          __builtin_popcountl(outcomes[0]) <= 27 &&
          __builtin_popcountl(outcomes[1]) <= 27 &&
          __builtin_popcountl(outcomes[2]) <= 27 
        ) {
          configs.push_back({
            (b << N) ^ a,
            __builtin_popcountl(b) * 2,
            {
              outcomes[0],
              outcomes[1],
              outcomes[2],
            }
          });
        } else {
          ++filtered;
        }
      }
    }
  }
  LOG(INFO) << "Filtered " << filtered << " configs.";
  std::sort(configs.begin(), configs.end(), [](const auto& a, const auto& b) {
    return a.cnt < b.cnt;
  });
  return configs;
}

std::string weighOne(int config) {
  auto oneSet = [](int a) {
    std::string res;
    for (int i = 0; i < N; ++i) {
      if (a & (1 << i)) {
        res.append(1, char('A' + i));
      }
    }
    return res;
  };
  return oneSet(config >> N) + "-" + oneSet(config & Mask);
}

void printOutcome(int64_t outcome) {
  for (int k = 0; k < C; ++k) {
    if (outcome & (int64_t(1) << k)) {
      std::cout << " (" << char('A' + Table[k].first) << ", "
                << char('A' + Table[k].second) << "),";
    }
  }
}

void explainConfig(const Config& config) {
  std::vector<std::string> names = {
    "less", "equal", "more",
  };
  auto conf = config.conf;
  std::cout << weighOne(conf) << std::endl;
  const auto& sets = config.sets;
  for (int i = 0; i < 3; ++i) {
    std::cout << "  " << names[i] << ":";
    printOutcome(config.sets[i]);
    std::cout << std::endl;
  }
}

bool goodStep(const int64_t set1[], int n, const int64_t set2[], int max,
              int64_t outcomes[]) {
  for (int i = 0; i < n; ++i) {
    outcomes[i * 3] = set1[i] & set2[0];
    outcomes[i * 3 + 1] = set1[i] & set2[1];
    outcomes[i * 3 + 2] = set1[i] & set2[2];
    if (__builtin_popcountl(set1[i] & set2[0]) > max ||
        __builtin_popcountl(set1[i] & set2[1]) > max ||
        __builtin_popcountl(set1[i] & set2[2]) > max) {
      return false;
    }
  }
  return true;
}

bool verify(const std::vector<int>& solution) {
  std::map<int, std::string> seen;
  for (int a = 0; a < N; ++a) {
    for (int b = 0; b < a; ++b) {
      const int mk = (1 << a) ^ (1 << b);
      int res = 0;
      for (int x : solution) {
        int diff = __builtin_popcountl((x >> N) & mk) - __builtin_popcountl((x & Mask) & mk);
        if (diff) {
          diff /= abs(diff);
        }
        // PEEK(diff);
        res = (res << 2) ^ (diff + 1);
      }
      std::string two = {
          char(a + 'A'), char(b + 'A'),
      };
      // PEEK(two);
      if (seen.count(res)) {
        std::cout << two << " conflicts with " << seen[res] << std::endl;
        return false;
      }
      seen[res] = two;
    }
  }
  return true;
}

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  PEEK(Mask);
  CHECK_EQ(__builtin_popcountll((int64_t(1) << 54) + 1), 2);
  const auto& configs = genAllConfigs();
  LOG(INFO) << "There are " << configs.size() << " configs.";
  // explainConfig(configs[0]);
  // explainConfig(configs[10000]);

  std::set<int> seen;
  std::atomic<int> solutions{0};
  auto tryWithFirstCount = [&](std::set<int> firstCount) {
    for (int a = 0; a < configs.size(); ++a) {
      int ca = __builtin_popcountl(configs[a].conf);
      if (firstCount.count(ca) == 0) {
        continue;
      }
      firstCount.erase(ca);
      for (int b = 0; b < configs.size(); ++b) {
        if (configs[b].cnt > ca) {
          continue;
        }
        int64_t outcomes2[9];
        if (goodStep(configs[a].sets, 3, configs[b].sets, 9, outcomes2)) {
          int64_t outcomes3[27];
          for (int c = 0; c < b; ++c) {
            if (goodStep(outcomes2, 9, configs[c].sets, 3, outcomes3)) {
              int64_t outcomes4[27 * 3];
              for (int d = 0; d < c; ++d) {
                LOG_EVERY_N(INFO, 3000000)
                    << "Trying #" << ::google::COUNTER << " combination at ("
                    << ca << " " << b << " " << c << ") with " << solutions
                    << " solutions so far.";
                if (goodStep(outcomes3, 27, configs[d].sets, 1, outcomes4)) {
                  std::vector<int> solution;
                  for (auto x : {a, b, c, d}) {
                    solution.push_back(configs[x].conf);
                  }
                  CHECK(verify(solution));
                  ++solutions;
                  LOG_FIRST_N(INFO, 10)
                      << "Found one solution: " << weighOne(configs[a].conf)
                      << " " << weighOne(configs[b].conf) << " "
                      << weighOne(configs[c].conf) << " "
                      << weighOne(configs[d].conf);
                }
              }
            }
          }
        }
      }
    }
  };
  #pragma omp parallel for
  for (int ca = 2; ca <= 10; ca += 2) {
    tryWithFirstCount({ca});
  }
  LOG(INFO) << "Found " << solutions << " solutions.";
  return 0;
}

