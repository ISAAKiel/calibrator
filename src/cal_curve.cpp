#include "../include/cal_curve.h"
#include <cmath>
#include <cstdlib>

CalCurve::CalCurve(vector<int> cal_bp, vector<int> c14_bp, vector<int> error):
     cal_bp_(std::move(cal_bp)),
     c14_bp_(std::move(c14_bp)),
     error_(std::move(error))
     {}
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
    grid_valid_ = false;
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

const CalCurve::Grid& CalCurve::grid() {
  if (!grid_valid_) build_grid();
  return grid_;
}

static int linear_interpolate_int(int y1, int y2, double mu) {
  return (int)round(y1 * (1 - mu) + y2 * mu);
}

void CalCurve::build_grid() {
  const int max_bp = max_bp_cal_curve();
  const int num_elements = round((max_bp - min_bp_cal_curve()) / grid_step);

  // Ascending copies of the curve for binary search.
  vector<int> asc_bp(cal_bp_.rbegin(), cal_bp_.rend());
  vector<int> asc_c14(c14_bp_.rbegin(), c14_bp_.rend());
  vector<int> asc_err(error_.rbegin(), error_.rend());

  grid_.bp.assign(num_elements, 0);
  grid_.c14_bp.assign(num_elements, 0);
  grid_.error.assign(num_elements, 0);

  for (int i = 0; i < num_elements; i++) {
    const int this_bp = max_bp - i * grid_step;
    auto it = std::lower_bound(asc_bp.begin(), asc_bp.end(), this_bp);
    const size_t pos = it - asc_bp.begin();
    int this_c14, this_error;
    if (it != asc_bp.end() && *it == this_bp) {
      // exact hit on a curve node
      this_c14 = asc_c14[pos];
      this_error = asc_err[pos];
    } else {
      // linear interpolation between neighbouring nodes
      const int upper_bp = asc_bp[pos], lower_bp = asc_bp[pos - 1];
      const double mu = (double)(this_bp - lower_bp) / (double)(upper_bp - lower_bp);
      this_c14 = linear_interpolate_int(asc_c14[pos - 1], asc_c14[pos], mu);
      this_error = linear_interpolate_int(asc_err[pos - 1], asc_err[pos], mu);
    }
    grid_.bp[i] = this_bp;
    grid_.c14_bp[i] = this_c14;
    grid_.error[i] = this_error;
  }
  grid_valid_ = true;
}
