#include "../include/uncal_date.h"
#include <cmath>

UncalDate::UncalDate(string name, int bp, int std):
_name(std::move(name)),
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

CalDate UncalDate::calibrate(const CalCurve &calcurve, int step, double k) const {
	vector<int> bp;
	vector<double> c14, err;
	int lo, hi;
	if (calcurve.relevant_range(_bp, _std, k, step, lo, hi))
		calcurve.sample(hi, lo, step, bp, c14, err);

	vector<double> probs = compute_probs(err, c14, step);
	vector<double> probs_return;
	vector<int> bp_return;
	for (size_t i = 0; i < probs.size(); i++)
	{
		if (probs[i] > 1.0e-05)
		{
			probs_return.push_back(probs[i]);
			bp_return.push_back(bp[i]);
		}
	}

	return CalDate(_name, std::move(probs_return), std::move(bp_return), _bp, _std, std::move(bp), std::move(probs));
};

vector<double> UncalDate::compute_probs(const vector<double> &error_cal_curve, const vector<double> &full_c14_bp, int step) const {
	const double df = 100.0;
	const double expo = -(df + 1.0) / 2.0;
	const size_t n = error_cal_curve.size();
	std::vector<double> prob_return_value(n);
	double prob_sum = 0;
	const double stdsq = (double)_std * _std;
	for (size_t t = 0; t < n; t++) {
		// combined variance of measurement and calibration curve
		const double curve_error = error_cal_curve[t];
		double this_tau = stdsq + curve_error * curve_error;
		double x = (_bp - full_c14_bp[t]) / sqrt(this_tau);
		double this_prob = exp(expo * log1p(x * x / df));
		prob_return_value[t] = this_prob;
		prob_sum += this_prob;
	}
	if (prob_sum <= 0) return prob_return_value;
	// density per calendar year (sums to 1 over the grid)
	const double norm = 1.0 / (prob_sum * step);
	for (double& f : prob_return_value) f *= norm;
	return prob_return_value;
}
