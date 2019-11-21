#include <URL.hpp>
#include <gtest/gtest.h>
#include <stddef.h>

TEST(URLTest, CorrectScheme)
{
	URL::URL url;
	ASSERT_TRUE(url.ParseURL("urn:example:animal:ferret:nose"));
	ASSERT_EQ("urn", url.GetScheme());
	std::vector < std::string > vec{ "example", "animal", "ferret", "nose" };
	ASSERT_EQ(vec, url.GetPath());
	ASSERT_TRUE(url.ParseURL("foo://example.com:8042"));
	ASSERT_EQ("foo", url.GetScheme());
	ASSERT_TRUE(url.ParseURL("example.com:8042"));
	ASSERT_EQ("", url.GetScheme());
	
	ASSERT_TRUE(url.ParseURL("urn:example"));
	ASSERT_EQ("urn", url.GetScheme());
}
TEST(URLTest, CorrectDomain)
{
	URL::URL url;
	ASSERT_TRUE(url.ParseURL("foo://example.com:8042"));
	ASSERT_EQ("example.com", url.GetDomain());
	ASSERT_TRUE(url.ParseURL("example.com/api"));
	ASSERT_EQ("example.com", url.GetDomain());
	ASSERT_TRUE(url.ParseURL("urn:example:animal:ferret:nose"));
	ASSERT_EQ("", url.GetDomain());
	ASSERT_TRUE(url.ParseURL("urn:example"));
	ASSERT_EQ("", url.GetDomain());
}
TEST(URLTest, CorrectPort)
{
	URL::URL url;
	ASSERT_TRUE(url.ParseURL("foo://example.com:8042/api"));
	ASSERT_EQ(8042, url.GetPort());
	ASSERT_TRUE(url.ParseURL("example.com:65535/api"));
	ASSERT_EQ(65535, url.GetPort());
	ASSERT_TRUE(url.ParseURL("example.com/api"));
	ASSERT_EQ(0, url.GetPort());
	ASSERT_TRUE(url.ParseURL("urn:example:animal:ferret:nose"));
	ASSERT_EQ(0, url.GetPort());
	ASSERT_TRUE(url.ParseURL("urn:example:8042"));
	ASSERT_EQ(0, url.GetPort());
}

TEST(URLTest, CorrectPathsPortsQueriesFragments)
{
	struct TestVector
	{
		std::string urlString;
		std::vector< std::string > paths;
		uint16_t urlPort;
		std::string urlQuery;
		std::string urlFragment;
	};

	const std::vector< TestVector > tests =
	{
		{"foo://example.com/api/foo/gg/gg?query#fragment/", {"api", "foo", "gg", "gg"}, 0, "query", "fragment"},
		{"foo://example.com/api/foo/gg/gg?query#fragment", {"api", "foo", "gg", "gg"}, 0, "query", "fragment"},
		{"foo://example.com:8042/api/foo?query#fragment#", {"api", "foo"}, 8042, "query", "fragment"},
		{"foo://example.com:8042/api/foo?query#fragment?", {"api", "foo"}, 8042, "query", "fragment"},
		{"foo://example.com:8042/api/foo?query#fragment?#/", {"api", "foo"}, 8042, "query", "fragment"},
		{"urn:example:animal:ferret:nose", {"example", "animal", "ferret", "nose"}, 0, "", ""},
	};

	size_t index = 0;
	for (const auto& test : tests)
	{
		URL::URL url;
		ASSERT_TRUE(url.ParseURL(test.urlString)) << index;
		ASSERT_EQ(test.paths, url.GetPath()) << index;
		ASSERT_EQ(test.urlPort, url.GetPort()) << index;
		ASSERT_EQ(test.urlQuery, url.GetQuery()) << index;
		ASSERT_EQ(test.urlFragment , url.GetFragment()) << index;
		++index;
	}
}
TEST(URLTest, CorrectDomainsPathsPortsQueriesFragments)
{
	struct TestVector
	{
		std::string urlString;
		std::string urlDomain;
		std::vector< std::string > paths;
		uint16_t urlPort;
		std::string urlQuery;
		std::string urlFragment;
	};

	const std::vector< TestVector > tests =
	{
		{"example.com:8042", "example.com", {}, 8042, {}, {}},
		{"example.com:8042/api/foo/", "example.com", {"api", "foo"}, 8042, {}, {}},
		{"example.com:8042/api/foo", "example.com", {"api", "foo"}, 8042, {}, {}},
		{"example.com/api/foo/", "example.com", {"api", "foo"}, 0, {}, {}},
		{"example.com:8042/api/foo?bar#fobar", "example.com", {"api", "foo"}, 8042, "bar", "fobar"},
		{"foo://example.com/api/foo/gg/gg?query#fragment/", "example.com", {"api", "foo", "gg", "gg"}, 0, "query", "fragment"},
		{"foo://example.com/api/foo/gg/gg?query#fragment", "example.com", {"api", "foo", "gg", "gg"}, 0, "query", "fragment"},
		{"foo://example.com:8042/api/foo?query#fragment#", "example.com",{"api", "foo"}, 8042, "query", "fragment"},
		{"foo://example.com:8042/api/foo?query#fragment?", "example.com",{"api", "foo"}, 8042, "query", "fragment"},
		{"foo://example.com:8042/api/foo?query#fragment?#/", "example.com", {"api", "foo"}, 8042, "query", "fragment"},
		{"urn:example:animal:ferret:nose", {}, {"example", "animal", "ferret", "nose"}, 0, "", ""},
	};

	size_t index = 0;
	for (const auto& test : tests)
	{
		URL::URL url;
		ASSERT_TRUE(url.ParseURL(test.urlString)) << index;
		ASSERT_EQ(test.urlDomain, url.GetDomain()) << index;
		ASSERT_EQ(test.paths, url.GetPath()) << index;
		ASSERT_EQ(test.urlPort, url.GetPort()) << index;
		ASSERT_EQ(test.urlQuery, url.GetQuery()) << index;
		ASSERT_EQ(test.urlFragment , url.GetFragment()) << index;
		++index;
	}
}
