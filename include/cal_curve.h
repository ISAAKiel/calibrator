/**
 * @file cal_curve.h
 * \class CalCurve
 *
 * \brief Represents the calibration curve
 *
 * This class represents the calibration curve
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

#ifndef _cal_curve_h_
#define _cal_curve_h_

#include <iostream>
#include <string>
#include <fstream>
#include <iterator>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

class CalCurve{
public:
	/**
	 * Constructor for empty CalCurve()
	 */
	CalCurve();
	/**
	 * Constructor for CalCurve
	 *
	 * @param cal_bp a vector of bp values
	 * @param c14_bp a vector of 14C bp values
	 * @param error a vector of standard deviation from the calibration curve
	 */
    CalCurve(vector<int> cal_bp, vector<int> c14_bp, vector<int> error);

    /**
     * Imports a Calibration Curve from file
     * @param file The path of the calibration curve file.
     * @return 0 if successful, 1 otherwise
     */
    int import(string file);

    /**
     * returns the standard deviation  of the calibration curve
     *
     * @return a vector<int> of standard deviation from the calibration curve
     */
    vector<int> get_error();
    /**
     * @return a vector<int> of the bp values of the calibration curve
     */
    vector<int> get_bp();
    /**
     *
     * @return a vector<int> of the c14 bp values of the calibration curve
     */
    vector<int> get_c14_bp();
    /**
     *
     * @return the number of rows of the calibration curve
     */
    int rows();
    /**
     *
     * @return the max bp value of the calibration curve
     */
    int max_bp_cal_curve();
    /**
     *
     * @return the min bp value of the calibration curve
     */
    int min_bp_cal_curve();
private:
    /**
     * The calibrated bp values
     */
    vector<int> cal_bp_;
    /**
     * the c14 bp calues
     */
    vector<int> c14_bp_;
    /**
     * the error (standard deviation) of the calibration curve
     */
    vector<int> error_;
};

#endif
