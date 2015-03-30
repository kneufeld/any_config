
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
