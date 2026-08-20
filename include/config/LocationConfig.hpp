#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

# include <string>
# include <vector>

/*
** One location block. Produced by Ravi's ConfigParser, held by const
** pointer inside Alona's RouteResult. Immutable after startup.
*/
struct LocationConfig {
	std::string					path;			// URI prefix, e.g. "/cgi-bin/"
	std::string					root;			// filesystem root for this block
	std::vector<std::string>	methods;		// allowed methods
	std::string					index;			// file served for a directory
	bool						autoindex = false;
	std::string					cgiExt;			// ".py" — empty means no CGI here
	std::string					cgiPath;		// execve target, e.g. "/usr/bin/python3"
	std::string					uploadPath;		// where POSTed files land
	std::string					redirect;		// non-empty means this block redirects
	int							redirectCode = 301;

	/*
	** Defaults to GET only. The parser overwrites `methods` whenever the
	** config names them explicitly, so an untouched block stays read-only.
	*/
	LocationConfig() : methods(1, "GET"), index("index.html") {}

	bool allowsMethod(const std::string &method) const
	{
		for (size_t i = 0; i < methods.size(); i++)
		{
			if (methods[i] == method)
				return (true);
		}
		return (false);
	}

	bool isCGI() const { return (!cgiExt.empty()); }
	bool isRedirect() const { return (!redirect.empty()); }
};

#endif
