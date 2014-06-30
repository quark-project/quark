#include <boost/test/unit_test.hpp>

#include "main.h"

BOOST_AUTO_TEST_SUITE(GetBlockValue_tests)

BOOST_AUTO_TEST_CASE(GetBlockValue_limits)
{
    BOOST_CHECK_EQUAL(_GetBlockValue(1,          0,0), 2048*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(2,          0,0), 2048*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*1-1,  0,0), 2048*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*1,    0,0), 1024*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*2-1,  0,0), 1024*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*2,    0,0),  512*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*3-1,  0,0),  512*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*3,    0,0),  256*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*4-1,  0,0),  256*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*4,    0,0),  128*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*5-1,  0,0),  128*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*5,    0,0),   64*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*6-1,  0,0),   64*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*6,    0,0),   32*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*7-1,  0,0),   32*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*7,    0,0),   16*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*8-1,  0,0),   16*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*8,    0,0),    8*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*9-1,  0,0),    8*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*9,    0,0),    4*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*10-1, 0,0),    4*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*10,   0,0),    2*COIN); 
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*11-1, 0,0),    2*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(60480*11,   0,0),    1*COIN); // 665280
    BOOST_CHECK_EQUAL(_GetBlockValue(665280,     0,0),    1*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(1000000,    0,0),    1*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(2000000,    0,0),    1*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(3870720,    0,0),    1*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(3870721,    0,0),    1*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(4000000,    0,0),    1*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(8000000,    0,0),    1*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(10000000,   0,0),    1*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(20000000,   0,0),    1*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(40000000,   0,0),    1*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(80000000,   0,0),    1*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(100000000,  0,0),    1*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(200000000,  0,0),    1*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(400000000,  0,0),    1*COIN);
    BOOST_CHECK_EQUAL(_GetBlockValue(800000000,  0,0),    1*COIN);
}

BOOST_AUTO_TEST_SUITE_END()
