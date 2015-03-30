any_config
=========

A template class that follows the prototype pattern.

* http://steve-yegge.blogspot.com/2008/10/universal-design-pattern.html
* http://wiki.aqsis.org/dev/template_tricks

The original use case was to configure a plot (think gnuplot or excel
chart). The software itself required some properties to be set in every
plot so there was a global plot config object. But different modules
had their own requirements (for example the default background color)
so they overrode those items and set the parent to the global. But then
companies and users had their own preferences so they overrode the
values and pointed back to the module defaults.

You can think of this class as a linked list of std::map. When you try
to get a value from an instance, you get that instances value or the
request is propogated up to the the parent. The value is returned or an
exeception is thrown if there is no matching key found.

Based on key/value tuples, key is an integer (enum) and the value is
any type you want it to be. Inside the map the value is a boost::any
which is a fancy class that can hold any type of object. We then cast
boost::any to the 'real' type on get<type>(key).

The beauty of this class is that all gets/sets are type safe and
validated at compile time.

The downside is that arbitrary types can't be added to the map, a
special struct must be created first, the typedef is what determines the
type of the value. Use ANY_CONFIG_KEY to easily create new key types.

Another downside is that you the developer must manually give each key
type a unique number, the preprocessor could do this for us but that
could have unfortunate side effects if the object is ever serialized and
then the numbers change. Explicit is better than implicit.

Despite these limitations, any_config can make some difficult problems
trivial to solve.

ps. The makefiles are pretty good examples for other projects

Dependencies
============

any_config depends on boost and the tests depend on cppunit.

Homebrew
--------

	brew install cppunit boost

Fedora
------

	yum install boost-devel cppunit-devel


Building
========

Building has been rudimentarily tested on OSX and Linux (Fedora).

`make` and `make test` should do what you expect. There is no `make
install` as you'll probably just want to copy the include file into any
project that may use them.

Note: you may need to edit the Makefile(s) INC_DIR and LIB_DIR to find
your installation of cppunit or other.

Example
=======

Note: you probably want to put all your types in a namespace or in a
parent class but that is not required.

Taken from `example/main.cpp`

```cpp
#include <iostream>

#include "any_config.h"

namespace car
{
  ANY_CONFIG_KEY( Year, int, 1 );
  ANY_CONFIG_KEY( Make, std::string, 2 );
  ANY_CONFIG_KEY( Model, std::string, 3 );
}

int main( int, char** )
{
  using namespace std;

  any_config company;
  company.set<car::Make>( "VW" );

  any_config jetta( &company );
  jetta.set<car::Model>( "Jetta GLI" );
  jetta.set<car::Year>( 2013 );

  any_config golf( &company );
  golf.set<car::Model>( "Golf" );
  golf.set<car::Year>( 2010 );

  // note: we didn't set Make on jetta but the correct value is returned
  cout << "make: " << jetta.get<car::Make>() << ", model: "
      << jetta.get<car::Model>() << ", year: " << jetta.get<car::Year>() << endl;

  cout << "make: " << golf.get<car::Make>() << ", model: "
      << golf.get<car::Model>() << ", year: " << golf.get<car::Year>() << endl;

  return 0;
}
```

```
make: VW, model: Jetta GLI, year: 2013
make: VW, model: Golf, year: 2010
```

