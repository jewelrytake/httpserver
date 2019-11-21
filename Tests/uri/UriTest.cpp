//#include <Uri.hpp>
//#include <gtest/gtest.h>
//#include <stddef.h>
//TEST(UriTests, ParseFromStringNoScheme)
//{
//	Uri::Uri uri;
//	ASSERT_TRUE(uri.ParseFromString("foo/bar"));
//	ASSERT_EQ("", uri.GetScheme());
//	ASSERT_EQ((std::vector< std::string >
//	{
//		"foo", "bar"
//	}), uri.GetPath());
//}
//TEST(UriTests, ParseFromStringUrl)
//{
//	Uri::Uri uri;
//	ASSERT_TRUE(uri.ParseFromString("http://www.example.com/foo/bar/foo/foo"));
//	ASSERT_EQ("http", uri.GetScheme());
//	ASSERT_EQ("www.example.com", uri.GetHost());
//	ASSERT_EQ((std::vector< std::string >
//	{
//		"", "foo", "bar", "foo", "foo"
//	}), uri.GetPath());
//}
//
//TEST(UriTests, ParseFromStringUrn)
//{
//	Uri::Uri uri;
//	ASSERT_TRUE(uri.ParseFromString("urn:book:fantasy:Hobbit"));
//	ASSERT_EQ("urn", uri.GetScheme());
//	ASSERT_EQ("", uri.GetHost());
//	ASSERT_EQ((std::vector< std::string >
//	{
//		"book:fantasy:Hobbit"
//	}), uri.GetPath());
//}
//TEST(UriTests, ParseFromStringPathCornerCases)
//{
//	struct TestVector
//	{
//		std::string pathIn;
//		std::vector< std::string > pathOut;
//	};
//	const std::vector< TestVector > testVectors
//	{
//		{"", {}},
//		{"/", {""}},
//		{"/foo", {"", "foo"}},
//		{"foo/", {"foo", ""}}
//	};
//	size_t index = 0;
//	for (const auto& testVector : testVectors)
//	{
//		Uri::Uri uri;
//		ASSERT_TRUE(uri.ParseFromString(testVector.pathIn)) << index;
//		ASSERT_EQ(testVector.pathOut, uri.GetPath()) << index;
//		++index;
//	}
//}
//
//TEST(UriTests, ParseFromStringHostName)
//{
//	Uri::Uri uri;
//	ASSERT_TRUE(uri.ParseFromString("http://www.example.com:8080/foo/bar/foo/foo"));
//	ASSERT_EQ("www.example.com", uri.GetHost());
//}
//
//TEST(UriTests, ParseFromStringDoesNotHaveAPortNumber)
//{
//	Uri::Uri uri;
//	ASSERT_TRUE(uri.ParseFromString("http://www.example.com/foo/bar/foo/foo"));
//	ASSERT_EQ("www.example.com", uri.GetHost());
//	ASSERT_FALSE(uri.HasPort());
//}
//TEST(UriTests, ParseFromStringTwiceFirstWithPortNumberThenWithout)
//{
//	Uri::Uri uri;
//	ASSERT_TRUE(uri.ParseFromString("http://www.example.com:8080/foo/bar/foo/foo"));
//	ASSERT_TRUE(uri.ParseFromString("http://www.example.com/foo/bar/foo/foo"));
//	ASSERT_FALSE(uri.HasPort());
//}
//TEST(UriTests, ParseFromStringBadPortNumberAlph)
//{
//	Uri::Uri uri;
//	ASSERT_FALSE(uri.ParseFromString("http://www.example.com:spam/foo/bar/foo/foo"));
//}
//TEST(UriTests, ParseFromStringBadPortNumberNumAndAlph)
//{
//	Uri::Uri uri;
//	ASSERT_FALSE(uri.ParseFromString("http://www.example.com:8080spam/foo/bar/foo/foo"));
//}
//
//TEST(UriTests, ParseFromStringBadPortNumberTooBig)
//{
//	Uri::Uri uri;
//	ASSERT_FALSE(uri.ParseFromString("http://www.example.com:65536/foo/bar/foo/foo"));
//}
//TEST(UriTests, ParseFromStringLargestPortNumber)
//{
//	Uri::Uri uri;
//	ASSERT_TRUE(uri.ParseFromString("http://www.example.com:65535/foo/bar/foo/foo"));
//	ASSERT_EQ(65535, uri.GetPort());
//	ASSERT_TRUE(uri.HasPort());
//}
//
//TEST(UriTests, ParseFromStringNegativePortNumber)
//{
//	Uri::Uri uri;
//	ASSERT_FALSE(uri.ParseFromString("http://www.example.com:-100/foo/bar/foo/foo"));
//}
//
//TEST(UriTests, ParseFromStringEndsAfterAuthority)
//{
//	Uri::Uri uri;
//	ASSERT_TRUE(uri.ParseFromString("http://www.example.com"));
//}
//
//TEST(UriTests, ParseFromStringRelativeVsNonRelativeReferences)
//{
//	struct TestVector
//	{
//		std::string uriString;
//		bool isRelativeReference;
//	};
//	const std::vector< TestVector > testVectors
//	{
//		{"http::/www.example.com/", false},
//		{"http::/www.example.com", false},
//		{"/foo", true},
//		{"/", true}};
//	size_t index = 0;
//	for (const auto& testVector : testVectors)
//	{
//		Uri::Uri uri;
//		ASSERT_TRUE(uri.ParseFromString(testVector.uriString)) << index;
//		ASSERT_EQ(testVector.isRelativeReference, uri.IsRelativeReference()) << index;
//		++index;
//	}
//}
//TEST(UriTests, ParseFromStringRelativeVsNonRelativePaths)
//{
//	struct TestVector
//	{
//		std::string uriString;
//		bool containsRelativePath;
//	};
//	const std::vector< TestVector > testVectors
//	{
//		{"http://www.example.com/", false},
//		{"http://www.example.com", true},
//		{"/", false},
//		{"foo", true},
//		{"", true}
//	};
//	size_t index = 0;
//	for (const auto& testVector : testVectors)
//	{
//		Uri::Uri uri;
//		ASSERT_TRUE(uri.ParseFromString(testVector.uriString)) << index;
//		ASSERT_EQ(testVector.containsRelativePath, uri.ContainsRelativePath()) << index;
//		++index;
//	}
//}
//
//TEST(UriTests, ParseFromStringQueryAndFragmentElements)
//{
//	struct TestVector
//	{
//		std::string uriString;
//		std::string host;
//		std::string query;
//		std::string fragment;
//	};
//	const std::vector< TestVector > testVectors
//	{
//		{"http://www.example.com/", "www.example.com", "", ""},
//		{"http://example.com?foo", "example.com", "foo", ""},
//		{"http://www.example.com#foo", "www.example.com", "", "foo"},
//		{"http://www.example.com?foo#bar", "www.example.com", "foo", "bar"},
//		{"http://www.example.com?earth?day#bar", "www.example.com", "earth?day", "bar"},
//		{"http://www.example.com/spam?foo#bar", "www.example.com", "foo", "bar"}, 
//		{"http://www.example.com?", "www.example.com", "", ""},
//	}; 
//	size_t index = 0;
//	for (const auto& testVector : testVectors)
//	{
//		Uri::Uri uri;
//		ASSERT_TRUE(uri.ParseFromString(testVector.uriString)) << index;
//		ASSERT_EQ(testVector.host, uri.GetHost()) << index;
//		ASSERT_EQ(testVector.query, uri.GetQuery()) << index;
//		ASSERT_EQ(testVector.fragment, uri.GetFragment()) << index;
//		++index;
//	}
//}
//
//TEST(UriTests, ParseFromStringUserInfo)
//{
//	struct TestVector
//	{
//		std::string uriString;
//		std::string userInfo;
//	};
//	const std::vector< TestVector > testVectors
//	{
//		{"http://www.example.com/", ""},
//		{"http://joe@www.example.com", "joe"},
//		{"http://pepe:feelsbadman@www.example.com", "pepe:feelsbadman"},
//		{"//www.example.com", ""},
//		{"//bob@www.example.com", "bob"},
//		{"/", ""},
//		{"foo", ""}
//	};
//	size_t index = 0;
//	for (const auto& testVector : testVectors)
//	{
//		Uri::Uri uri;
//		ASSERT_TRUE(uri.ParseFromString(testVector.uriString)) << index;
//		ASSERT_EQ(testVector.userInfo, uri.GetUserInfo()) << index;
//		++index;
//	}
//}
//
