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
#include <string>
#include <ostream>

using namespace std;

class CalDateList{
	public:
		CalDateList(vector<CalDate> dates);
		CalDateList();
		vector<CalDate> get_dates();
		void push_back(CalDate date);
		json to_json();
		/// Same as to_json().dump(), but faster and with less memory.
		std::string to_json_string();
		/// Write to_json().dump() / to_csv() directly to a stream.
		void write_json(std::ostream& os);
		void write_csv(std::ostream& os);
		vector<CalDate> _dates;
    string to_csv();
    void sum();
    /// Calculates the sigma ranges of all dates (in parallel).
    void calculate_sigma_ranges();
	private:
		std::vector<std::string> json_parts();
		std::vector<std::string> csv_parts();

};

#endif
