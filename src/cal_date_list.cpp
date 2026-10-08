#include "../include/cal_date_list.h"
#include <sstream>
#include "../include/parallel.h"
#include <set>
#include <map>
#include <numeric>
#include <algorithm>


CalDateList::CalDateList(vector<CalDate> dates):
	_dates(std::move(dates))
	{}
CalDateList::CalDateList():
	_dates()
	{}

vector<CalDate> CalDateList::get_dates(){
	vector<CalDate> return_value(_dates);
	return return_value;
};

void CalDateList::push_back(CalDate date){
	_dates.push_back(std::move(date));
}

json CalDateList::to_json(){
	json return_value;
	int i = 0;
	for (auto& element : _dates) {
			json this_date;
			string this_name = element.get_name();
			return_value[this_name] = element.to_json();
			i++;
	}
	return return_value;	
}

string CalDateList::to_csv(){
  std::stringstream ss;
	ss << "name,bp,probability\n";
	for (auto& element : _dates) {
		ss << element.to_csv();
	}
  std::string return_value = ss.str();
	return return_value;	
}

void CalDateList::sum() {
    if (_dates.empty()) return;  // Early return if _dates is empty
    {
        const std::vector<int>& ref_bp = _dates.front().full_bp_ref();
        bool same_grid = !ref_bp.empty();
        for (const auto& element : _dates) {
            if (element.full_bp_ref() != ref_bp) { same_grid = false; break; }
        }
        if (same_grid) {
            std::vector<double> acc(ref_bp.size(), 0.0);
            for (const auto& element : _dates) {
                const std::vector<double>& probs = element.full_probabilities_ref();
                for (size_t i = 0; i < acc.size(); ++i) acc[i] += probs[i];
            }
            // output in ascending bp order, as the map-based version does
            std::vector<int> filtered_bp;
            std::vector<double> filtered_probs;
            std::vector<size_t> order(ref_bp.size());
            std::iota(order.begin(), order.end(), 0);
            std::sort(order.begin(), order.end(),
                      [&](size_t a, size_t b) { return ref_bp[a] < ref_bp[b]; });
            for (size_t k : order) {
                if (acc[k] >= 1e-5) {
                    filtered_bp.push_back(ref_bp[k]);
                    filtered_probs.push_back(acc[k]);
                }
            }
            if (!filtered_probs.empty()) {
                _dates.push_back(CalDate("sum", filtered_probs, filtered_bp, 0, 0, filtered_bp, filtered_probs));
            } else {
                std::cerr << "Filtered probs or full_bp is empty" << std::endl;
            }
            return;
        }
    }

    // Determine the full range of years (bp)
    std::set<int> all_bps;
    for (const auto& element : _dates) {
        const std::vector<int>& bps = element.full_bp_ref();
        all_bps.insert(bps.begin(), bps.end());
    }

    // Create a unified map for probabilities
    std::map<int, double> unified_probs;
    for (int bp : all_bps) {
        unified_probs[bp] = 0.0;
    }

    // Accumulate probabilities for each year (bp)
    for (const auto& element : _dates) {
        const std::vector<int>& bps = element.full_bp_ref(); const std::vector<double>& probs = element.get_full_probabilities();
        

        for (size_t i = 0; i < bps.size(); ++i) {
            int bp = bps[i];
            double prob = probs[i];
            unified_probs[bp] += prob;
        }
    }

    // Filter out probabilities smaller than 1e-5 and collect final base pairs and probabilities
    std::vector<int> filtered_bp;
    std::vector<double> filtered_probs;
    for (const auto& entry : unified_probs) {
        if (entry.second >= 1e-5) {
            filtered_bp.push_back(entry.first);
            filtered_probs.push_back(entry.second);
        }
    }

    // Verify if filtered_probs and filtered_bp are not empty before creating sum_date
    if (!filtered_probs.empty() && !filtered_bp.empty()) {
        CalDate sum_date = CalDate("sum", filtered_probs, filtered_bp, 0, 0, filtered_bp, filtered_probs);
        _dates.push_back(sum_date);
    } else {
        std::cerr << "Filtered probs or full_bp is empty" << std::endl;
    }
}

void CalDateList::calculate_sigma_ranges() {
    parallel_for(_dates.size(), [&](size_t i) {
        _dates[i].calculate_sigma_ranges();
    });
}
