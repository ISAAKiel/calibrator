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

CalDate UncalDate::calibrate(CalCurve &calcurve) const {
	return calibrate(calcurve.grid());
}

CalDate UncalDate::calibrate(const CalCurve::Grid &grid) const {
	vector<double> probs = compute_probs(grid.error, grid.c14_bp);
	vector<double> probs_return;
	vector<int> bp_return;
	for (size_t i = 0; i < probs.size(); i++)
	{
		if (probs[i] > 1.0e-05)
		{
			probs_return.push_back(probs[i]);
			bp_return.push_back(grid.bp[i]);
		}
	}

	return CalDate(_name, std::move(probs_return), std::move(bp_return), _bp, _std, grid.bp, std::move(probs));
};

vector<double> UncalDate::compute_probs(const vector<int> &error_cal_curve, const vector<int> &full_c14_bp) const {
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
	const double norm = 1.0 / (prob_sum * 5); // norm to 1
	for (double& f : prob_return_value) f *= norm;
	return prob_return_value;
}
