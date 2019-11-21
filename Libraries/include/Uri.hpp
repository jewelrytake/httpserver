#pragma once

#include <memory>
#include <string>
#include <vector>
#include <stdint.h>

namespace Uri
{
	class Uri
	{
	public:
		~Uri();
		Uri(const Uri&) = delete;
		Uri(Uri&&) = delete;
		Uri& operator=(const Uri&) = delete;
		Uri operator=(Uri&&) = delete;
		
		// API
		Uri();
		/**
		* This method builds the URI from the elements parsed
		* from the given string rendering of a URI.
		*/
		bool ParseFromString(const std::string& uriString);
		std::string GetHost() const;
		std::string GetScheme() const;
		std::string GetFragment() const;
		std::string GetQuery() const;
		std::string GetUserInfo() const;
		bool HasPort() const;
		uint16_t GetPort() const;
		bool IsRelativeReference() const;
		bool ContainsRelativePath() const;
		std::vector < std::string > GetPath() const;
	private:
		struct Impl;
		std::unique_ptr< Impl > m_impl;
	};
}
