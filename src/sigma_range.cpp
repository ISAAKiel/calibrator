#include "../include/sigma_range.h"

SigmaRange::SigmaRange(int begin, int end, double sigma_level):
	_begin(begin),
	_end(end),
	_sigma_level(sigma_level)
	{}

json SigmaRange::to_json() {
	json this_json;
	this_json["sigma_level"] = _sigma_level;
	this_json["begin"] = _begin;
	this_json["end"] = _end;
	return this_json;
}
