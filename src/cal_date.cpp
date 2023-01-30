#include "../include/cal_date.h"
#include <sstream>

CalDate::CalDate(string name, vector<double> probabilities, vector<int> bp, int uncal_bp, int uncal_error, vector<int> full_bp, vector<double> full_probabilities):
	_name(name),
	_probabilities(probabilities),
	_bp(bp),
	_uncal_bp(uncal_bp),
	_uncal_error(uncal_error),
	_full_probabilities(full_probabilities),
	_full_bp(full_bp)
	{}

CalDate::CalDate(string name, vector<double> probabilities, vector<int> bp, int uncal_bp, int uncal_error):
	_name(name),
	_probabilities(probabilities),
	_bp(bp),
	_uncal_bp(uncal_bp),
	_uncal_error(uncal_error)
	{}

CalDate::CalDate():
	_probabilities()
	{}

void CalDate::info()const{
	std::cout << "calibrated_date";
	std::cout << "bp: " << _uncal_bp;
	std::cout << "std: " << _uncal_error;
	std::cout << std::endl;
};

json CalDate::to_json(){
	json return_value;
	return_value["uncal_bp"] = _uncal_bp;
	return_value["uncal_error"] = _uncal_error;
	return_value["probabilities"] = prob_to_json();
	return_value["bp"] = bp_to_json();
	return_value["sigma_ranges"] = sigma_ranges_to_json();
	return return_value;
}

json CalDate::prob_to_json(){
	json return_value(_probabilities);
	return return_value;
}

json CalDate::bp_to_json(){
	json return_value(_bp);
	return return_value;
}

string CalDate::get_name() {
	return _name;
}

vector<int> CalDate::get_full_bp() {
	return _full_bp;
}

vector<double> CalDate::get_full_probabilities() {
	return _full_probabilities;
}

void CalDate::calculate_sigma_ranges() {
	static const double arr[] = {1 - 0.954};
	vector<int> my_sigma_ranges;
	for (unsigned t = 0; t < sizeof(arr)/sizeof(arr[0]); t++) {
		double this_sigma = arr[t];
		my_sigma_ranges.clear();
		while (my_sigma_ranges.empty() || my_sigma_ranges.size() % 2) {
			my_sigma_ranges = sigma_range_helper(this_sigma);
		}
		for (unsigned i=0;i < my_sigma_ranges.size(); i += 2) {
		SigmaRange this_sigma_range = SigmaRange(my_sigma_ranges[i], my_sigma_ranges[i+1], 1-this_sigma);
		_sigma_ranges.push_back(this_sigma_range);
		}
	}
}

vector<int> CalDate::sigma_range_helper(double &prob){
	int nrun = 5000;
	vector<double> prob_vector;
	vector<double> cum_sum(_probabilities.size());

	std::partial_sum(_probabilities.begin(), _probabilities.end(), cum_sum.begin(), plus<double>());

	double max=cum_sum.back();
	for(int i = 0;
	      i < nrun;
	      ++i)
	  {
	      double linear = rand()*max/RAND_MAX;
	      int up = upper_bound(cum_sum.begin(), cum_sum.end(), linear) - cum_sum.begin();
	      prob_vector.push_back(_probabilities[up]);
	  }

	std::sort (prob_vector.begin(), prob_vector.end());

	int n = prob_vector.size();
	int thres_index = floor( (n - 1) * prob);
	double threshold = prob_vector[thres_index];
	vector<int> index;

	vector<double>::iterator lower;
	vector<double>::iterator upper;
	unsigned i = 0;
	while ( i < _probabilities.size()) {
		lower = find_if (_probabilities.begin()+i, _probabilities.end(),    // range
                 bind2nd(greater<double>(),threshold));  // criterion
		upper = find_if (lower, _probabilities.end(),    // range
                 bind2nd(less<double>(),threshold));  // criterion

    if (upper == _probabilities.end()) break;
 
		index.push_back(lower - _probabilities.begin());
		index.push_back(upper - _probabilities.begin());
    i = upper - _probabilities.begin();
		i++;
	}

	int ni = index.size();
	vector<int> bp_collector;
	if (ni > 0)
	{
		for (int j = 0; j < ni; j++)
		{
			int a = index[j];
			int b = index[j]-1;
			double y1 = _bp[a];
			double y2 = _bp[b];
			double mu = (float)(threshold-_probabilities[a]) / (float)(_probabilities[b] - _probabilities[a]);
			int this_sigma_end = round(LinearInterpolate(y1, y2, mu));
			bp_collector.push_back(this_sigma_end);
		}
	}
	std::unique (bp_collector.begin(), bp_collector.end());
	bp_collector.erase( unique( bp_collector.begin(), bp_collector.end() ), bp_collector.end() );
	return bp_collector;
}

double CalDate::LinearInterpolate(double y1, double y2, double mu){
   return(y1*(1-mu)+y2*mu);
}

vector<SigmaRange> CalDate::get_sigma_ranges() {
	return _sigma_ranges;
}

json CalDate::sigma_ranges_to_json() {
	json sigma_ranges_json;
	for(auto &this_sigma_range : _sigma_ranges)
		sigma_ranges_json.push_back(this_sigma_range.to_json());
	return sigma_ranges_json;
}

string CalDate::to_csv() {
  std::stringstream ss;

  for(unsigned i = 0; i < _bp.size(); ++i)
  {
    ss << _name << "," << _bp[i] << "," << _probabilities[i] << "\n";
  }
  std::string return_value = ss.str();

  return return_value;
}
