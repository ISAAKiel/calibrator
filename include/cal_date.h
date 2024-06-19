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
class CalDate {
public:
    CalDate(std::string name, std::vector<double> probabilities, std::vector<int> bp, int uncal_bp, int uncal_error, std::vector<int> full_bp, std::vector<double> full_probabilities);
    CalDate(std::string name, std::vector<double> probabilities, std::vector<int> bp, int uncal_bp, int uncal_error);
    CalDate();

    void info() const;
    json to_json();
    json prob_to_json();
    json bp_to_json();
    std::string get_name();
    std::vector<int> get_full_bp() const;  // Marked as const
    std::vector<double> get_full_probabilities() const;  // Marked as const
    void calculate_sigma_ranges();
    std::vector<SigmaRange> get_sigma_ranges();
    json sigma_ranges_to_json();
    std::string to_csv();
    
private:
    std::vector<int> sigma_range_helper(double &prob);
    double LinearInterpolate(double y1, double y2, double mu);

    std::string _name;
    std::vector<double> _probabilities;
    std::vector<int> _bp;
    int _uncal_bp;
    int _uncal_error;
    std::vector<double> _full_probabilities;
    std::vector<int> _full_bp;
    std::vector<SigmaRange> _sigma_ranges;
};

#endif // CAL_DATE_H
