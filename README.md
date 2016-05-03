# cleopatra
A command line argument parser better than getopt for Hack/HHVM.

It got its name because it parses *CLI* *opt* ions.

## Installation

You can install this library using Composer:

```console
$ composer require appertly/cleopatra
```

* The master branch (version 0.x) of this project requires HHVM 3.12 and has no dependencies.

## Compliance

Releases of this library will conform to [Semantic Versioning](http://semver.org).

Our code is intended to comply with [PSR-1](http://www.php-fig.org/psr/psr-1/), [PSR-2](http://www.php-fig.org/psr/psr-2/), and [PSR-4](http://www.php-fig.org/psr/psr-4/). If you find any issues related to standards compliance, please send a pull request!

## Features

Users can specify command line options pretty much how you'd expect from typical shell conventions.
* Short options
  * Multiple short options can be bundled together (e.g. `-abcdef`)
  * Short options with required values can be specified either with a space or without (e.g. `-d2` or `-d 2`)
  * Short options with optional values must be specified without a space (e.g. `-d2`)
  * Short options without values can be bundled with those with values (e.g. `-abcd 2`)
* Long options
  * Long options with required values can be specified either with an equals sign or a space (e.g. `--foo bar` or `--foo="bar"`)
  * Long options with optional values must be specified with an equals sign (e.g. `--foo` or `--foo="bar"`)
* Normal arguments
  * Options can be specified in any order among regular arguments (e.g. `command -a value -b argument1 --opt="value" -c argument2 argument3`)
    * A real-world example of this: `aws --profile mine ec2 start-instances --instance-ids i-123456`
  * Arguments can be separated from all options with a double dash (e.g. `command -a value -xyz -- argument1 argument2 argument3`)
* Automatic help documentation

## Usage

### Specification

For simplicity, we stuck to a combination of
[PHP's own `getopt` function](http://php.net/manual/en/function.getopt.php) and
[Perl's Getopt::Long](http://perldoc.perl.org/Getopt/Long.html).

`[label][constraints][type]`

* The label contains aliases for the option, separated with a pipe (`|`), e.g. `l|length`
  * Labels must only consist of alphanumeric characters and the dash (e.g. `[a-zA-Z0-9\-]`)
* The constraint dictates how the option can be used
  * A plus (`+`) marks an option as incremental (e.g. `-vvv`)
  * A colon (`:`) marks an option as having a required value
  * Two colons (`::`) marks an option as non-required; a value can be specified with equals (e.g `--option="foo"`)
* The type can be used to specify how the argument value is evaluated
  * `s` = string, `i` = integer, `f` = float, `d` = `DateTimeImmutable`
  * A trailing at sign (`@`) will store multiple arguments in a `Vector` (e.g. `-d 1.txt -d 2.txt -d 3.txt`)

#### Examples

```
l|length    = Boolean parameter with no argument (-l or --length)
v|verbose+  = Incremental option; value increased each time used
d|dir:      = Option with required value (-d stuff, --dir stuff)
d|dir::     = Option with optional value (--dir, --dir="stuff")
f|file:@    = Required option with multiple values (-f a.txt -f b.txt -f c.txt)
dir:s       = Required option will be evaluated as a string
dir::i      = Optional option will be evaluated as an integer
dir:f       = Option will be evaluated as a float
dir:d       = Option will be evaluated as a `DateTimeImmutable`
dir:s@      = Multiple options will result in a `Vector<string>`
```

### Samples

Here's a quick example.

The contents of test.hh:
```hack
<?hh
require "vendor/autoload.php";

use Cleopatra\Parser;
use Cleopatra\OptionSet;
use Cleopatra\Option;

$optionSet = new OptionSet(
    new Option("help", "Display this help"),
    new Option("version", "Shows version information"),
    new Option("v|verbose+", "Enables verbose output; use multiple times to increase verbosity"),
    new Option("e|exclude:s@", "Excludes files and folders from processing"),
    new Option("nice:i", "Sets the process nice value"),
    new Option("profile:", "Specifies which profile to use"),
    new Option("q|quiet", "Disables all output to stdout"),
    new Option("x|experimental", "Enables experimental features"),
    new Option("log::", "Enables log output; default is syslog, but you can specify a log filename")
);
$parser = new Parser($optionSet);
$cmd = $parser->parse($_SERVER['argv']);
$options = $cmd->getOptions(); // ImmMap<string,mixed>
$arguments = $cmd->getArguments(); // ImmVector<string>
if ($arguments->isEmpty() || $options->containsKey('help')) {
    echo $optionSet->getHelp(), PHP_EOL;
} else {
    echo "You executed: " . $cmd->getProgram(), PHP_EOL;
    echo "With options: " . json_encode($options), PHP_EOL;
    echo "With arguments: " . implode(", ", $arguments), PHP_EOL;
}
```
You run:
```bash
hhvm test.hh -qe foo -e bar --nice 123 run-tests -vvv -v --log=errors.log -v -xebaz src
```
You get:
```
You executed: test.hh
With options: {"q":true,"e":["foo","bar","baz"],"nice":123,"v":5,"log":"errors.log","x":true}
With arguments: run-tests, src
```
You run:
```bash
hhvm test.hh --help
```
You get:
```
  --help             Display this help
  --version          Shows version information
  -v --verbose       Enables verbose output; use multiple times to increase
                     verbosity
  -e --exclude       Excludes files and folders from processing
  --nice             Sets the process nice value
  --profile          Specifies which profile to use
  -q --quiet         Disables all output to stdout
  -x --experimental  Enables experimental features
  --log              Enables log output; default is syslog, but you can specify
                     a log filename
```
