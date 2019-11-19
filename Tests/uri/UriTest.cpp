#include <Uri.hpp>
#include <gtest/gtest.h>

TEST(UriTests, ParseFromString)
{
	Uri::Uri uri;
	ASSERT_TRUE(uri.ParseFromString("http://www.example.com/foo/bar/foo/foo/"));
	ASSERT_EQ("http", uri.GetScheme());
	ASSERT_EQ("www.example.com", uri.GetHost());
	ASSERT_EQ((std::vector< std::string >
	{
		"", "foo", "bar", "foo", "foo"
	}), uri.GetPath());

	ASSERT_TRUE(uri.ParseFromString("urn:book:fantasy:Hobbit"));
	ASSERT_EQ("urn", uri.GetScheme());
	ASSERT_EQ("", uri.GetHost());
	ASSERT_EQ((std::vector< std::string >
	{
		"book", "fantasy", "Hobbit"
	}), uri.GetPath());
}