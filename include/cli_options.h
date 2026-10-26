/**
 * @file cli_options.h
 *
 * \brief Minimal command line parser for calibrator.
 *
 * Replaces boost::program_options while reproducing its observable
 * behaviour for the options of calibrator: short and long options,
 * "--opt=value", "-ovalue", sticky short flags ("-rh"), unambiguous
 * prefixes of long options ("--out"), positional input files, "--"
 * as end of options, and boost's error messages.
 */

#ifndef _cli_options_h_
#define _cli_options_h_

#include <string>
#include <vector>

struct CliOptions {
  /**
   * An option with values. `present` mirrors variables_map::count():
   * boost could leave an option present but without value when its first
   * value failed validation.
   */
  template <typename T> struct Multi {
    bool present = false;
    std::vector<T> values;
  };

  bool help = false;
  bool ranges = false;
  bool sum = false;
  Multi<std::string> input_file;
  Multi<std::string> json_string;
  Multi<int> bp;
  Multi<int> std;
  bool has_output = false;
  std::string output;

  /// Non-empty if parsing failed. Options parsed before a value or
  /// duplicate error are kept (as boost::program_options::store did);
  /// after a syntax error nothing is kept.
  std::string error;
};

/// The help text, exactly as boost::program_options formatted it.
extern const char* const CLI_HELP_TEXT;

CliOptions parse_cli_options(int argc, char** argv);

#endif
