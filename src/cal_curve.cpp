#include "../include/cal_curve.h"

CalCurve::CalCurve(vector<int> cal_bp, vector<int> c14_bp, vector<int> error):
     cal_bp_(cal_bp),
     c14_bp_(c14_bp),
     error_(error)
     {}
CalCurve::CalCurve() : cal_bp_(), c14_bp_(), error_()
     {}

int CalCurve::rows() {
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

    vector< string > vec;
    string line;
    int counter = 0;
    while (getline(in, line)) {
        if (counter > head_length) {
          std::stringstream iss(line);
          std::string val;
          std::getline(iss, val, ',');
          if ( !iss.good() )
            break;
          cal_bp_.push_back(atoi(val.c_str()));
          std::getline(iss, val, ',');
          if ( !iss.good() )
            break;
          c14_bp_.push_back(atoi(val.c_str()));
          std::getline(iss, val, ',');
          if ( !iss.good() )
            break;
          error_.push_back(atoi(val.c_str()));
        }
        counter++;
    }
  return EXIT_SUCCESS;
}

int CalCurve::max_bp_cal_curve() {
  int my_max = *std::max_element(cal_bp_.begin(), cal_bp_.end());
  return my_max;
}

int CalCurve::min_bp_cal_curve() {
  int my_min = *std::min_element(cal_bp_.begin(), cal_bp_.end());
  return my_min;
}

vector<int> CalCurve::get_error() {
  return error_;
}

vector<int> CalCurve::get_bp() {
  return cal_bp_;
}

vector<int> CalCurve::get_c14_bp() {
  return c14_bp_;
}
