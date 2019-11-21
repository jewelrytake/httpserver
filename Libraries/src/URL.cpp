#include <URL.hpp>
#include <limits.h>
#include <string_view>


namespace
{

	bool ParseRestString(std::string_view& startString, size_t removeIndex, std::shared_ptr< URL::URL::URLNode> node, const char* delims = "/?#:");
	bool ParseUint16(std::string_view& str, uint16_t& portNumber, size_t& count);
}

namespace URL
{
	URL::~URL() = default;

	struct URL::URLNode
	{
		std::string m_scheme;
		std::string m_domain;
		uint16_t m_port = 0;
		std::vector< std::string > m_path;
		std::string m_query;
		std::string m_fragment;
	};

	URL::URL() : m_node(new URLNode) {}

	std::string URL::GetScheme() const
	{
		return m_node->m_scheme;
	}

	std::string URL::GetDomain() const
	{
		return m_node->m_domain;
	}

	uint16_t URL::GetPort() const
	{
		return m_node->m_port;
	}

	std::vector<std::string> URL::GetPath() const
	{
		return m_node->m_path;
	}

	std::string URL::GetQuery() const
	{
		return m_node->m_query;
	}

	std::string URL::GetFragment() const
	{
		return m_node->m_fragment;
	}
	
	bool URL::ParseURL(const std::string& url)
	{
		m_node->m_port = 0;
		std::string_view startString = url;
		size_t schemeDelimiter = startString.find(':');
		if (startString[schemeDelimiter + 1] == '/' && startString[schemeDelimiter + 2] == '/')
		{
			m_node->m_scheme = startString.substr(0, schemeDelimiter);
			startString.remove_prefix(schemeDelimiter + 3);
			size_t portOrPathDelim = startString.find_first_of(":/");
			if (portOrPathDelim == std::string::npos)
			{
				m_node->m_domain = startString;
			}
			else
			{
				m_node->m_domain = startString.substr(0, portOrPathDelim);
				if (startString[portOrPathDelim] == ':')
				{
					size_t portLength = 0;
					ParseUint16(startString, m_node->m_port, portLength);
					ParseRestString(startString, portOrPathDelim + portLength + 1, m_node);
				}
				else
				{
					ParseRestString(startString, portOrPathDelim, m_node);
				}
			}
		}
		else
		{
			// example - "urn:example:animal:ferret:nose"
			std::string_view schemeString = startString.substr(0, schemeDelimiter);
			size_t domainPos = schemeString.find_first_of("/.");
			if (domainPos == std::string::npos)
			{
				m_node->m_scheme = schemeString;
				m_node->m_domain.clear();
				ParseRestString(startString, schemeDelimiter, m_node);
			}
			else
			{
				m_node->m_scheme.clear();
				size_t domainEnd = startString.find_first_of(":/");
				if (domainEnd == std::string::npos)
				{
					m_node->m_domain = startString;
				}
				else
				{
					m_node->m_domain = startString.substr(0, domainEnd);
					if (startString[domainEnd] == ':')
					{
						size_t portLength = 0;
						ParseUint16(startString, m_node->m_port, portLength);
						ParseRestString(startString, domainEnd + portLength + 1, m_node);
					}
					else
					{
						ParseRestString(startString, domainEnd, m_node);
					}
				}
			}
		}
		return true;
	}
}

namespace
{
	bool ParseRestString(std::string_view& startString, size_t removeIndex, std::shared_ptr< URL::URL::URLNode> node, const char* delims)
	{

		startString.remove_prefix(removeIndex);
		if (startString.empty())
			return true;
		for (;;)
		{
			if (startString.find_first_not_of(delims) == std::string::npos)
				break;
			if (startString[0] == ':' || startString[0] == '/')
			{
				size_t pathEnd = startString.find_first_of(delims, 1);
				if (pathEnd == std::string::npos)
				{
					node->m_path.emplace_back(startString.substr(1));
					break;
				}
				else
				{
					node->m_path.emplace_back(startString.begin() + 1, startString.begin() + pathEnd);
					startString.remove_prefix(pathEnd);
				}
			}
			if (startString[0] == '/')
			{
				size_t pathEnd = startString.find_first_of(delims, 1);
				if (pathEnd == std::string::npos)
				{
					node->m_path.emplace_back(startString.substr(1));
					break;
				}
				else
				{
					node->m_path.emplace_back(startString.begin() + 1, startString.begin() + pathEnd);
					startString.remove_prefix(pathEnd);
				}
			}
			//CheckDelsim(startString, delims, node)
			if (startString[0] == '?')
			{
				size_t pathEnd = startString.find_first_of(delims, 1);
				if (pathEnd == std::string::npos)
				{
					node->m_query = startString.substr(1);
					break;
				}
				else
				{
					node->m_query = startString.substr(1, pathEnd - 1);
					startString.remove_prefix(pathEnd);
				}
			}
			if (startString[0] == '#')
			{
				size_t pathEnd = startString.find_first_of(delims, 1);
				if (pathEnd == std::string::npos)
				{
					node->m_fragment = startString.substr(1);
					break;
				}
				else
				{
					node->m_fragment = startString.substr(1, pathEnd - 1);
					startString.remove_prefix(pathEnd);
				}
			}
		}
	}

	//TODO: mb rafactor
	//void CheckDelsim(std::string_view& startString, const char* delims, std::shared_ptr< URL::URL::URLNode> node)
	//{
	//	size_t pathEnd = startString.find_first_of(delims, 1);
	//	std::string make;
	//	if (startString[0] == '?')
	//	{
	//		make = node->m_query;
	//	}
	//	else if (startString[0] == '#')
	//	{
	//		make = node->m_fragment;
	//	}
	//	if (pathEnd == std::string::npos)
	//	{
	//		make = startString.substr(1);
	//		return;
	//	}
	//	else
	//	{
	//		make = startString.substr(1, pathEnd - 1);
	//		startString.remove_prefix(pathEnd);
	//	}
	//}
	bool ParseUint16(std::string_view& str, uint16_t& portNumber, size_t& count)
	{
		uint32_t tmpPort = 0;
		int index = 0;
		for (auto c : str)
		{
			if (c < '0' || c > '9')
				continue;
			tmpPort *= 10;
			tmpPort += (uint16_t)(c - '0');
			if ((tmpPort & ~((1 << 16) - 1)) != 0)
			{
				index = 0;
				return false;
			}
			++index;
		}
		count = index;
		portNumber = (uint16_t)tmpPort;
		return true;
	}
}
