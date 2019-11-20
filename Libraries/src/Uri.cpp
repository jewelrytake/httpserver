#include <Uri.hpp>
#include <string>
#include <limits.h>

namespace Uri
{
	struct Uri::Impl
	{
		std::string m_scheme;
		std::string m_host;
		std::string m_fragment;
		std::string m_query;
		bool m_hasPort = false;
		uint16_t m_port = 0;
		std::vector< std::string > m_path;
	};
	Uri::~Uri() = default;
	Uri::Uri() :
		m_impl(new Impl)
	{}
	
	bool Uri::ParseFromString(const std::string& uriString)
	{
		const auto schemeEnd = uriString.find(':');
		std::string rest;
		if (schemeEnd == std::string::npos)
		{
			m_impl->m_scheme.clear();
			rest = uriString;
		}
		else
		{
			m_impl->m_scheme = uriString.substr(0, schemeEnd);
			rest = uriString.substr(schemeEnd + 1);
		}
		m_impl->m_hasPort = false;
		const size_t pathEnd = rest.find_first_of("?#");
		std::string hostAndPathString = rest.substr(0, pathEnd);
		const std::string queryAndOrFragment = rest.substr(hostAndPathString.length());
		if (hostAndPathString.substr(0, 2) == "//")
		{
			size_t authorityEnd = hostAndPathString.find('/', 2);
			if (authorityEnd == std::string::npos)
			{
				authorityEnd = hostAndPathString.length();
			}
			const size_t portDelimiter = hostAndPathString.find(':');
			if (portDelimiter == std::string::npos)
			{
				m_impl->m_host = hostAndPathString.substr(2, authorityEnd - 2);
			}
			else
			{
				m_impl->m_host = hostAndPathString.substr(2, portDelimiter - 2);
				uint32_t tmpPort = 0;
				for (auto c : hostAndPathString.substr(portDelimiter + 1, authorityEnd - portDelimiter - 1))
				{
					if (c < '0' || c > '9')
						return false;
					tmpPort *= 10;
					tmpPort += (uint16_t)(c - '0');

					if ((tmpPort & ~((1 << 16) - 1)) != 0)
						return false;
				}
				m_impl->m_port = (uint16_t)tmpPort;
				m_impl->m_hasPort = true;
			}
			hostAndPathString = hostAndPathString.substr(authorityEnd);
		}
		else
		{
			m_impl->m_host.clear();
		}
		std::string pathString = hostAndPathString;
		m_impl->m_path.clear();

		if (pathString == "/")
		{
			m_impl->m_path.emplace_back("");
			pathString.clear();
		}
		else if (!pathString.empty())
		{
			for (;;)
			{
				auto pathDelimiter = pathString.find("/");
				if (pathDelimiter == std::string::npos)
				{
					m_impl->m_path.emplace_back(pathString);
					pathString.clear();
					break;
				}
				else
				{
					m_impl->m_path.emplace_back(pathString.begin(), pathString.begin() + pathDelimiter);
					pathString = pathString.substr(pathDelimiter + 1);
				}
			}
		}
		const size_t fragmentDelimiter = queryAndOrFragment.find('#');
		if (fragmentDelimiter == std::string::npos)
		{
			m_impl->m_fragment.clear();
			rest = queryAndOrFragment; 
		}
		else
		{
			m_impl->m_fragment = queryAndOrFragment.substr(fragmentDelimiter + 1);
			rest = queryAndOrFragment.substr(0, fragmentDelimiter);
		}
		if (rest.empty())
		{
			m_impl->m_query.clear();
		}
		else
		{
			m_impl->m_query = rest.substr(1);
		}
		return true;
	}
	std::string Uri::GetHost() const
	{
		return m_impl->m_host;
	}
	std::string Uri::GetScheme() const
	{
		return m_impl->m_scheme;
	}
	std::string Uri::GetFragment() const
	{
		return m_impl->m_fragment;
	}
	std::string Uri::GetQuery() const
	{
		return m_impl->m_query;
	}
	bool Uri::HasPort() const
	{
		return m_impl->m_hasPort;
	}
	uint16_t Uri::GetPort() const
	{
		return m_impl->m_port;
	}
	std::vector<std::string> Uri::GetPath() const
	{
		return m_impl->m_path;
	}
	bool Uri::IsRelativeReference() const
	{
		return m_impl->m_scheme.empty();
	}
	bool Uri::ContainsRelativePath() const
	{
		if (m_impl->m_path.empty())
		{
			return true;
		}
		else
		{
			return !m_impl->m_path[0].empty();
		}
	}
}