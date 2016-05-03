<?hh

namespace Cleopatra;

use HackPack\HackUnit\Contract\Assert;

class OptionSetTests
{
    <<Test>>
    public async function testHelp(Assert $assert): Awaitable<void>
    {
        $set = new OptionSet(
            new Option("d|directory", "This is the name of the directory you wish to be parsed"),
            new Option("v|verbose+", "Enable verbose mode. Use multiple times for more verbosity."),
            new Option("l|length|size", "Specify the number of widgets"),
            new Option("1", "Output to stdout"),
            new Option("2", "Output to stderr"),
        );
        $out =  "  -d --directory      This is the name of the directory you wish to be parsed" . PHP_EOL .
                "  -v --verbose        Enable verbose mode. Use multiple times for more" . PHP_EOL .
                "                      verbosity." . PHP_EOL .
                "  -l --length --size  Specify the number of widgets" . PHP_EOL .
                "  -1                  Output to stdout" . PHP_EOL .
                "  -2                  Output to stderr" . PHP_EOL;
        $assert->string($set->getHelp())->is($out);
    }

    <<Test>>
    public async function testEmptyConstruct(Assert $assert): Awaitable<void>
    {
        $assert->whenCalled(() ==> {new OptionSet();})
            ->willThrowClassWithMessage(\InvalidArgumentException::class,
                'You must provide at least one option');
    }

    <<Test>>
    public async function testCombine1(Assert $assert): Awaitable<void>
    {
        $set = new OptionSet(
            new Option("d|directory", "This is the name of the directory you wish to be parsed"),
            new Option("v|verbose+", "Enable verbose mode. Use multiple times for more verbosity.")
        );
        $other = new OptionSet(
            new Option("l|length|size", "Specify the number of widgets"),
            new Option("1", "Output to stdout"),
            new Option("2", "Output to stderr"),
        );
        $out = $set->combine($other)->getOptions();
        $assert->container($out)->containsAll($set->getOptions());
        $assert->container($out)->containsAll($other->getOptions());
        $assert->mixed($set->combine($set))->identicalTo($set);
    }

    <<Test>>
    public async function testCombine2(Assert $assert): Awaitable<void>
    {
        $set = new OptionSet(
            new Option("d|directory", "This is the name of the directory you wish to be parsed"),
            new Option("v|verbose+", "Enable verbose mode. Use multiple times for more verbosity.")
        );
        $other = new OptionSet(
            new Option('v|volume', "Which volume to use")
        );
        $assert->whenCalled(() ==> {$set->combine($other);})
            ->willThrowClassWithMessage(\InvalidArgumentException::class,
                'Duplicate option: -v');
    }

    <<Test>>
    public async function testCombine3(Assert $assert): Awaitable<void>
    {
        $set = new OptionSet(
            new Option("d|directory", "This is the name of the directory you wish to be parsed"),
            new Option("v|verbose+", "Enable verbose mode. Use multiple times for more verbosity.")
        );
        $other = new OptionSet(
            new Option('x|verbose', "Which volume to use")
        );
        $assert->whenCalled(() ==> {$set->combine($other);})
            ->willThrowClassWithMessage(\InvalidArgumentException::class,
                'Duplicate option: --verbose');
    }
}
