#include "../include/cal_date.h"
#include <sstream>
#include <functional>
#include <iterator>
#include <cstdlib>  // For rand()
#include <cmath>  // For floor(), round()

CalDate::CalDate(std::string name, std::vector<double> probabilities, std::vector<int> bp, int uncal_bp, int uncal_error, std::vector<int> full_bp, std::vector<double> full_probabilities):
    _name(name),
    _probabilities(probabilities),
    _bp(bp),
    _uncal_bp(uncal_bp),
    _uncal_error(uncal_error),
    _full_probabilities(full_probabilities),
    _full_bp(full_bp)
    {}

CalDate::CalDate(std::string name, std::vector<double> probabilities, std::vector<int> bp, int uncal_bp, int uncal_error):
    _name(name),
    _probabilities(probabilities),
    _bp(bp),
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
    int nrun = 5000;
    std::vector<double> prob_vector;
    std::vector<double> cum_sum(_probabilities.size());

    std::partial_sum(_probabilities.begin(), _probabilities.end(), cum_sum.begin(), std::plus<double>());
    double max = cum_sum.back();

    for (int i = 0; i < nrun; ++i) {
        double linear = rand() * max / RAND_MAX;
        int up = std::upper_bound(cum_sum.begin(), cum_sum.end(), linear) - cum_sum.begin();
        prob_vector.push_back(_probabilities[up]);
    }

    std::sort(prob_vector.begin(), prob_vector.end());
    int n = prob_vector.size();
    int thres_index = floor((n - 1) * prob);
    double threshold = prob_vector[thres_index];
    std::vector<int> index;

    auto greater_than_threshold = [threshold](double value) { return value > threshold; };
    auto less_than_threshold = [threshold](double value) { return value < threshold; };

    std::vector<double>::iterator lower;
    std::vector<double>::iterator upper;
    unsigned i = 0;
    while (i < _probabilities.size()) {
        lower = std::find_if(_probabilities.begin() + i, _probabilities.end(), greater_than_threshold);
        upper = std::find_if(lower, _probabilities.end(), less_than_threshold);

        if (upper == _probabilities.end()) break;

        index.push_back(lower - _probabilities.begin());
        index.push_back(upper - _probabilities.begin());
        i = upper - _probabilities.begin();
        i++;
    }

    int ni = index.size();
    std::vector<int> bp_collector;
    if (ni > 0) {
        for (int j = 0; j < ni; j++) {
            int a = index[j];
            int b = index[j] - 1;
            double y1 = _bp[a];
            double y2 = _bp[b];
            double mu = (threshold - _probabilities[a]) / (_probabilities[b] - _probabilities[a]);
            int this_sigma_end = round(LinearInterpolate(y1, y2, mu));
            bp_collector.push_back(this_sigma_end);
        }
    }

    auto last = std::unique(bp_collector.begin(), bp_collector.end());
    bp_collector.erase(last, bp_collector.end());
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
    std::stringstream ss;
    for (unsigned i = 0; i < _bp.size(); ++i) {
        ss << _name << "," << _bp[i] << "," << _probabilities[i] << "\n";
    }
    std::string return_value = ss.str();
    return return_value;
}
