#include "../include/uncal_date.h"

UncalDate::UncalDate(string name, int bp, int std):
_name(name),
_bp(bp),
_std(std)
{}

UncalDate::UncalDate():
			_bp(),
			_std()
{}

void UncalDate::info()const{
	std::cout << "name: " << _name << "\n";
	std::cout << "bp: " << _bp << "\n";
	std::cout << "std: " << _std << "\n";
	std::cout << std::endl;
};

CalDate UncalDate::calibrate(CalCurve &calcurve){
	int step = 5;
	int num_elements = round((calcurve.max_bp_cal_curve() - calcurve.min_bp_cal_curve())/step);

	vector<int> full_bp(num_elements);
	vector<int> full_c14_bp(num_elements);
	vector<int> full_error(num_elements);

	generate_date_grid(full_bp, full_c14_bp, full_error, calcurve);

	vector<double> probs = compute_probs(full_error, full_c14_bp);
	vector<double> probs_return;
	vector<int> bp_return;
	for (unsigned i=0; i < probs.size(); i++)
	{
		if (probs[i] > 1.0e-05)
		{
			probs_return.push_back(probs[i]);
			bp_return.push_back(full_bp[i]);
		}
	}

	return CalDate(_name, probs_return, bp_return, _bp, _std, full_bp, probs);
	//	return CalDate(_name, probs_return, bp_return, _bp, _std);
};

vector<double> UncalDate::compute_probs(vector<int> &error_cal_curve, vector<int> &full_c14_bp){
	using boost::math::students_t; // typedef provides default type is double.
	unsigned t = 0;
	std::vector<double> prob_return_value;
	students_t dist(100);
	double prob_sum = 0;
	int stdsq = pow(_std,2);
	while (t < error_cal_curve.size()){
		double this_tau = stdsq + error_cal_curve[t];
		double this_value = (_bp - full_c14_bp[t]) / sqrt(this_tau);
		double this_prob;
		this_prob = pdf(dist, this_value);
		prob_return_value.push_back(this_prob);
		prob_sum += this_prob;
		t++;
	}
	for(double& f : prob_return_value) f/=prob_sum*5; // norm to 1
	return prob_return_value;
}

void UncalDate::generate_date_grid(std::vector<int>& full_bp, std::vector<int>& full_c14_bp, std::vector<int>& full_error, CalCurve& calcurve){

	vector<int> cal_bp = calcurve.get_bp();
	vector<int> error =  calcurve.get_error();
	vector<int> c14_bp = calcurve.get_c14_bp();

	int num_elements = round((calcurve.max_bp_cal_curve() - calcurve.min_bp_cal_curve())/5);
	int i = 0;
	int max_bp = calcurve.max_bp_cal_curve();

	vector<int> working_bp = cal_bp;
	vector<int> working_c14_bp = c14_bp;
	vector<int> working_error = error;

	std::reverse(working_bp.begin(), working_bp.end());
	std::reverse(working_c14_bp.begin(), working_c14_bp.end());
	std::reverse(working_error.begin(), working_error.end());

	while (i < num_elements) {
		int this_bp = max_bp - i * 5;
		vector<int>::iterator it=find(cal_bp.begin(), cal_bp.end(), this_bp);

		int pos = it - cal_bp.begin();
		int this_c14;
		int this_error;
		if(it!=cal_bp.end()){
			this_c14 = c14_bp[pos];
			this_error = error[pos];
		}
		else{
			auto const upper_it = std::upper_bound(working_bp.begin(), working_bp.end(), this_bp);
			int upper_pos =  upper_it - working_bp.begin();
			int upper_bp = working_bp[upper_pos];
			int lower_bp = working_bp[upper_pos-1];
			int upper_c14 = working_c14_bp[upper_pos];
			int lower_c14 = working_c14_bp[upper_pos-1];
			int upper_error = working_error[upper_pos];
			int lower_error = working_error[upper_pos-1];
			double mu = (double)(this_bp-lower_bp) / (double)(upper_bp - lower_bp);
			this_c14 =  LinearInterpolateInt(lower_c14, upper_c14,mu);
			this_error =  LinearInterpolateInt(lower_error, upper_error,mu);
		}
		full_bp[i] = this_bp;
		full_c14_bp[i] = this_c14;
		full_error[i] = this_error;
		i++;
	}
}

int UncalDate::LinearInterpolateInt(int y1, int y2, double mu){
	return (int)round(y1*(1-mu)+y2*mu);
}
