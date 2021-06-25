#include "../include/cal_date_list.h"
#include <sstream>

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

void CalDateList::sum(){
	vector<double> probs;
	vector<int> full_bp;
   vector<double> new_probs;

   for(auto it = _dates.begin();  it != _dates.end(); ++it) {
      CalDate element = *it;
	   if (it == _dates.begin()) {
	      full_bp = element.get_full_bp();
	      probs = element.get_full_probabilities();
      } else {
         vector<double> new_probs = element.get_full_probabilities();
   		std::transform (  probs.begin(),
   		                  probs.end(), 
   		                  new_probs.begin(),
   		                  new_probs.begin(), 
   		                  std::plus<double>());
		}
		if (it == _dates.end()) full_bp = element.get_full_bp();
	}
	CalDate sum_date = CalDate("sum", probs, full_bp, 0, 0, full_bp, probs);
	_dates.push_back(sum_date);
}
