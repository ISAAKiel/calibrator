/**
 * @file uncal_date_list.h
 * \class UncalDateList
 *
 * \brief Represents a list of uncalibrated dates
 *
 * This class represents a list of uncalibrated dates
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

#ifndef _uncal_date_list_h_
#define _uncal_date_list_h_

#include "uncal_date.h"
#include "cal_curve.h"
#include "cal_date_list.h"
#include <vector>

using namespace std;

class UncalDateList{
	public:
		UncalDateList(vector<UncalDate> dates);
		UncalDateList();
		vector<UncalDate> get_dates();
		void push_back(UncalDate date);
		CalDateList calibrate(CalCurve &calcurve);
	private:
		vector<UncalDate> _dates;
};

#endif
