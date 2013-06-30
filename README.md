any_config
=========

A template class that follows the prototype pattern.

http://steve-yegge.blogspot.com/2008/10/universal-design-pattern.html
http://wiki.aqsis.org/dev/template_tricks

The original use case was to configure a plot (think gnuplot or excel chart). The software
itself required some properties to be set in every plot so there was a global
plot config object. But different modules had their own requirements (for example
the default background color) so they overrode those items and set the parent to the
global. But then companies and users had their own preferences so they overrode the
values and pointed back to the module defaults.

You can think of this class as a linked list of std::map. When you try to get
a value from an instance, you get that instances value or the request is propogated
up to the the parent. The value is returned or an exeception is thrown if there is
no matching key found.

Based on key/value tuples, key is an integer (enum) and the value is any type you
want it to be. Inside the map the value is a boost::any which is a fancy class
that can hold any type of object. We then cast boost::any to the 'real' type on get<type>(key).

The beauty of this class is that all gets/sets are type safe and validated at compile time.

The downside is that arbitrary types can't be added to the map, a special struct must be
created first, the typedef is what determines the type of the value. Use ANY_CONFIG_KEY to
easily create new key types.

Another downside is that you the developer must manually give each key type a unique number,
the preprocessor could do this for us but that could have unfortunate side effects if the object
is ever serialized and then the numbers change. Explicit is better than implicit.

Despite these limitations, any_config can make some difficult problems trivial to solve.

ps. The makefiles are pretty good examples for other projects

Example
=======

Note: you probably want to put all your types in a namespace or in a parent class but that is not required.

Taken from tests/any_config_tests.cpp, any_config_tests::Example():

namespace car_example
{
	ANY_CONFIG_KEY( Year, int, 1 );
	ANY_CONFIG_KEY( Make, std::string, 2 );
	ANY_CONFIG_KEY( Model, std::string, 3 );
}

void any_config_tests::Example()
{
	using namespace std;
	using namespace car_example;

	any_config car;
	car.set<Make>( "VW" );

	any_config jetta( &car );
	jetta.set<Model>( "Jetta GLI" );
	jetta.set<Year>( 2013 );

	// note: we didn't set Make on jetta but the correct value is returned
	cout << endl << "make: " << jetta.get<Make>() << ", model: " << jetta.get<Model>() << ", year: " << jetta.get<Year>() << endl;
}

