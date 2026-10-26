#include "../include/cli_options.h"

#include <cerrno>
#include <climits>
#include <cstdlib>
#include <stdexcept>

const char* const CLI_HELP_TEXT =
  "\n"
  "A tool for 14C calibration from the command line.\n"
  "\n"
  "Allowed arguments:\n"
  "  -h [ --help ]               Produce this help message.\n"
  "  -i [ --input-file ] arg     Specifies input file.\n"
  "  -b [ --bp ] arg             The BP Value.\n"
  "  -s [ --std ] arg            The standard deviation.\n"
  "  -j [ --json-string ] arg    Input as as JSON string. Format: {\"bp\": xx, \n"
  "                              \"std\": xx}\n"
  "  -r [ --ranges ]             calculate sigma ranges (only for json output).\n"
  "  --sum                       calculate sum probability.\n"
  "  -o [ --output ] arg (=json) csv for csv-output, json for json (default).\n";

namespace {

enum class Kind { Flag, Text, Int };

struct OptionDef {
  const char* long_name;
  char short_name;  // 0 if none
  Kind kind;
};

const OptionDef OPTIONS[] = {
  {"help", 'h', Kind::Flag},
  {"input-file", 'i', Kind::Text},
  {"bp", 'b', Kind::Int},
  {"std", 's', Kind::Int},
  {"json-string", 'j', Kind::Text},
  {"ranges", 'r', Kind::Flag},
  {"sum", 0, Kind::Flag},
  {"output", 'o', Kind::Text},
};

struct ParsedOption {
  const OptionDef* def;
  std::string value;
};

struct CliError : std::runtime_error {
  using std::runtime_error::runtime_error;
};

std::string opt_name(const OptionDef* d) {
  return std::string("'--") + d->long_name + "'";
}

const OptionDef* find_short(char c) {
  for (const auto& d : OPTIONS)
    if (d.short_name != 0 && d.short_name == c) return &d;
  return nullptr;
}

bool is_known_short_token(const std::string& tok) {
  return tok.size() == 2 && tok[0] == '-' && find_short(tok[1]) != nullptr;
}

const OptionDef* find_long(const std::string& name, const std::string& token) {
  for (const auto& d : OPTIONS)
    if (name == d.long_name) return &d;
  std::vector<const OptionDef*> matches;
  for (const auto& d : OPTIONS)
    if (std::string(d.long_name).compare(0, name.size(), name) == 0)
      matches.push_back(&d);
  if (matches.empty())
    throw CliError("unrecognised option '" + token + "'");
  if (matches.size() > 1) {
    std::string msg = "option '" + token + "' is ambiguous and matches ";
    for (size_t k = 0; k < matches.size(); k++) {
      if (k > 0) msg += (k + 1 == matches.size()) ? ", and " : ", ";
      msg += opt_name(matches[k]);
    }
    throw CliError(msg);
  }
  return matches[0];
}

// Strict int conversion like boost::lexical_cast<int>.
bool to_int(const std::string& s, int& out) {
  size_t i = 0;
  if (i < s.size() && (s[i] == '+' || s[i] == '-')) i++;
  if (i == s.size()) return false;
  for (size_t k = i; k < s.size(); k++)
    if (s[k] < '0' || s[k] > '9') return false;
  errno = 0;
  long long v = strtoll(s.c_str(), nullptr, 10);
  if (errno == ERANGE || v < INT_MIN || v > INT_MAX) return false;
  out = (int)v;
  return true;
}

// Phase 1: split argv into options (syntax only).
std::vector<ParsedOption> tokenize(int argc, char** argv) {
  static const OptionDef positional = {"input-file", 'i', Kind::Text};
  std::vector<ParsedOption> result;
  bool only_positional = false;

  auto take_next = [&](int& i, const OptionDef* d) -> std::string {
    if (i + 1 < argc && !is_known_short_token(argv[i + 1]))
      return argv[++i];
    throw CliError("the required argument for option " + opt_name(d) + " is missing");
  };

  for (int i = 1; i < argc; i++) {
    const std::string tok = argv[i];
    if (only_positional) {
      result.push_back({&positional, tok});
    } else if (tok == "--") {
      only_positional = true;
    } else if (tok.size() > 2 && tok[0] == '-' && tok[1] == '-' && tok[2] != '=') {
      // long option
      std::string body = tok.substr(2);
      size_t eq = body.find('=');
      std::string name = body.substr(0, eq);
      const OptionDef* d = find_long(name, tok);
      if (d->kind == Kind::Flag) {
        if (eq != std::string::npos)
          throw CliError("option " + opt_name(d) + " does not take any arguments");
        result.push_back({d, ""});
      } else if (eq != std::string::npos) {
        std::string value = body.substr(eq + 1);
        if (value.empty())
          throw CliError("the argument for option " + opt_name(d) +
                         " should follow immediately after the equal sign");
        result.push_back({d, value});
      } else {
        result.push_back({d, take_next(i, d)});
      }
    } else if (tok.size() >= 2 && tok[0] == '-' && tok[1] != '-') {
      // one or more short options ("-r", "-rh", "-b3000", "-rb 3000")
      for (size_t k = 1; k < tok.size(); k++) {
        const OptionDef* d = find_short(tok[k]);
        if (!d) throw CliError("unrecognised option '" + tok + "'");
        if (d->kind == Kind::Flag) {
          result.push_back({d, ""});
          continue;
        }
        std::string rest = tok.substr(k + 1);
        result.push_back({d, rest.empty() ? take_next(i, d) : rest});
        break;
      }
    } else {
      result.push_back({&positional, tok});
    }
  }
  return result;
}

// Phase 2: store the values, validating them in command line order.
void store(const std::vector<ParsedOption>& parsed, CliOptions& o) {
  auto duplicate = [](const OptionDef* d) {
    return CliError("option " + opt_name(d) + " cannot be specified more than once");
  };
  for (const auto& p : parsed) {
    const std::string name = p.def->long_name;
    if (p.def->kind == Kind::Flag) {
      bool& flag = name == "help" ? o.help : name == "ranges" ? o.ranges : o.sum;
      if (flag) throw duplicate(p.def);
      flag = true;
    } else if (name == "output") {
      if (o.has_output) throw duplicate(p.def);
      o.has_output = true;
      o.output = p.value;
    } else if (p.def->kind == Kind::Text) {
      auto& m = name == "input-file" ? o.input_file : o.json_string;
      m.present = true;
      m.values.push_back(p.value);
    } else {
      auto& m = name == "bp" ? o.bp : o.std;
      m.present = true;  // boost created the entry before validating
      int v;
      if (!to_int(p.value, v)) {
        if (p.value.empty())
          throw CliError("the argument for option " + opt_name(p.def) + " is invalid");
        throw CliError("the argument ('" + p.value + "') for option " +
                       opt_name(p.def) + " is invalid");
      }
      m.values.push_back(v);
    }
  }
  // Defaults are applied only if storing succeeded.
  if (!o.has_output) {
    o.has_output = true;
    o.output = "json";
  }
}

}  // namespace

CliOptions parse_cli_options(int argc, char** argv) {
  CliOptions o;
  std::vector<ParsedOption> parsed;
  try {
    parsed = tokenize(argc, argv);
  } catch (const CliError& e) {
    o.error = e.what();
    return o;  // syntax errors: nothing is stored
  }
  try {
    store(parsed, o);
  } catch (const CliError& e) {
    o.error = e.what();
  }
  return o;
}
