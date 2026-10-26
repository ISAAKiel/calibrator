#include "../include/cal_curve.h"
#include <cmath>
#include <cstdlib>

CalCurve::CalCurve(vector<int> cal_bp, vector<int> c14_bp, vector<int> error):
     cal_bp_(std::move(cal_bp)),
     c14_bp_(std::move(c14_bp)),
     error_(std::move(error))
     { build_ascending(); }
CalCurve::CalCurve() : cal_bp_(), c14_bp_(), error_()
     {}

int CalCurve::rows() const {
     return cal_bp_.size();
}

int CalCurve::import(string file) {

    int head_length = 10;

    cal_bp_.clear();
    c14_bp_.clear();
    error_.clear();
    ifstream in(file.c_str());
    if (!in.is_open()) {
      printf("%s does not exist\n", file.c_str());
      exit(EXIT_FAILURE);
      return 1;
    }

    string line;
    int counter = 0;
    while (getline(in, line)) {
        if (counter > head_length) {
          const char* p = line.c_str();
          char* end;
          long v[3];
          bool ok = true;
          for (int k = 0; k < 3; k++) {
            v[k] = strtol(p, &end, 10);
            if (*end != ',') { ok = false; break; }
            p = end + 1;
          }
          if (!ok)
            break;
          cal_bp_.push_back((int)v[0]);
          c14_bp_.push_back((int)v[1]);
          error_.push_back((int)v[2]);
        }
        counter++;
    }
  build_ascending();
  return EXIT_SUCCESS;
}

int CalCurve::max_bp_cal_curve() const {
  return *std::max_element(cal_bp_.begin(), cal_bp_.end());
}

int CalCurve::min_bp_cal_curve() const {
  return *std::min_element(cal_bp_.begin(), cal_bp_.end());
}

const vector<int>& CalCurve::get_error() const {
  return error_;
}

const vector<int>& CalCurve::get_bp() const {
  return cal_bp_;
}

const vector<int>& CalCurve::get_c14_bp() const {
  return c14_bp_;
}

void CalCurve::build_ascending() {
  vector<size_t> order(cal_bp_.size());
  for (size_t i = 0; i < order.size(); i++) order[i] = i;
  std::sort(order.begin(), order.end(),
            [&](size_t a, size_t b) { return cal_bp_[a] < cal_bp_[b]; });
  asc_cal_.clear(); asc_c14_.clear(); asc_err_.clear();
  for (size_t i : order) {
    asc_cal_.push_back(cal_bp_[i]);
    asc_c14_.push_back(c14_bp_[i]);
    asc_err_.push_back(error_[i]);
  }
}

static int floor_to(int x, int step) {
  int r = x % step;
  return r < 0 ? x - r - step : x - r;
}

bool CalCurve::relevant_range(double c14_age, double sigma, double k, int step,
                              int &lo, int &hi) const {
  const size_t n = asc_cal_.size();
  const double sigma2 = sigma * sigma;
  size_t first = n, last = 0;
  for (size_t i = 0; i < n; i++) {
    const double d = c14_age - asc_c14_[i];
    const double limit2 = k * k * (sigma2 + asc_err_[i] * asc_err_[i]);
    if (d * d < limit2) {
      if (first == n) first = i;
      last = i;
    }
  }
  if (first == n) return false;
  // include the neighbouring nodes: the interpolated curve between them
  // and the first/last relevant node may still be relevant
  if (first > 0) first--;
  if (last + 1 < n) last++;
  lo = -floor_to(-asc_cal_[first], step);  // ceil to multiple of step
  hi = floor_to(asc_cal_[last], step);
  return lo <= hi;
}

void CalCurve::sample(int hi, int lo, int step, vector<int> &cal_bp,
                      vector<double> &c14_bp, vector<double> &error) const {
  const size_t count = hi >= lo ? (size_t)((hi - lo) / step + 1) : 0;
  cal_bp.resize(count);
  c14_bp.resize(count);
  error.resize(count);
  if (count == 0) return;
  // index of the first node >= hi; walk downwards from there
  size_t j = std::lower_bound(asc_cal_.begin(), asc_cal_.end(), hi) - asc_cal_.begin();
  for (size_t t = 0; t < count; t++) {
    const int x = hi - (int)t * step;
    while (j > 0 && asc_cal_[j - 1] >= x) j--;
    // now asc_cal_[j] >= x > asc_cal_[j-1] (or j == 0)
    if (asc_cal_[j] == x || j == 0) {
      c14_bp[t] = asc_c14_[j];
      error[t] = asc_err_[j];
    } else {
      const double mu = (double)(x - asc_cal_[j - 1]) / (asc_cal_[j] - asc_cal_[j - 1]);
      c14_bp[t] = asc_c14_[j - 1] + (asc_c14_[j] - asc_c14_[j - 1]) * mu;
      error[t] = asc_err_[j - 1] + (asc_err_[j] - asc_err_[j - 1]) * mu;
    }
    cal_bp[t] = x;
  }
}
