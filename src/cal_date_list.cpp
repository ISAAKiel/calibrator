#include "../include/cal_date_list.h"
#include <sstream>
#include "../include/parallel.h"
#include <algorithm>
#include <map>


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
	for (auto& element : _dates) {
			return_value[element.get_name()] = element.to_json();
	}
	return return_value;
}

std::vector<std::string> CalDateList::json_parts(){
	// Like a json object (std::map): keys sorted, the last date wins if
	// two dates have the same name.
	std::map<std::string, size_t> by_name;
	for (size_t i = 0; i < _dates.size(); i++) by_name[_dates[i].get_name()] = i;
	std::vector<size_t> order;
	for (auto& kv : by_name) order.push_back(kv.second);
	std::vector<std::string> parts(order.size());
	parallel_for(order.size(), [&](size_t k) {
		CalDate& d = _dates[order[k]];
		parts[k] = json(d.get_name()).dump() + ":" + d.to_json().dump();
	});
	return parts;
}

void CalDateList::write_json(std::ostream& os){
	// Same result as os << to_json(), but each date is serialised in
	// parallel and the full document is never held as a json tree.
	std::vector<std::string> parts = json_parts();
	if (parts.empty()) { os << json(); return; }  // "null"
	os << '{';
	for (size_t k = 0; k < parts.size(); k++) {
		if (k) os << ',';
		os << parts[k];
		std::string().swap(parts[k]);  // free as we go
	}
	os << '}';
}

std::string CalDateList::to_json_string(){
	std::ostringstream ss;
	write_json(ss);
	return ss.str();
}

std::vector<std::string> CalDateList::csv_parts(){
	std::vector<std::string> parts(_dates.size());
	parallel_for(_dates.size(), [&](size_t i) { parts[i] = _dates[i].to_csv(); });
	return parts;
}

void CalDateList::write_csv(std::ostream& os){
	std::vector<std::string> parts = csv_parts();
	os << "name,bp,probability\n";
	for (auto& p : parts) { os << p; std::string().swap(p); }
}

string CalDateList::to_csv(){
	std::ostringstream ss;
	write_csv(ss);
	return ss.str();
}

void CalDateList::sum() {
    if (_dates.empty()) return;  // Early return if _dates is empty

    // Range of calendar years covered by any date
    int min_bp = 0, max_bp = -1;
    bool any = false;
    for (const auto& element : _dates) {
        const std::vector<int>& bps = element.full_bp_ref();
        if (bps.empty()) continue;
        auto mm = std::minmax_element(bps.begin(), bps.end());
        if (!any || *mm.first < min_bp) min_bp = *mm.first;
        if (!any || *mm.second > max_bp) max_bp = *mm.second;
        any = true;
    }

    // Accumulate the probabilities year by year in a dense array
    std::vector<double> acc(any ? (size_t)(max_bp - min_bp + 1) : 0, 0.0);
    for (const auto& element : _dates) {
        const std::vector<int>& bps = element.full_bp_ref();
        const std::vector<double>& probs = element.full_probabilities_ref();
        for (size_t i = 0; i < bps.size(); ++i)
            acc[bps[i] - min_bp] += probs[i];
    }

    // Filter out probabilities smaller than 1e-5 (ascending bp order)
    std::vector<int> filtered_bp;
    std::vector<double> filtered_probs;
    for (size_t i = 0; i < acc.size(); ++i) {
        if (acc[i] >= 1e-5) {
            filtered_bp.push_back(min_bp + (int)i);
            filtered_probs.push_back(acc[i]);
        }
    }

    if (!filtered_probs.empty()) {
        _dates.push_back(CalDate("sum", filtered_probs, filtered_bp, 0, 0, filtered_bp, filtered_probs));
    } else {
        std::cerr << "Filtered probs or full_bp is empty" << std::endl;
    }
}

void CalDateList::calculate_sigma_ranges() {
    parallel_for(_dates.size(), [&](size_t i) {
        _dates[i].calculate_sigma_ranges();
    });
}
