#pragma once
#include <string>
#include <stdint.h>
#include <vector>
#include <memory>

namespace URL
{
	struct URL
	{
		~URL();
		URL(const URL&) = delete;
		URL(URL&&) = delete;
		URL& operator=(const URL&) = delete;
		URL operator=(URL&&) = delete;

		// API
		URL();


		bool ParseURL(const std::string& url);
		std::string GetScheme() const;
		std::string GetUserInfo() const;
		std::string GetDomain() const;
		uint16_t GetPort() const;
		std::vector< std::string > GetPath() const;
		std::string GetQuery() const;
		std::string GetFragment() const;

		struct URLNode;
		std::shared_ptr< URLNode > m_node;
	};
}