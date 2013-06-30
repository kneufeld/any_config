#pragma once

#include <map>
#include <vector>
#include <set>

#include <string>
using std::string;

#include "boost/any.hpp"
using boost::any_cast;

#define ANY_CONFIG_KEY( _key_name, _key_type, _key_value ) \
struct _key_name		{ typedef _key_type type;	static const any_config::key_type key; }; \
const any_config::key_type _key_name::key		= _key_value;

/************************************************************************
	Exceptions for the any_config class
************************************************************************/
class any_config_exception
{
public:

	any_config_exception( string msg ) : m_msg( msg ) {};
	string m_msg;
};

class any_config_key_error : public any_config_exception
{
public:
	any_config_key_error( string msg ) : any_config_exception( msg ) {};
};

class any_config_cast_error : public any_config_exception
{
public:
	any_config_cast_error( string msg ) : any_config_exception( msg ) {};
};


/************************************************************************
	Class that holds configuration information

	Based on prototype pattern:
	http://steve-yegge.blogspot.com/2008/10/universal-design-pattern.html
	http://wiki.aqsis.org/dev/template_tricks

	Based on key/value tuples, key is an integer (enum) and the value
	is any type you want it to be. Inside the map the value is a
	boost::any which is a fancy class that can hold any type of object.
	We then cast boost::any to the 'real' type on get<type>(key).

	Example to prototype a car:
	struct Year		{ typedef int type;				static const any_config::key_type key; };
	const any_config::key_type Year::key = 1;
	// or use helper macro do accomplish the same thing
	ANY_CONFIG_KEY( Make, std::string, 2 );

	any_config car;
	car.set<Year>( 23 );
	car.set<Make>( "VW" );
	string make = car.get<Make>( "VW" );

	see tests directory for more examples
************************************************************************/
class any_config
{
	friend class any_config_tests;

public:

	typedef int			key_type;
	typedef boost::any	val_type;

	typedef std::map< key_type, val_type>	config_map;
	typedef	std::vector< key_type >			key_array;
	typedef	std::set< key_type >			key_set;

	any_config( any_config *parent = NULL );
	~any_config();

	any_config( const any_config& src );				// copy ctor
	any_config& operator=( const any_config& src );

	bool operator==( const any_config& rhs );
	bool operator!=( const any_config& rhs )			{ return ! operator==( rhs ); }

	int	size() const;				// not a recursive call

	key_set keys() const;

	bool erase_all();				// not a recursive call

	void parent( any_config * val )	{ m_parent = val; }
	any_config* parent() const		{ return m_parent; }

	template<typename T>	typename T::type		get() const;
	//template<typename T>	typename T::type&		get();

	template<typename T>	void				set( const typename T::type& val );
	template<typename T>	bool				has_key( bool recurse = true ) const;
	template<typename T>	bool				erase();								// not a recursive call

protected:


private:

	any_config		*m_parent;
	config_map		m_map;
};

/************************************************************************
set a value in the config. const char * must be cast to LPCSTR
eg. config.set<ID>( "config id1" )
************************************************************************/
template<typename T>
void any_config::set( const typename T::type &val )
{
	m_map[ T::key ] = val;
}

/************************************************************************
get a value from the config, you must pass in the return type
eg. config.get<ID>()
************************************************************************/
template<typename T>
typename T::type any_config::get() const
{
	// ret holds a copy of whatever is in the map, so you can't return
	// a reference to whatever it is holding because your reference will
	// point to garbage as the any pointer is destroyed and cleans up its data

	config_map::const_iterator it = m_map.find( T::key );

	if( it != m_map.end() )
	{
		return any_cast<const typename T::type &>( ( *it ).second );
	}

	if( m_parent )
		return m_parent->get<T>();

	// that key does not exist in config chain, check key with has_key() first
	// returning the key isn't too helpful, which struct is key 34?
	throw any_config_key_error( "that key does not exist in config chain" );
}

/************************************************************************
Check to see if config chain has specified key
Could use get() with exception catching to reduce code duplication
but that seems like a kludgey solution for such a simple function.
************************************************************************/
template<typename T>
bool any_config::has_key( bool recurse ) const
{
	if( m_map.count( T::key ) )
		return true;

	if( m_parent && recurse )
		return m_parent->has_key<T>( recurse );

	return false;
}

/************************************************************************
This only deletes the key in this object, it does not delete
the key recursively through any parents. That could be very
dangerous if the parent has many children and those children
are depending on that key being there.

Return true/false if something was deleted. Either way, that key
won't be there.
************************************************************************/
template<typename T>
bool any_config::erase()
{
	if( ! has_key<T>( false ) )
		return false;

	m_map.erase( T::key );

	return true;
}

