/**
 * @file uncal_date.h
 * \class UncalDate
 *
 * \brief Represents an uncalibrated date
 *
 * This class represents an uncalibrated date
 *
 * \author Martin Hinz
 *
 * \version 0.1
 *
 * \date 14/07/2016 11:07:28
 *
 * Contact: martin.hinz@ufg.uni-kiel.de
 *
 */

#ifndef _uncal_date_h_
#define _uncal_date_h_

#include <iostream>
#include "cal_date.h"
#include "cal_curve.h"
#include <numeric>

using namespace std;

class UncalDate{
	public:
    UncalDate(string name, int bp, int std);
    UncalDate();
		void info()const;
		/**
		 * Calibrates the date on a grid of `step` years, restricted to the
		 * part of the curve within `k` combined standard deviations of the
		 * 14C age. Only reads the curve, so it may be called concurrently.
		 * Dates outside the calibration curve yield an empty result.
		 */
		CalDate calibrate(const CalCurve &calcurve,
		                  int step = CalCurve::default_step,
		                  double k = 6.0) const;
	private:
		string _name;
		int _bp;
		int _std;
		vector<double> compute_probs(const vector<double> &error_cal_curve, const vector<double> &c14_bp, int step) const;
};


#endif
