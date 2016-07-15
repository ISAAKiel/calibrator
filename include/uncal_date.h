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
#include <boost/math/distributions/students_t.hpp>
#include <numeric>

using namespace std;

class UncalDate{
	public:
    UncalDate(string name, int bp, int std);
    UncalDate();
		void info()const;
		CalDate calibrate(CalCurve &calcurve);
	private:
		string _name;
		int _bp;
		int _std;
		vector<double> compute_probs(vector<int> &error_cal_curve, vector<int> &full_c14_bp);
		double studentT( int df );
		void generate_date_grid(std::vector<int>& full_bp, std::vector<int>& full_c14_bp, std::vector<int>& full_error, CalCurve& calcurve);
		int LinearInterpolateInt(int y1, int y2, double mu);
};


#endif
