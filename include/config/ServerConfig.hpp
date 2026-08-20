#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <string>
# include <vector>
# include <map>
# include <cstddef>

# include "config/LocationConfig.hpp"

/*
** One server block. Ravi parses these once at startup; everyone else holds
** const pointers and never mutates them.
**
** Location matching lives in Alona's Router, not here — this struct only
** answers questions about its own data.
*/
struct ServerConfig {
	int							port = 8080;
	std::string					host = "0.0.0.0";	// Ivan binds this
	std::vector<std::string>	serverNames;		// matched against Host:
	std::string					root = "./www";
	size_t						clientMaxBody = 1048576;	// 1 MiB
	std::map<int, std::string>	errorPages;			// code -> page path
	std::vector<LocationConfig>	locations;			// declaration order preserved

	/* Returns "" when no custom page is configured; caller falls back to
	   HttpResponse::makeError()'s built-in page. */
	std::string errorPage(int code) const
	{
		std::map<int, std::string>::const_iterator it = errorPages.find(code);
		if (it != errorPages.end())
			return (it->second);
		return ("");
	}

	bool hasServerName(const std::string &name) const
	{
		for (size_t i = 0; i < serverNames.size(); i++)
		{
			if (serverNames[i] == name)
				return (true);
		}
		return (false);
	}
};

/*
** Day-1 mock. Real ConfigParser fills the same structs later, so nothing
** downstream changes when it lands.
*/
ServerConfig				mockConfig();
std::vector<ServerConfig>	mockConfigs();

#endif
