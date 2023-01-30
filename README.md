<img align="right" src="img/dalek2.jpg">

# calibrator

[![Project Status: Active – The project has reached a stable, usable state and is being actively developed.](https://www.repostatus.org/badges/latest/active.svg)](https://www.repostatus.org/#active)

Calibrator is a small c++ command line tool to calibrate 14C dates. In can process inputs in csv, json or as cli parameters, and it outputs csv or json, that can be piped into a file.

## Usage
```
Allowed arguments:
  -h [ --help ]               Produce this help message.
  -i [ --input-file ] arg     Specifies input file.
  -b [ --bp ] arg             The BP Value.
  -s [ --std ] arg            The standard deviation.
  -j [ --json-string ] arg    Input as as JSON string. Format: {"bp": xx, 
                              "std": xx}
  -r [ --ranges ]             calculate sigma ranges (only for json output).
  --sum                       calculate sum probability.
  -o [ --output ] arg (=json) csv for csv-output, json for json (default).
```

## Examples
```{bash}
./calibrator -b 5000 -s 50
./calibrator -i input_test.txt
./calibrator -i input_test2.txt
./calibrator -i input_test3.csv
```

## Documentation

See [doxygen docu](https://isaakiel.github.io/calibrator/).
