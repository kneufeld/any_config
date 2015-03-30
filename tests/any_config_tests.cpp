
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/ui/text/TestRunner.h>

#include "any_config_tests.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( any_config_tests );

any_config_tests::any_config_tests()
    : m_config( &m_parent )
{
    m_delta = 10e-11;
}

any_config_tests::~any_config_tests()
{
}

void any_config_tests::setUp()
{
    m_parent.set<CFG_ID>( string( "parent" ) );
    m_config.set<CFG_ID>( string( "config" ) );
}

void any_config_tests::tearDown()
{
    m_parent.m_map.clear();
    m_config.m_map.clear();
}

void any_config_tests::Creation()
{
    // make sure pointer hierarchy is correct
    CPPUNIT_ASSERT( m_parent.m_parent == NULL );
    CPPUNIT_ASSERT( m_config.m_parent == &m_parent );
}

void any_config_tests::Setting()
{
    // set config, make sure you get the correct value and you haven't changed the parent
    m_config.set<CFG_MIN>( 1.0 );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, m_config.get<CFG_MIN>(), m_delta );

    m_parent.set<CFG_MIN>( 10.0 );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, m_config.get<CFG_MIN>(), m_delta );    // still 1
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 10.0, m_parent.get<CFG_MIN>(), m_delta ); // is 10

    // add a new value
    m_config.set<CFG_MAX>( 10.0 );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 10.0, m_config.get<CFG_MAX>(), m_delta );

    // we've added CFG_MAX to child, make sure it's not in the parent
    CPPUNIT_ASSERT_THROW_MESSAGE( "", m_parent.get<CFG_MAX>(), any_config::key_error );
}

void any_config_tests::Getting()
{
    // these are set in the setUp() function
    CPPUNIT_ASSERT( m_parent.get<CFG_ID>() == "parent" );
    CPPUNIT_ASSERT( m_config.get<CFG_ID>() == "config" );

    // make sure we throw an exception for a key that does not exist
    CPPUNIT_ASSERT_THROW_MESSAGE( "", m_parent.get<CFG_MAX>(), any_config::key_error );

    CPPUNIT_ASSERT( ! m_config.has_key<CFG_MAX>() );
    CPPUNIT_ASSERT( m_config.has_key<CFG_ID>() );
}

void any_config_tests::Deleting()
{
    CPPUNIT_ASSERT( m_parent.has_key<CFG_ID>() );    // make sure ID keys are present
    CPPUNIT_ASSERT( m_config.has_key<CFG_ID>() );

    CPPUNIT_ASSERT( m_config.erase<CFG_ID>() );        // remove ID from config
    CPPUNIT_ASSERT( ! m_config.has_key<CFG_ID>( false ) );    // make sure it's gone (don't recurse, parent has one)
    CPPUNIT_ASSERT( m_parent.has_key<CFG_ID>() );        // make sure parent still has an ID

    CPPUNIT_ASSERT( ! m_config.erase<CFG_ID>() );    // already gone, should fail
}

void any_config_tests::Keys()
{
    // ID is already set
    CPPUNIT_ASSERT( m_config.has_key<CFG_ID>() );
    CPPUNIT_ASSERT( m_parent.has_key<CFG_ID>() );

    CPPUNIT_ASSERT( ! m_config.has_key<CFG_MIN>() );
    m_config.set<CFG_MIN>( 1.0 );
    CPPUNIT_ASSERT( m_config.has_key<CFG_MIN>() );

    m_parent.set<CFG_MAX>( 10.0 );
    CPPUNIT_ASSERT( m_config.has_key<CFG_MAX>() );            // recurse by default
    CPPUNIT_ASSERT( m_config.has_key<CFG_MAX>( true ) );        // recurse explicitly
    CPPUNIT_ASSERT( ! m_config.has_key<CFG_MAX>( false ) );    // don't recurse, don't find

    // make sure the number of keys we get equal number of elems in config
    any_config::key_set keys = m_config.keys();
    CPPUNIT_ASSERT( m_config.m_map.size() == 2 );    // ID & MIN (MAX in parent)

    m_config.clear();
    CPPUNIT_ASSERT( m_config.m_map.size() == 0 );
    CPPUNIT_ASSERT( m_parent.m_map.size() == 2 );    // parent should still have its keys

    keys = m_config.keys();
    CPPUNIT_ASSERT( keys.size() == 2 );                // the parents ID & MAX

    m_parent.set<CFG_MIN>( 0.0 );
    m_config.set<CFG_ID>( "config" );
    keys = m_config.keys();
    CPPUNIT_ASSERT( keys.size() == 3 );                // IDs get merged, parent.MAX and config.MIN == 3

    CPPUNIT_ASSERT( keys.find( CFG_ID::key ) != keys.end() );
}

void any_config_tests::Equality()
{
    CPPUNIT_ASSERT( m_parent == m_config );            // both have ids

    m_config.set<CFG_MIN>( 1.0 );
    CPPUNIT_ASSERT( m_parent != m_config );
}

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

    // note: we didn't set make on jetta
    cout << endl << "make: " << jetta.get<Make>() << ", model: " << jetta.get<Model>() << ", year: " << jetta.get<Year>() << endl;
}

int main( int argc, char** argv )
{
    // informs test-listener about testresults
    CppUnit::TestResult controller;

    // register listener for collecting the test-results
    CppUnit::TestResultCollector result;
    controller.addListener( &result );

    // register listener for per-test progress output
    CppUnit::BriefTestProgressListener progress;
    controller.addListener( &progress );

    // insert test-suite at test-runner by registry
    CppUnit::TestRunner testrunner;
    //testrunner.addTest (CppUnit::TestFactoryRegistry::getRegistry().makeTest() );
    testrunner.addTest( any_config_tests::suite() );
    testrunner.run( controller );

    // output results in compiler-format
    CppUnit::CompilerOutputter compileroutputter( &result, std::cerr );
    compileroutputter.write();
}


