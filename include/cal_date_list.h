/**
 * @file cal_date_list.h
 * \class CalDateList
 *
 * \brief Represents a list of calibrated dates
 *
 * This class represents a list of calibrated dates
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

#ifndef _cal_date_list_h_
#define _cal_date_list_h_

#include "cal_date.h"
#include <vector>

using namespace std;

class CalDateList{
	public:
		CalDateList(vector<CalDate> dates);
		CalDateList();
		vector<CalDate> get_dates();
		void push_back(CalDate date);
		json to_json();
		vector<CalDate> _dates;
    string to_csv();
    void sum();
	private:

};

#endif
