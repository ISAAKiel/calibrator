#include "../include/cal_date_list.h"
#include <sstream>
#include <set>


CalDateList::CalDateList(vector<CalDate> dates):
	_dates(dates)
	{}
CalDateList::CalDateList():
	_dates()
	{}

vector<CalDate> CalDateList::get_dates(){
	vector<CalDate> return_value(_dates);
	return return_value;
};

void CalDateList::push_back(CalDate date){
	_dates.push_back(date);
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

    // Determine the full range of years (bp)
    std::set<int> all_bps;
    for (const auto& element : _dates) {
        const std::vector<int>& bps = element.get_full_bp();
       
        all_bps.insert(bps.begin(), bps.end());
    }

    // Create a unified map for probabilities
    std::map<int, double> unified_probs;
    for (int bp : all_bps) {
        unified_probs[bp] = 0.0;
    }

    // Accumulate probabilities for each year (bp)
    for (const auto& element : _dates) {
        const std::vector<int>& bps = element.get_full_bp();
        const std::vector<double>& probs = element.get_full_probabilities();
        

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
