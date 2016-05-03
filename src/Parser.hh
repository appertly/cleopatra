<?hh // strict
/**
 * Cleopatra
 *
 * @copyright 2016 Appertly contributors
 * @license   http://opensource.org/licenses/MIT MIT License
 */
namespace Cleopatra;

/**
 * A parser of CLI options
 *
 * @copyright 2016 Appertly contributors
 * @license   http://opensource.org/licenses/MIT MIT License
 */
class Parser
{
    /**
     * Creates a new Parser
     *
     * @param $options - The options to parse
     */
    public function __construct(protected OptionSet $options)
    {
    }

    /**
     * Parses a list of arguments into a proper CLI command
     *
     * @param $arguments - The arguments
     * @return A parsed command
     * @throws \InvalidArgumentException if the arguments parameter is empty
     * @throws \UnexpectedValueException if an unknown option is used or a required value is not supplied
     */
    public function parse(Traversable<string> $arguments): Command
    {
        if (count($arguments) === 0) {
            throw new \InvalidArgumentException("Arguments parameter must not be empty");
        }
        $args = new Vector($arguments);
        $args->reverse();
        $program = $args->pop();
        $options = Map{};
        $arguments = Vector{};
        while (!$args->isEmpty()) {
            $arg = $args->pop();
            if ($arg == '--') {
                $args->reverse();
                $arguments->addAll($args);
                $args->clear();
            } elseif (substr($arg, 0, 2) === '--') {
                $this->parseLong(substr($arg, 2), $args, $options);
            } elseif (substr($arg, 0, 1) === '-') {
                $this->parseShort(substr($arg, 1), $args, $options);
            } else {
                $arguments[] = $arg;
            }
        }
        return new Command($program, $options->immutable(), $arguments->immutable());
    }

    /**
     * Safely adds an option to the map, taking into account multiplicity.
     *
     * @param $label - The label
     * @param $value - The value
     * @param $option - The Option object
     * @param $options - The map to store values
     */
    protected function addOption(string $label, mixed $value, Option $option, Map<string,mixed> $options): void
    {
        if ($option->isMultiple()) {
            if ($options->containsKey($label)) {
                /* HH_IGNORE_ERROR[4006]: We know this is a Vector */
                $options[$label][] = $option->parse($value);
            } else {
                $options[$label] = Vector{$option->parse($value)};
            }
        } else {
            $options[$label] = $option->parse($value);
        }
    }

    /**
     * Parses a short option (e.g. `-d`)
     *
     * @param $arg - The string argument
     * @param $args - All remaining arguments
     * @param $options - The map to store values
     */
    protected function parseShort(string $arg, Vector<string> $args, Map<string,mixed> $options): void
    {
        $label = substr($arg, 0, 1);
        $remainder = strlen($arg) > 1 ? substr($arg, 1) : '';
        $value = null;
        $option = $this->options->getOption($label);
        if ($option === null) {
            throw new \UnexpectedValueException("Unknown option: -$label");
        } elseif ($option->isIncremental()) {
            $val = (int)$options->get($label);
            $value = $val + 1;
        }
        if (strlen($arg) == 1) {
            if ($option->isRequired()) {
                $next = $args->lastValue();
                if ($next === null || substr($next, 0, 1) === '-') {
                    throw new \UnexpectedValueException("Option -$label expects a value");
                }
                $value = $args->pop();
            }
        } else {
            if ($option->isRequired() || $option->isOptional()) {
                $value = $remainder;
                $remainder = '';
            }
        }
        $this->addOption($label, $value, $option, $options);
        if (strlen($remainder) > 0) {
            $this->parseShort($remainder, $args, $options);
        }
    }

    /**
     * Parses a long option (e.g. `--dir`)
     *
     * @param $arg - The string argument
     * @param $args - All remaining arguments
     * @param $options - The map to store values
     */
    protected function parseLong(string $arg, Vector<string> $args, Map<string,mixed> $options): void
    {
        if (strpos($arg, '=') !== false) {
            list($label, $value) = array_pad(explode('=', $arg), 2, '');
            $option = $this->options->getOption($label);
            if ($option === null) {
                throw new \UnexpectedValueException("Unknown option: --$label");
            } elseif ($option->isSolo()) {
                throw new \UnexpectedValueException("Option --$label does not take a value");
            }
            $this->addOption($label, $value, $option, $options);
        } else {
            $option = $this->options->getOption($arg);
            $value = null;
            if ($option === null) {
                throw new \UnexpectedValueException("Unknown option: --$arg");
            } elseif ($option->isIncremental()) {
                $val = (int)$options->get($arg);
                $value = $val + 1;
            } elseif ($option->isRequired()) {
                $next = $args->lastValue();
                if ($next === null || substr($next, 0, 1) === '-') {
                    throw new \UnexpectedValueException("Option --$arg expects a value");
                }
                $value = $args->pop();
            } else {
                $value = $option->parse('');
            }
            $this->addOption($arg, $value, $option, $options);
        }
    }
}
