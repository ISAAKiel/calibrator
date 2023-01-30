/**
@file main.cpp

@mainpage Calibrator
@author martin.hinz@ufg.uni-kiel.de

Calibrator is a small c++ command line tool to calibrate 14C dates. In can
process inputs in csv, json or as cli parameters, and it outputs csv or json,
that can be piped into a file.
*/

#include "../include/cal_curve.h"
#include "../include/uncal_date.h"
#include "../include/cal_date.h"
#include "../include/uncal_date_list.h"
#include "../include/cal_date_list.h"

// Include the headers relevant to the boost::program_options
// library
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <istream> 
#include <cctype> 

using namespace boost;
using namespace boost::program_options;

/** \brief Method to validate that a string contains numbers only.
  * \param a A string
  * \return An integer with 0 if the string has only numbers, 1 otherwise
  * 
  * This method checks if a string contains numbers only.
  */
int validate_numeric_string ( string &a )
{
  unsigned i;
  for ( i = 0; i < a.size(); i++ )
    if ( !isdigit ( a[i] ) ) return 1;
  return 0;
}

/** \brief Checks whether a string contains json or csv.
  * \param input_string A string
  * \return An integer with 1 if the string seem to be json,
  *                         2 if the string seem to be csv,
  *                         0 otherwise
  * 
  * This method tries to estimate whether a string is json or csv.
  */
int check_input_format(string &input_string) {
  if( !input_string.empty() ) {
    char first_char = input_string[0];
    if (first_char == '{' || first_char == '['){
      return 1; //json
    } else if (first_char == '"' || first_char == '\'' || isalpha(first_char)) {
      return 2; //csv
    } 
  }
	return 0;
}

/** \brief Converts a csv input string to json.
  * \param in A string
  * \return A json object
  * 
  * This method converts a csv string into a json object.
  */
json csv_input_to_json(string &in) {
    bool err = false;
    json return_value;
    unsigned head_length = 0;
    char first_char = in.front();
    if (isalpha(first_char)) head_length = 1;
    string line;
    unsigned counter = 0;
    std::istringstream iss(in);

    while (getline(iss, line)) {
        json temp_json;
        int bp = -1;
        int std = -1;
        if (counter >= head_length) {
          std::stringstream iss(line);
          if ( !iss.good() )
            break;
          std::string name_str;
          std::getline(iss, name_str, ',');
          std::string bp_str;
          std::getline(iss, bp_str, ',');
          std::string std_str;
          std::getline(iss, std_str, ',');
          if (!name_str.empty() && !bp_str.empty() && !std_str.empty()) {
            if (validate_numeric_string(bp_str)==0) bp=atoi(bp_str.c_str());
            if (validate_numeric_string(std_str)==0) std=atoi(std_str.c_str());
            if (bp > 0 && std > 0) {
              name_str.erase(remove( name_str.begin(), name_str.end(), '\"' ),name_str.end());
              name_str.erase(remove( name_str.begin(), name_str.end(), '\'' ),name_str.end());
              temp_json["bp"] = bp;
              temp_json["std"] = std;
              return_value[name_str] = temp_json;
            } else {
              err = true;
              break;
            }
          } else {
            err = true;
            break;
          }
        }
        counter++;
    }
    if (err) {
      cout << "Invalid file format!" << std::endl;
      cout << "Please use \"DateName\",bp,std" << std::endl;
      return EXIT_FAILURE;
    } else {
      return return_value;
    }
}

/// \brief  Main function
/// \param  argc An integer argument count of the command line arguments
/// \param  argv An argument vector of the command line arguments
/// \return an integer 0 upon exit success
int main(int argc , char **argv) {
  json j;

  /*
   * Defines the cli input parameters via boost.
   */

  options_description desc(
    "\nA tool for 14C calibration from the command line.\n\nAllowed arguments");

  desc.add_options()
    ("help,h", "Produce this help message.")
    ("input-file,i", value< vector<string> >(),
     "Specifies input file.")
    ("bp,b", value< vector<int> >(),
     "The BP Value.")
    ("std,s", value< vector<int> >(),
     "The standard deviation.")
    ("json-string,j", value< vector<string> >(),
     "Input as as JSON string. Format: {\"bp\": xx, \"std\": xx}")
    ("ranges,r", "calculate sigma ranges (only for json output).")
	("sum", "calculate sum probability.")
    ("output,o", value<string>()->default_value("json"), 
     "csv for csv-output, json for json (default).");

    positional_options_description p;
    p.add("input-file", -1);

    // Map the input parameters
    variables_map vm;

    // Parse the input parameters
    try {
        store(command_line_parser(
        argc, argv).options(desc).positional(p).run(), vm);
        notify(vm);
    } catch (std::exception &e) {
        cout << endl << e.what() << endl;
        cout << desc << endl;
    }

    // Display help text when requested
    if (vm.count("help")) {
        cout << "–help specified" << endl;
        cout << desc << endl;
        return EXIT_SUCCESS;
    }

    // Handles input files
    if (vm.count("input-file")) {
        vector<string> inputFilename =
            vm["input-file"].as< vector<string> >();

        string line;
        string file_content;
        ifstream myfile(inputFilename[0]);
        if (myfile.is_open()) {
          while ( getline (myfile, line) ) {
            file_content += line + '\n';
          }
          myfile.close();
        } else {
          cout << "Unable to open file";
        }
        int file_format = check_input_format(file_content);
        switch(file_format){
          case 1:
            j = json::parse(file_content);
            break;
          case 2:
            j = csv_input_to_json(file_content);
            break;
          default:
            cout << "Invalid file format!";
            return EXIT_FAILURE;
         }
    }

    // Handles json strings
    if (vm.count("json-string")) {
        vector<string>  json_string =
            vm["json-string"].as< vector<string> >();
        j = json::parse(json_string[0]);
    }

    // Handles output format parameter
    string output_format =
            vm["output"].as<string>();

    // Handles bp and std as cli parameters
    if (vm.count("bp") && vm.count("std")) {
        json j_temp;
        vector<int> bp =
            vm["bp"].as< vector<int> >();
        vector<int> std =
            vm["std"].as< vector<int> >();
        j_temp["bp"] = bp[0];
        j_temp["std"] = std[0];
        j["date"] = j_temp;
    }

    // Should the sigma ranges be calculated?
    bool calc_sigma_ranges = false;
    if (vm.count("ranges")) {
      calc_sigma_ranges = true;
    }

    // Instantize an uncalibrated date an a list for uncalibrated dates
    UncalDate my_date;
    UncalDateList my_date_list;

    for (json::iterator it = j.begin(); it != j.end(); ++it) {
      my_date = UncalDate(it.key(), it.value()["bp"], it.value()["std"]);
      my_date_list.push_back(my_date);
    }

    // Read the calibration curve
    string filename = "../data/intcal20.14c";
    CalCurve my_cal_curve = CalCurve();
    my_cal_curve.import(filename);

    // Calibrate all dates in the uncalibrated date list
    // and return a list of calibrated dates
    CalDateList my_cal_date_list = my_date_list.calibrate(my_cal_curve);

    // If requested calculate the sum probability
    if (vm.count("sum")) {
    my_cal_date_list.sum();
    }

    // If requested, and output format is not csv, 
    // calculate the sigma ranges
    if (calc_sigma_ranges && output_format != "csv") {
      for (auto &this_date : my_cal_date_list._dates) {
        this_date.calculate_sigma_ranges();
      }
    }

    // If output format is set to json, export a json string
    if (output_format == "json") {
      std::cout << my_cal_date_list.to_json() << std::endl;
    } else if (output_format == "csv") {     // If output format is set to csv, export a csv string
      std::cout << my_cal_date_list.to_csv() << std::endl;
    } else {
      cout << "Invalid output format!";
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
