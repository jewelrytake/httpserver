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
		{"foo://example.com/api/foo/gg/gg?q=john#fragment", {"api", "foo", "gg", "gg"}, 0, "john", "fragment"},
		{"foo://example.com/api/foo/gg/gg?q=uery#fragment", {"api", "foo", "gg", "gg"}, 0, "uery", "fragment"},
		{"foo://example.com:8042/api/foo?q=alex#fragment#", {"api", "foo"}, 8042, "alex", "fragment"},
		{"foo://example.com:8042/api/foo?q=name#fragment?#/", {"api", "foo"}, 8042, "name", "fragment"},
		{"foo://example.com?q=name#fragment?#/", {}, 0, "name", "fragment"},
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
		{"example.com:8042/api/foo?q=bar#fobar", "example.com", {"api", "foo"}, 8042, "bar", "fobar"},
		{"foo://example.com/api/foo/gg/gg#fragment/", "example.com", {"api", "foo", "gg", "gg"}, 0, {}, "fragment"},
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

TEST(URLTest, CorrectDomainsPathsPortsQueriesFragmentsUserInfos)
{
	struct TestVector
	{
		std::string urlString;
		std::string urlScheme;
		std::string userInfo;
		std::string urlDomain;
		std::vector< std::string > paths;
		uint16_t urlPort;
		std::string urlQuery;
		std::string urlFragment;
	};

	const std::vector< TestVector > tests =
	{
		{"http://user@example.com:8080/fdsafdafa", "http", "user", "example.com", {"fdsafdafa"}, 8080, {}, {}},
		{"http://user@example.com/fdsafdafa", "http", "user", "example.com", {"fdsafdafa"}, 0, {}, {}},
		{"urn:user@example:animal:ferret:nose", "urn", "user", {}, {"example", "animal", "ferret", "nose"}, 0, {}, {}},
		{"urn:example:animal:ferret:nose", "urn", {}, {}, {"example", "animal", "ferret", "nose"}, 0, {}, {}},
		{"john@example.com:3030/fdsafdafa", {}, "john", "example.com", {"fdsafdafa"}, 3030, {}, {}},
		{"example.com:3030/fdsafdafa////", {}, {}, "example.com", {"fdsafdafa"}, 3030, {}, {}},
		{"https://john@example.com/api/foo/", "https", "john", "example.com", {"api", "foo"}, 0, {}, {}},
		{"user@example.com:8042", {}, "user", "example.com", {}, 8042, {}, {}},
		{"example.com:8042/api/foo?q=bar#fobar", {}, {}, "example.com", {"api", "foo"}, 8042, "bar", "fobar"},
	};

	size_t index = 0;
	for (const auto& test : tests)
	{
		URL::URL url;
		ASSERT_TRUE(url.ParseURL(test.urlString)) << index;
		ASSERT_EQ(test.urlScheme, url.GetScheme()) << index;
		ASSERT_EQ(test.userInfo, url.GetUserInfo()) << index;
		ASSERT_EQ(test.urlDomain, url.GetDomain()) << index;
		ASSERT_EQ(test.paths, url.GetPath()) << index;
		ASSERT_EQ(test.urlPort, url.GetPort()) << index;
		ASSERT_EQ(test.urlQuery, url.GetQuery()) << index;
		ASSERT_EQ(test.urlFragment, url.GetFragment()) << index;
		++index;
	}
}
