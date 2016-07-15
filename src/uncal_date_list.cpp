#include "../include/uncal_date_list.h"

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
	_dates.push_back(date);
};

CalDateList UncalDateList::calibrate(CalCurve &calcurve){
	CalDateList my_cal_date_list;
for (auto& element : _dates) {
			CalDate my_cal_date = element.calibrate(calcurve);
			my_cal_date_list.push_back(my_cal_date);
		}
	return my_cal_date_list;
};
