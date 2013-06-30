
#include <algorithm>
#include <set>
using namespace std;

#include "any_config.h"

// ctor
any_config::any_config( any_config *parent )
{
	m_parent = parent;
}

any_config::~any_config()
{
}

// copy ctor
any_config::any_config( const any_config& src )
{
	m_parent = src.m_parent;
	m_map = src.m_map;
}

any_config& any_config::operator=( const any_config & src )
{
	if( this == &src ) return *this;

	m_parent = src.m_parent;
	m_map = src.m_map;

	return *this;
}

/************************************************************************
	FIXME: this only checks if the keys match and not the values
************************************************************************/
bool any_config::operator==( const any_config& rhs )
{
	if( this == &rhs ) return true;

	// we don't care if the parents don't match, only if the map does
	// if( m_parent != rhs.m_parent ) return false;

	if( m_map.size() != rhs.m_map.size() ) return false;

	for( config_map::const_iterator it = rhs.m_map.begin(); it != rhs.m_map.end(); it++ )
	{
		if( m_map.count( ( *it ).first ) == 0 ) return false;
	}

	return true;
}

int any_config::size() const
{
	return m_map.size();
}

/************************************************************************
	return all the keys, INCLUDING the parents keys
	do not count on key order being the same as insertion order
************************************************************************/
any_config::key_set any_config::keys() const
{
	key_set keys;
	key_set::iterator ks_it;

	for( config_map::const_iterator it = m_map.begin(); it != m_map.end(); it++ )
	{
		keys.insert( ( *it ).first );
	}

	if( m_parent )
	{
		key_set parent_keys = m_parent->keys();
		key_set key_union;

		set_union( keys.begin(), keys.end(), parent_keys.begin(), parent_keys.end(),
		           inserter( key_union, key_union.begin() ) );

		keys.swap( key_union );
	}

	return keys;
}

bool any_config::erase_all()
{
	m_map.clear();
	return size() == 0;
}
