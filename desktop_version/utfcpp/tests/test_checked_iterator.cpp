#include "gtest/gtest.h"
#include "utf8.h"

using namespace utf8;


TEST(CheckedIteratrTests, test_increment)
{
    const char* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    utf8::iterator<const char*> it(threechars, threechars, threechars + 9);
    utf8::iterator<const char*> it2 = it;
    EXPECT_EQ (it2, it);
    EXPECT_EQ (*it, 0x10346);
    EXPECT_EQ (*(++it), 0x65e5);
    EXPECT_EQ ((*it++), 0x65e5);
    EXPECT_EQ (*it, 0x0448);
    EXPECT_NE (it, it2);
    utf8::iterator<const char*> endit (threechars + 9, threechars, threechars + 9);
    EXPECT_EQ (++it, endit);
}

TEST(CheckedIteratrTests, test_decrement)
{
    const char* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    utf8::iterator<const char*> it(threechars+9, threechars, threechars + 9);
    EXPECT_EQ (*(--it), 0x0448);
    EXPECT_EQ ((*it--), 0x0448);
    EXPECT_EQ (*it, 0x65e5);
    EXPECT_EQ (--it, utf8::iterator<const char*>(threechars, threechars, threechars + 9));
    EXPECT_EQ (*it, 0x10346);
}
