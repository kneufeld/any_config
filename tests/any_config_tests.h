#pragma once

#include <string>

#include "cppunit/extensions/HelperMacros.h"

#define private public
#include "any_config.h"

ANY_CONFIG_KEY( CFG_ID, std::string, 1 );
ANY_CONFIG_KEY( CFG_MAX, double, 2 );
ANY_CONFIG_KEY( CFG_MIN, double, 3 );

class CTestConfig : public any_config
{
public:

    CTestConfig( any_config* parent = NULL ) : any_config( parent ) {};
};

class any_config_tests : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( any_config_tests );
    CPPUNIT_TEST( Creation );
    CPPUNIT_TEST( Keys );
    CPPUNIT_TEST( Deleting );
    CPPUNIT_TEST( Setting );
    CPPUNIT_TEST( Getting );
    CPPUNIT_TEST( Equality );
    CPPUNIT_TEST( Example );
    CPPUNIT_TEST_SUITE_END();

public:

    any_config_tests();
    ~any_config_tests();

    void setUp();       // Setup initial condition before each test run
    void tearDown();    // Cleanup data after each test run

private:

    void Creation();
    void Setting();
    void Getting();
    void Types();
    void Deleting();
    void Keys();
    void Equality();
    void Example();

    CTestConfig m_parent;
    CTestConfig m_config;

    double m_delta;     // maximum difference for double equality
};
