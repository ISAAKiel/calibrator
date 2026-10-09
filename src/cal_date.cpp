#include "../include/cal_date.h"
#include <sstream>
#include <functional>
#include <iterator>
#include <cstdlib>
#include <cstdio>
#include <numeric>
#include <algorithm>
#include <cmath>  // For floor(), round()

CalDate::CalDate(std::string name, std::vector<double> probabilities, std::vector<int> bp, int uncal_bp, int uncal_error, std::vector<int> full_bp, std::vector<double> full_probabilities):
    _name(std::move(name)),
    _probabilities(std::move(probabilities)),
    _bp(std::move(bp)),
    _uncal_bp(uncal_bp),
    _uncal_error(uncal_error),
    _full_probabilities(std::move(full_probabilities)),
    _full_bp(std::move(full_bp))
    {}

CalDate::CalDate(std::string name, std::vector<double> probabilities, std::vector<int> bp, int uncal_bp, int uncal_error):
    _name(std::move(name)),
    _probabilities(std::move(probabilities)),
    _bp(std::move(bp)),
    _uncal_bp(uncal_bp),
    _uncal_error(uncal_error)
    {}

CalDate::CalDate():
    _probabilities()
    {}

void CalDate::info() const {
    std::cout << "calibrated_date";
    std::cout << "bp: " << _uncal_bp;
    std::cout << "std: " << _uncal_error;
    std::cout << std::endl;
}

json CalDate::to_json() {
    json return_value;
    return_value["uncal_bp"] = _uncal_bp;
    return_value["uncal_error"] = _uncal_error;
    return_value["probabilities"] = prob_to_json();
    return_value["bp"] = bp_to_json();
    return_value["sigma_ranges"] = sigma_ranges_to_json();
    return return_value;
}

json CalDate::prob_to_json() {
    json return_value(_probabilities);
    return return_value;
}

json CalDate::bp_to_json() {
    json return_value(_bp);
    return return_value;
}

std::string CalDate::get_name() {
    return _name;
}

std::vector<int> CalDate::get_full_bp() const {  // Marked as const
    return _full_bp;
}

std::vector<double> CalDate::get_full_probabilities() const {  // Marked as const
    return _full_probabilities;
}

void CalDate::calculate_sigma_ranges() {
    std::vector<int> my_sigma_ranges;
    double this_sigma = 1 - 0.954;
    my_sigma_ranges = sigma_range_helper(this_sigma);

    for (unsigned i = 0; i < my_sigma_ranges.size(); i += 2) {
        SigmaRange this_sigma_range = SigmaRange(my_sigma_ranges[i], my_sigma_ranges[i + 1], 1 - this_sigma);
        _sigma_ranges.push_back(this_sigma_range);
    }
}

std::vector<int> CalDate::sigma_range_helper(double &prob) {
    const bool use_full = !_full_probabilities.empty() &&
                          _full_probabilities.size() == _full_bp.size();
    const std::vector<double>& p = use_full ? _full_probabilities : _probabilities;
    const std::vector<int>& bp = use_full ? _full_bp : _bp;
    const size_t n = p.size();
    std::vector<int> bp_collector;
    if (n == 0) return bp_collector;

    std::vector<double> sorted(p);
    std::sort(sorted.begin(), sorted.end(), std::greater<double>());
    const double total = std::accumulate(sorted.begin(), sorted.end(), 0.0);
    const double target = (1.0 - prob) * total;

    double threshold = sorted.back();
    double cum = 0;
    for (size_t k = 0; k < n; k++) {
        cum += sorted[k];
        if (cum >= target) {
            threshold = sorted[k];
            if (k > 0 && sorted[k] > 0)
                threshold += (sorted[k - 1] - sorted[k]) * (cum - target) / sorted[k];
            break;
        }
    }

    // Border between a point inside (i_in) and outside (i_out) the range.
    auto border = [&](size_t i_in, size_t i_out) {
        double mu = (threshold - p[i_out]) / (p[i_in] - p[i_out]);
        return (int)round(LinearInterpolate(bp[i_out], bp[i_in], mu));
    };

    size_t i = 0;
    while (i < n) {
        while (i < n && p[i] < threshold) i++;
        if (i == n) break;
        const size_t first = i;
        while (i < n && p[i] >= threshold) i++;
        const size_t last = i - 1;
        bp_collector.push_back(first > 0 ? border(first, first - 1) : bp[first]);
        bp_collector.push_back(last + 1 < n ? border(last, last + 1) : bp[last]);
    }
    return bp_collector;
}

double CalDate::LinearInterpolate(double y1, double y2, double mu) {
    return (y1 * (1 - mu) + y2 * mu);
}

std::vector<SigmaRange> CalDate::get_sigma_ranges() {
    return _sigma_ranges;
}

json CalDate::sigma_ranges_to_json() {
    json sigma_ranges_json;
    for (auto &this_sigma_range : _sigma_ranges)
        sigma_ranges_json.push_back(this_sigma_range.to_json());
    return sigma_ranges_json;
}

std::string CalDate::to_csv() {
    // snprintf("%g") formats exactly like the default std::ostream
    // (precision 6), but is considerably faster.
    std::string out;
    out.reserve(_bp.size() * (_name.size() + 24));
    char buf[64];
    for (size_t i = 0; i < _bp.size(); ++i) {
        int len = snprintf(buf, sizeof(buf), ",%d,%g\n", _bp[i], _probabilities[i]);
        out += _name;
        out.append(buf, len);
    }
    return out;
}
