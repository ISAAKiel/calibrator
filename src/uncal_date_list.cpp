#include "../include/uncal_date_list.h"
#include "../include/parallel.h"

UncalDateList::UncalDateList(vector<UncalDate> dates):
	_dates(dates)
	{}
UncalDateList::UncalDateList():
	_dates()
	{}

vector<UncalDate> UncalDateList::get_dates(){
	vector<UncalDate> return_value(_dates);
	return return_value;
};

void UncalDateList::push_back(UncalDate date){
	_dates.push_back(std::move(date));
};

CalDateList UncalDateList::calibrate(CalCurve &calcurve){
	vector<CalDate> results(_dates.size());
	parallel_for(_dates.size(), [&](size_t i) {
		results[i] = _dates[i].calibrate(calcurve);
	});
	return CalDateList(std::move(results));
};
