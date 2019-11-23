#include <URL.hpp>
#include <limits.h>
#include <string_view>

namespace
{
	void ProcessSpecialCase(std::string_view& str, std::string& out, uint8_t& flag);
	void ProcessSpecialCase(std::string_view& str, std::vector< std::string >& out, uint8_t& flag, const char* delims = "/?#:");
	void ParseRestString(std::string_view& startString, size_t removeIndex, std::shared_ptr< URL::URL::URLNode> node, uint8_t& mainFlag, const char* delims = "/?#:");
	bool ParseUint16(std::string_view& str, uint16_t& portNumber, uint8_t& count);
}

namespace URL
{
	URL::~URL() = default;

	struct URL::URLNode
	{
		std::string m_scheme;
		std::string m_userInfo;
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

	std::string URL::GetUserInfo() const
	{
		return m_node->m_userInfo;
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
		uint8_t mainFlag = 0;
		std::string_view schemeOrDomain = startString.substr(0, schemeDelimiter);

		size_t notSchemeDelimiter = schemeOrDomain.find_first_of("./@");
		if (notSchemeDelimiter == std::string::npos)
		{
			m_node->m_scheme = schemeOrDomain;
			startString.remove_prefix(schemeDelimiter);

			if (auto authority = startString.substr(0, 3); authority == "://")
			{
				startString.remove_prefix(authority.length());
				if (size_t userDelim = startString.find('@'); userDelim != std::string::npos)
				{
					m_node->m_userInfo = startString.substr(0, userDelim);
					ParseRestString(startString, userDelim, m_node, mainFlag);
				}
				else
				{
					std::string tmp;
					size_t length = startString.length();
					tmp.resize(length + 1);
					tmp[0] = '@';
					startString.copy(tmp.data() + 1, length, 0);
					std::string_view rest = tmp;

					m_node->m_userInfo.clear();
					ParseRestString(rest, 0, m_node, mainFlag);
				}
			}
			else if (startString[0] == ':')
			{
				if (size_t userDelim = startString.find('@'); userDelim != std::string::npos)
				{
					startString.remove_prefix(1);
					m_node->m_userInfo = startString.substr(0, userDelim - 1);
					std::string tmp = startString.data();
					tmp[userDelim - 1] = ':';
					std::string_view rest = tmp;
					ParseRestString(rest, userDelim - 1, m_node, mainFlag);
					if (mainFlag == 1)
						return true;
				}
				else
				{
					m_node->m_userInfo.clear();
					ParseRestString(startString, 0, m_node, mainFlag);
					if (mainFlag == 1)
						return true;
				}
			}
		}
		else if (notSchemeDelimiter != std::string::npos)
		{
			m_node->m_scheme.clear();
 			if (size_t userDelim = schemeOrDomain.find('@'); userDelim != std::string::npos)
			{
				m_node->m_userInfo = startString.substr(0, userDelim);
				ParseRestString(startString, userDelim, m_node, mainFlag);
				if (mainFlag == 1)
					return true;
			}
			else
			{
				//TODO: remake mb
				m_node->m_userInfo.clear();

				std::string tmp;
				size_t length = startString.length();
				tmp.resize(length + 1);
				tmp[0] = '@';
				startString.copy(tmp.data() + 1, length, 0);
				std::string_view rest = tmp;
				ParseRestString(rest, 0, m_node, mainFlag);
				if (mainFlag == 1)
					return true;
			}
		}
		return true;
	}
}

namespace
{
	void ParseRestString(std::string_view& startString, size_t removeIndex, std::shared_ptr< URL::URL::URLNode> node, uint8_t& mainFlag, const char* delims)
	{
		startString.remove_prefix(removeIndex);
		if (startString.empty())
			mainFlag = 1;
		uint8_t flag;
		for (;;)
		{
			flag = 0;
			if (startString.empty() || (startString.find_first_not_of(delims) == std::string::npos))
				break;
			else if (startString[0] == '@')
			{
				ProcessSpecialCase(startString, node->m_domain, flag);
				if (flag == 1)
					break;
			}
			else if (startString[0] == ':')
			{
				uint8_t portLength = 0;
				if (ParseUint16(startString, node->m_port, portLength))
				{
					startString.remove_prefix(portLength + 1);
				}
				else
				{
					ProcessSpecialCase(startString, node->m_path, flag);
				}
				if (flag == 1)
					break;
			}
			else if (startString[0] == '/')
			{

				ProcessSpecialCase(startString, node->m_path, flag);
				if (flag == 1)
					break;
			}
			else if (startString[0] == '?')
			{
				if (startString[1] == 'q' && startString[2] == '=')
				{
					startString.remove_prefix(3);
					ProcessSpecialCase(startString, node->m_query, flag);
				}
				if (flag == 1)
					break;
			}
			else if (startString[0] == '#')
			{
				ProcessSpecialCase(startString, node->m_fragment, flag);
				if (flag == 1)
					break;
			}
		}
		mainFlag = 0;
	}
	void ProcessSpecialCase(std::string_view& str, std::string& out, uint8_t& flag)
	{
		const char* delims = "/?#:@";
		size_t pathEnd = str.find_first_of(delims, 1);
		if (pathEnd == std::string::npos)
		{
			out = str.substr(1);
			flag = 1;
		}
		else if (str.substr(0, 1).find_first_of(delims) != std::string::npos)
		{
			out = str.substr(1, pathEnd - 1);
			str.remove_prefix(pathEnd);
			flag = 0;
		}
		else if (pathEnd != std::string::npos)
		{
			out = str.substr(0, pathEnd);
			str.remove_prefix(pathEnd);
			flag = 0;
		}
	}
	void ProcessSpecialCase(std::string_view& str, std::vector< std::string >& out, uint8_t& flag, const char* delims)
	{
		size_t pathEnd = str.find_first_of(delims, 1);
		if (pathEnd == std::string::npos)
		{
			out.emplace_back(str.substr(1));
			flag = 1;
		}
		else
		{
			out.emplace_back(str.begin() + 1, str.begin() + pathEnd);
			str.remove_prefix(pathEnd);
			flag = 0;
		}
	}
	bool ParseUint16(std::string_view& str, uint16_t& portNumber, uint8_t& count)
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
		if (count > 0)
		{
			portNumber = (uint16_t)tmpPort;
			return true;
		}
		else
			return false;
	}
}
