/**
 * @file cal_date.h
 * \class CalDate
 *
 * \brief Represents a calibrated date
 *
 * This class represents a calibrated date
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


#ifndef _cal_date_h_
#define _cal_date_h_

#include "sigma_range.h"
#include <iostream>
#include <vector>
#include "json.hpp"
#include <random>

using json = nlohmann::json;

using namespace std;

class CalDate{
	public:
      CalDate(string name, vector<double> probabilities, vector<int> bp, int uncal_bp, int uncal_error, vector<int> full_bp, vector<double> full_probabilities);
      CalDate(string name, vector<double> probabilities, vector<int> bp, int uncal_bp, int uncal_error);
      CalDate();
   	void info()const;
   		json to_json();
   		string get_name();
		void calculate_sigma_ranges();
		vector<SigmaRange> get_sigma_ranges();
		vector<int> get_full_bp();
		vector<double> get_full_probabilities();
		string to_csv();
	private:
		string _name;
		int _uncal_bp;
		int _uncal_error;
		vector<double> _probabilities;
		vector<int> _bp;
		vector<double> _full_probabilities;
		vector<int> _full_bp;
		json prob_to_json();
		json bp_to_json();
		vector<SigmaRange> _sigma_ranges;
		double LinearInterpolate(double y1, double y2, double mu);
		vector<int> sigma_range_helper(double &prob);
		json sigma_ranges_to_json() ;
};

#endif

