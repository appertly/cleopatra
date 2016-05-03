<?hh // strict
/**
 * Cleopatra
 *
 * @copyright 2016 Appertly contributors
 * @license   http://opensource.org/licenses/MIT MIT License
 */
namespace Cleopatra;

/**
 * A set of options.
 *
 * @copyright 2016 Appertly contributors
 * @license   http://opensource.org/licenses/MIT MIT License
 */
class OptionSet
{
    private ImmMap<string,Option> $optionsByLabel;
    private ImmVector<Option> $options;

    /**
     * Creates a new OptionSet.
     *
     * @param $options - The options to add
     * @throws \InvalidArgumentException if any option labels collide
     */
    public function __construct(Option ...$options)
    {
        if (count($options) === 0) {
            throw new \InvalidArgumentException("You must provide at least one option");
        }
        $byLabel = Map{};
        foreach ($options as $option) {
            foreach ($option->getShorts() as $o) {
                if ($byLabel->containsKey($o)) {
                    throw new \InvalidArgumentException("Duplicate option: -$o");
                }
                $byLabel[$o] = $option;
            }
            foreach ($option->getLongs() as $o) {
                if ($byLabel->contains($o)) {
                    throw new \InvalidArgumentException("Duplicate option: --$o");
                }
                $byLabel[$o] = $option;
            }
        }
        $this->options = new ImmVector($options);
        $this->optionsByLabel = $byLabel->immutable();
    }

    /**
     * Gets an Option by label.
     *
     * @param $label - The label
     * @return The option found or null
     */
    public function getOption(string $label): ?Option
    {
        return $this->optionsByLabel->get($label);
    }

    /**
     * Gets the options.
     *
     * @return The options
     */
    public function getOptions(): ImmVector<Option>
    {
        return $this->options;
    }

    /**
     * Combines this OptionSet with another.
     *
     * @param $other - The other options
     * @return The new combined option set
     * @throws \InvalidArgumentException if any option labels collide
     */
    public function combine(OptionSet $other): OptionSet
    {
        if ($this === $other) {
            return $this;
        }
        $options = new Vector($this->options);
        $options->addAll($other->options);
        return new OptionSet(...$options);
    }

    /**
     * Gets a formatted help string for the option set.
     *
     * @return a formatted help string
     */
    public function getHelp(): string
    {
        $opts = Map{};
        $longest = 0;
        foreach ($this->options as $option) {
            $keys = Set{};
            $keys->addAll($option->getShorts()->map($a ==> "-$a"));
            $keys->addAll($option->getLongs()->map($a ==> "--$a"));
            $key = '  ' . implode(" ", $keys) . '  ';
            $opts[$key] = $option->getDescription();
            $longest = max($longest, strlen($key));
        }
        $out = '';
        foreach ($opts as $k => $v) {
            $out .= str_pad($k, $longest) .
                str_replace(PHP_EOL,
                    str_pad(PHP_EOL, $longest + 1),
                    wordwrap($v, 79 - $longest, PHP_EOL)
                ) . PHP_EOL;
        }
        return $out;
    }
}
