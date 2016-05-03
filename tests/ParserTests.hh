<?hh

namespace Cleopatra;

use HackPack\HackUnit\Contract\Assert;

class ParserTests
{
    <<Test>>
    public async function testParseEmpty(Assert $assert): Awaitable<void>
    {
        $parser = new Parser(new OptionSet(
            new Option("d|directory", "This is the name of the directory you wish to be parsed")
        ));
        $assert->whenCalled(() ==> {$parser->parse([]);})
            ->willThrowClassWithMessage(\InvalidArgumentException::class,
                'Arguments parameter must not be empty');
    }

    <<Test>>
    public async function testParseOne(Assert $assert): Awaitable<void>
    {
        $parser = new Parser(new OptionSet(
            new Option("d|directory", "This is the name of the directory you wish to be parsed")
        ));
        $cmd = $parser->parse(['test.php']);
        $assert->string($cmd->getProgram())->is('test.php');
        $assert->container($cmd->getOptions())->isEmpty();
        $assert->container($cmd->getArguments())->isEmpty();
    }

    <<Test>>
    public async function testParseTwo(Assert $assert): Awaitable<void>
    {
        $parser = new Parser(new OptionSet(
            new Option("h|help", ""),
            new Option("v|verbose+", ""),
            new Option("e|exclude:s@", ""),
            new Option("nice:i", ""),
            new Option("profile:", ""),
            new Option("q|quiet", ""),
            new Option("x|experimental", ""),
            new Option("log::", "")
        ));
        $args = ['test.php', '-qe', 'foo', '-e', 'bar', '--nice', '123', 'run-tests', '-vvv', '-v', '--log=syslog', '-v', '-xebaz', 'src'];
        $cmd = $parser->parse($args);
        $assert->string($cmd->getProgram())->is('test.php');
        $assert->mixed($cmd->getOptions())->looselyEquals(Map{'q' => true, 'e' => Vector{'foo', 'bar', 'baz'}, 'nice' => 123, 'v' => 5, 'log' => 'syslog', 'x' => true});
        $assert->container($cmd->getArguments())->containsAll(Vector{'run-tests', 'src'});
    }

    <<Test>>
    public async function testParseThree(Assert $assert): Awaitable<void>
    {
        $parser = new Parser(new OptionSet(
            new Option("d|directory", "This is the name of the directory you wish to be parsed")
        ));
        $assert->whenCalled(() ==> {$parser->parse(['test.php', '-x']);})
            ->willThrowClassWithMessage(\UnexpectedValueException::class,
                'Unknown option: -x');
        $assert->whenCalled(() ==> {$parser->parse(['test.php', '--foo']);})
            ->willThrowClassWithMessage(\UnexpectedValueException::class,
                'Unknown option: --foo');
    }

    <<Test>>
    public async function testParseFour(Assert $assert): Awaitable<void>
    {
        $parser = new Parser(new OptionSet(
            new Option("d|directory:", "This is the name of the directory you wish to be parsed")
        ));
        $assert->whenCalled(() ==> {$parser->parse(['test.php', '-d', '--', 'bar']);})
            ->willThrowClassWithMessage(\UnexpectedValueException::class,
                'Option -d expects a value');
        $assert->whenCalled(() ==> {$parser->parse(['test.php', '--directory']);})
            ->willThrowClassWithMessage(\UnexpectedValueException::class,
                'Option --directory expects a value');
    }

    <<Test>>
    public async function testParseFive(Assert $assert): Awaitable<void>
    {
        $parser = new Parser(new OptionSet(
            new Option("v|verbose", "Verbosity")
        ));
        $assert->whenCalled(() ==> {$parser->parse(['test.php', '--verbose=yes']);})
            ->willThrowClassWithMessage(\UnexpectedValueException::class,
                'Option --verbose does not take a value');
    }

    <<Test>>
    public async function testParseSix(Assert $assert): Awaitable<void>
    {
        $parser = new Parser(new OptionSet(
            new Option("v|verbose", "Verbosity")
        ));
        $cmd = $parser->parse(['test.hh', '-v', '--', 'src', 'tests']);
        $assert->string($cmd->getProgram())->is('test.hh');
        $assert->mixed($cmd->getOptions())->looselyEquals(Map{'v' => true});
        $assert->container($cmd->getArguments())->containsAll(Vector{'src', 'tests'});
    }
}
