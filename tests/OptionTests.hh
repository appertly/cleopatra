<?hh

namespace Cleopatra;

use HackPack\HackUnit\Contract\Assert;

class OptionTests
{
    <<Test>>
    public async function testConstruct01(Assert $assert): Awaitable<void>
    {
        $description = 'Increase verbosity';
        $option = new Option('v|verbose', $description);
        $assert->mixed($option->getShorts())->looselyEquals(ImmSet{'v'});
        $assert->mixed($option->getLongs())->looselyEquals(ImmSet{'verbose'});
        $assert->string($option->getDescription())->is($description);
        $assert->bool($option->isRequired())->is(false);
        $assert->bool($option->isOptional())->is(false);
        $assert->bool($option->isSolo())->is(true);
        $assert->bool($option->isIncremental())->is(false);
        $assert->bool($option->isMultiple())->is(false);
    }

    <<Test>>
    public async function testConstruct02(Assert $assert): Awaitable<void>
    {
        $description = 'Increase verbosity';
        $option = new Option('v|verb|verbose+', $description);
        $assert->mixed($option->getShorts())->looselyEquals(ImmSet{'v'});
        $assert->mixed($option->getLongs())->looselyEquals(ImmSet{'verb', 'verbose'});
        $assert->string($option->getDescription())->is($description);
        $assert->bool($option->isRequired())->is(false);
        $assert->bool($option->isOptional())->is(false);
        $assert->bool($option->isSolo())->is(true);
        $assert->bool($option->isIncremental())->is(true);
        $assert->bool($option->isMultiple())->is(false);
        $assert->string($option->getType())->is('i');
        $assert->mixed($option->parse('123'))->looselyEquals(123);
    }

    <<Test>>
    public async function testConstruct03(Assert $assert): Awaitable<void>
    {
        $description = 'Date of thing';
        $option = new Option('d:d', $description);
        $assert->mixed($option->getShorts())->looselyEquals(ImmSet{'d'});
        $assert->mixed($option->getLongs())->looselyEquals(ImmSet{});
        $assert->string($option->getDescription())->is($description);
        $assert->bool($option->isRequired())->is(true);
        $assert->bool($option->isOptional())->is(false);
        $assert->bool($option->isSolo())->is(false);
        $assert->bool($option->isIncremental())->is(false);
        $assert->bool($option->isMultiple())->is(false);
        $assert->string($option->getType())->is('d');
        $assert->mixed($option->parse('2014-04-01'))->looselyEquals(new \DateTimeImmutable('2014-04-01'));
    }

    <<Test>>
    public async function testConstruct04(Assert $assert): Awaitable<void>
    {
        $description = 'Something good';
        $option = new Option('9|nine::', $description);
        $assert->mixed($option->getShorts())->looselyEquals(ImmSet{'9'});
        $assert->mixed($option->getLongs())->looselyEquals(ImmSet{'nine'});
        $assert->string($option->getDescription())->is($description);
        $assert->bool($option->isRequired())->is(false);
        $assert->bool($option->isOptional())->is(true);
        $assert->bool($option->isSolo())->is(false);
        $assert->bool($option->isIncremental())->is(false);
        $assert->bool($option->isMultiple())->is(false);
    }

    <<Test>>
    public async function testConstruct05(Assert $assert): Awaitable<void>
    {
        $description = 'Something good';
        $option = new Option('d|directory:@', $description);
        $assert->mixed($option->getShorts())->looselyEquals(ImmSet{'d'});
        $assert->mixed($option->getLongs())->looselyEquals(ImmSet{'directory'});
        $assert->string($option->getDescription())->is($description);
        $assert->bool($option->isRequired())->is(true);
        $assert->bool($option->isOptional())->is(false);
        $assert->bool($option->isSolo())->is(false);
        $assert->bool($option->isIncremental())->is(false);
        $assert->bool($option->isMultiple())->is(true);
        $assert->string($option->getType())->is('s');
        $assert->mixed($option->parse('foo'))->looselyEquals('foo');
    }

    <<Test>>
    public async function testConstruct06(Assert $assert): Awaitable<void>
    {
        $description = 'A big count';
        $option = new Option('c|count:f@', $description);
        $assert->mixed($option->getShorts())->looselyEquals(ImmSet{'c'});
        $assert->mixed($option->getLongs())->looselyEquals(ImmSet{'count'});
        $assert->string($option->getDescription())->is($description);
        $assert->bool($option->isRequired())->is(true);
        $assert->bool($option->isOptional())->is(false);
        $assert->bool($option->isSolo())->is(false);
        $assert->bool($option->isIncremental())->is(false);
        $assert->bool($option->isMultiple())->is(true);
        $assert->string($option->getType())->is('f');
        $assert->mixed($option->parse('1234.56'))->looselyEquals(1234.56);
    }

    <<Test>>
    public async function testConstruct07(Assert $assert): Awaitable<void>
    {
        $bads = Vector{'-', 'aoeu_htns', '-abcd', ''};
        foreach ($bads as $bad) {
            $assert->whenCalled(() ==> {new Option($bad, '');})
                ->willThrowClassWithMessage(\InvalidArgumentException::class,
                    'Option labels must only contain letters, numbers, and the dash');
        }
    }
}
