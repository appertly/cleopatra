<?hh // strict
/**
 * Cleopatra
 *
 * @copyright 2016 Appertly contributors
 * @license   MIT
 */
namespace Cleopatra;

/**
 * A parsed command
 *
 * @copyright 2016 Appertly contributors
 * @license   MIT
 */
class Command
{
    /**
     * Creates a new Command
     *
     * @param $program - The program
     * @param $options - The parsed options
     * @param $arguments - The parsed arguments
     */
    public function __construct(private string $program, private ImmMap<string,mixed> $options, private ImmVector<string> $arguments)
    {
    }

    /**
     * Gets the program
     *
     * @return - The program
     */
    public function getProgram(): string
    {
        return $this->program;
    }

    /**
     * Gets the options
     *
     * @return - The options
     */
    public function getOptions(): ImmMap<string,mixed>
    {
        return $this->options;
    }

    /**
     * Gets the arguments
     *
     * @return - The arguments
     */
    public function getArguments(): ImmVector<string>
    {
        return $this->arguments;
    }
}
