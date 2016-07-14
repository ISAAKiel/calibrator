/**
 * @file sigma_range.h
 * \class SigmaRange
 *
 * \brief Represents a sigma range for a calibrated date
 *
 * This class represents a sigma range for a calibrated date
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

#ifndef _sigma_range_h_
#define _sigma_range_h_

#include "json.hpp"

using json = nlohmann::json;

class SigmaRange{
	public:
		int _begin;
		int _end;
		double _sigma_level;
		SigmaRange(int begin, int end, double sigma_level);
		json to_json();
};

#endif
