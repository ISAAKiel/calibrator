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
		 * Calibrates the date. Only reads the (already built) grid of the
		 * calibration curve, so it may be called concurrently once
		 * calcurve.grid() has been called.
		 */
		CalDate calibrate(const CalCurve::Grid &grid) const;
		CalDate calibrate(CalCurve &calcurve) const;
	private:
		string _name;
		int _bp;
		int _std;
		vector<double> compute_probs(const vector<int> &error_cal_curve, const vector<int> &full_c14_bp) const;
};


#endif
