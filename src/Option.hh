<?hh // strict
/**
 * Cleopatra
 *
 * @copyright 2016 Appertly contributors
 * @license   http://opensource.org/licenses/MIT MIT License
 */
namespace Cleopatra;

/**
 * An option
 *
 * @copyright 2016 Appertly contributors
 * @license   http://opensource.org/licenses/MIT MIT License
 */
class Option
{
    private ImmSet<string> $shorts;
    private ImmSet<string> $longs;
    private string $description;
    private bool $vector;
    private bool $incremental = false;
    private string $type = 's';
    private ?bool $required;

    /**
     * Creates a new Option
     *
     * @param $spec - The option specification
     * @param $description - A human readable description
     */
    public function __construct(string $spec, string $description)
    {
        $this->vector = substr($spec, -1, 1) === '@';
        $aspec = trim($spec, '@');
        $matches = [];
        if (preg_match('/:{1,2}([sifd])$/', $aspec, $matches)) {
            $this->type = $matches[1];
            $aspec = substr($aspec, 0, -1);
        }
        if (substr($aspec, -1, 1) === '+') {
            $this->incremental = true;
            $this->type = 'i';
            $aspec = substr($aspec, 0, -1);
        } elseif (substr($aspec, -2, 2) === '::') {
            $aspec = substr($aspec, 0, -2);
            $this->required = false;
        } elseif (substr($aspec, -1, 1) === ':') {
            $aspec = substr($aspec, 0, -1);
            $this->required = true;
        }
        $shorts = Set{};
        $longs = Set{};
        foreach (explode("|", $aspec) as $v) {
            if (!preg_match('/^[a-zA-Z0-9][a-zA-Z0-9\-]*$/', $v)) {
                throw new \InvalidArgumentException('Option labels must only contain letters, numbers, and the dash');
            } elseif (strlen($v) == 1) {
                $shorts[] = $v;
            } else {
                $longs[] = $v;
            }
        }
        $this->shorts = $shorts->immutable();
        $this->longs = $longs->immutable();
        $this->description = trim($description);
    }

    /**
     * Gets the option description.
     *
     * @return The description
     */
    public function getDescription(): string
    {
        return $this->description;
    }

    /**
     * Gets the option long aliases
     *
     * @return The long aliases
     */
    public function getLongs(): ImmSet<string>
    {
        return $this->longs;
    }

    /**
     * Gets the option short aliases.
     *
     * @return The short aliases
     */
    public function getShorts(): ImmSet<string>
    {
        return $this->shorts;
    }

    /**
     * Gets the option type.
     *
     * @return The type: s, i, f, d
     */
    public function getType() : string
    {
        return $this->type;
    }

    /**
     * Whether the option is incremental
     *
     * @return the option incremental
     */
    public function isIncremental(): bool
    {
        return $this->incremental;
    }

    /**
     * Whether the option requires a value
     *
     * @return true if the option requires a value
     */
    public function isRequired(): bool
    {
        return $this->required === true;
    }

    /**
     * Whether the option can take a value
     *
     * @return true if the option can take a value
     */
    public function isOptional(): bool
    {
        return $this->required === false;
    }

    /**
     * Whether the option takes no value
     *
     * @return true if the option takes no value
     */
    public function isSolo(): bool
    {
        return $this->required === null;
    }

    /**
     * Whether the option can be used multiple times
     *
     * @return true if the option is multiple
     */
    public function isMultiple(): bool
    {
        return $this->vector;
    }

    /**
     * Parses an incoming value based on the type defined
     *
     * @param $value - The incoming value
     * @return The converted value
     */
    public function parse(mixed $value): mixed
    {
        switch ($this->type) {
            case "f":
                return (float)$value;
            case "i":
                return (int)$value;
            case "d":
                return new \DateTimeImmutable($value);
        }
        return $this->required === null ? true : (string)$value;
    }
}
