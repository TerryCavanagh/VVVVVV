#include "gtest/gtest.h"
#include "utf8/unchecked.h"

#include <string>
#include <vector>
using namespace utf8::unchecked;
using namespace std;

TEST(UnCheckedAPITests, test_append)
{
    unsigned char u[5] = {0,0,0,0,0};
    append(0x0448, u);
    EXPECT_EQ (u[0], 0xd1);
    EXPECT_EQ (u[1], 0x88);
    EXPECT_EQ (u[2], 0);
    EXPECT_EQ (u[3], 0);
    EXPECT_EQ (u[4], 0);

    append(0x65e5, u);
    EXPECT_EQ (u[0], 0xe6);
    EXPECT_EQ (u[1], 0x97);
    EXPECT_EQ (u[2], 0xa5);
    EXPECT_EQ (u[3], 0);
    EXPECT_EQ (u[4], 0);

    append(0x3044, u);
    EXPECT_EQ (u[0], 0xe3);
    EXPECT_EQ (u[1], 0x81);
    EXPECT_EQ (u[2], 0x84);
    EXPECT_EQ (u[3], 0);
    EXPECT_EQ (u[4], 0);

    append(0x10346, u);
    EXPECT_EQ (u[0], 0xf0);
    EXPECT_EQ (u[1], 0x90);
    EXPECT_EQ (u[2], 0x8d);
    EXPECT_EQ (u[3], 0x86);
    EXPECT_EQ (u[4], 0);
}

TEST(UnCheckedAPITests, test_next)
{
    const char* twochars = "\xe6\x97\xa5\xd1\x88";
    const char* w = twochars;
    int cp = utf8::unchecked::next(w);
    EXPECT_EQ (cp, 0x65e5);
    EXPECT_EQ (w, twochars + 3);

    const char* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    w = threechars;

    cp = utf8::unchecked::next(w);
    EXPECT_EQ (cp, 0x10346);
    EXPECT_EQ (w, threechars + 4);

    cp = utf8::unchecked::next(w);
    EXPECT_EQ (cp, 0x65e5);
    EXPECT_EQ (w, threechars + 7);

    cp = utf8::unchecked::next(w);
    EXPECT_EQ (cp, 0x0448);
    EXPECT_EQ (w, threechars + 9);
}

TEST(UnCheckedAPITests, test_peek_next)
{
    const char* const cw = "\xe6\x97\xa5\xd1\x88";
    int cp = peek_next(cw);
    EXPECT_EQ (cp, 0x65e5);
}

TEST(UnCheckedAPITests, test_prior)
{
    const char* twochars = "\xe6\x97\xa5\xd1\x88";
    const char* w = twochars + 3;
    int cp = prior (w);
    EXPECT_EQ (cp, 0x65e5);
    EXPECT_EQ (w, twochars);

    const char* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    w = threechars + 9;
    cp = prior(w);
    EXPECT_EQ (cp, 0x0448);
    EXPECT_EQ (w, threechars + 7);
    cp = prior(w);
    EXPECT_EQ (cp, 0x65e5);
    EXPECT_EQ (w, threechars + 4);
    cp = prior(w);
    EXPECT_EQ (cp, 0x10346);
    EXPECT_EQ (w, threechars);
}

TEST(UnCheckedAPITests, test_advance)
{
    const char* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    const char* w = threechars;
    utf8::unchecked::advance(w, 2);
    EXPECT_EQ(w, threechars + 7);
    utf8::unchecked::advance(w, -2);
    EXPECT_EQ(w, threechars);
    utf8::unchecked::advance(w, 3);
    EXPECT_EQ(w, threechars + 9);
    utf8::unchecked::advance(w, -2);
    EXPECT_EQ(w, threechars + 4);
    utf8::unchecked::advance(w, -1);
    EXPECT_EQ(w, threechars);
}

TEST(UnCheckedAPITests, test_distance)
{
    const char* twochars = "\xe6\x97\xa5\xd1\x88";
    size_t dist = utf8::unchecked::distance(twochars, twochars + 5);
    EXPECT_EQ (dist, 2);
}

TEST(UnCheckedAPITests, test_utf32to8)
{
    int utf32string[] = {0x448, 0x65E5, 0x10346, 0};
    string utf8result;
    utf32to8(utf32string, utf32string + 3, back_inserter(utf8result));
    EXPECT_EQ (utf8result.size(), 9);
}

TEST(UnCheckedAPITests, test_utf8to32)
{
    const char* twochars = "\xe6\x97\xa5\xd1\x88";
    vector<int> utf32result;
    utf8to32(twochars, twochars + 5, back_inserter(utf32result));
    EXPECT_EQ (utf32result.size(), 2);
}

TEST(UnCheckedAPITests, test_utf16to8)
{
    unsigned short utf16string[] = {0x41, 0x0448, 0x65e5, 0xd834, 0xdd1e};
    string utf8result;
    utf16to8(utf16string, utf16string + 5, back_inserter(utf8result));
    EXPECT_EQ (utf8result.size(), 10);
}

TEST(UnCheckedAPITests, test_utf8to16)
{
    char utf8_with_surrogates[] = "\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e";
    vector <unsigned short> utf16result;
    utf8to16(utf8_with_surrogates, utf8_with_surrogates + 9, back_inserter(utf16result));
    EXPECT_EQ (utf16result.size(), 4);
    EXPECT_EQ (utf16result[2], 0xd834);
    EXPECT_EQ (utf16result[3], 0xdd1e);
}

TEST(UnCheckedAPITests, test_replace_invalid)
{
    char invalid_sequence[] = "a\x80\xe0\xa0\xc0\xaf\xed\xa0\x80z";
    vector<char> replace_invalid_result;
    replace_invalid (invalid_sequence, invalid_sequence + sizeof(invalid_sequence), std::back_inserter(replace_invalid_result), '?');
    bool bvalid = utf8::is_valid(replace_invalid_result.begin(), replace_invalid_result.end());
    EXPECT_TRUE (bvalid);
    const char fixed_invalid_sequence[] = "a????z";
    EXPECT_EQ (sizeof(fixed_invalid_sequence), replace_invalid_result.size());
    EXPECT_TRUE (std::equal(replace_invalid_result.begin(), replace_invalid_result.begin() + sizeof(fixed_invalid_sequence), fixed_invalid_sequence));
}

