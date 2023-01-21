#include <gtest/gtest.h>
#include <Core/String/String.hpp>
#include <defines.h>
#include <string>

TEST(StringTests, CreateDestroy)
{
  S4   str4   = "4444";
  S8   str8   = "88888888";
  S16  str16  = "1616161616";
  S32  str32  = "32323232323232";
  S64  str64  = "646464646464646464";
  S128 str128 = "128128128128128128128128";

  EXPECT_TRUE(str4.Equals("4444"));

  EXPECT_TRUE(str8.Equals("88888888"));

  EXPECT_TRUE(str16.Equals("1616161616"));

  EXPECT_TRUE(str32.Equals("32323232323232"));

  EXPECT_TRUE(str64.Equals("646464646464646464"));

  EXPECT_TRUE(str128.Equals("128128128128128128128128"));

  EXPECT_FALSE(str128.Equals("12812812812812812812812"));
  EXPECT_TRUE(str128.BeginsWith("1281"));
  EXPECT_TRUE(str128.BeginsWith("128128128128128128128128"));
  EXPECT_TRUE(str128.EndsWith("28128"));
  EXPECT_TRUE(str128.EndsWith("128128128128128128128128"));

  //std::cout << "Size of S4  : " << sizeof(S4) << std::endl;
  //std::cout << "Size of S8  : " << sizeof(S8) << std::endl;
  //std::cout << "Size of S16  : " << sizeof(S16) << std::endl;
  //std::cout << "Size of S32  : " << sizeof(S32) << std::endl;
  //std::cout << "Size of S64  : " << sizeof(S64) << std::endl;
  //std::cout << "Size of S128  : " << sizeof(S128) << std::endl;
}

TEST(StringTests, TooLarge)
{
  S4   str4   = "4444444444444";

  EXPECT_TRUE(str4.Equals("4444"));
  EXPECT_TRUE(str4.EndsWith("4"));
  EXPECT_TRUE(str4.BeginsWith("4"));


  EXPECT_TRUE(str4.EndsWith("4444"));
  EXPECT_TRUE(str4.BeginsWith("4444"));
}
