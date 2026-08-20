#include "config/ServerConfig.hpp"

/*
** Hardcoded stand-in for the config file so Ivan can bind/listen and Alona
** can route from day one. Deliberately covers every RouteResult::Type the
** router has to produce: static, CGI, upload/delete, redirect.
*/

static LocationConfig makeStatic()
{
	LocationConfig loc;
	loc.path = "/";
	loc.root = "./www";
	loc.index = "index.html";
	loc.autoindex = false;
	return loc;
}

static LocationConfig makeAutoindex()
{
	LocationConfig loc;
	loc.path = "/files/";
	loc.root = "./www/files";
	loc.autoindex = true;
	loc.index = "";
	return loc;
}

static LocationConfig makeUpload()
{
	LocationConfig loc;
	loc.path = "/upload/";
	loc.root = "./www/uploads";
	loc.uploadPath = "./www/uploads";
	loc.methods.clear();
	loc.methods.push_back("GET");
	loc.methods.push_back("POST");
	loc.methods.push_back("DELETE");
	return loc;
}

static LocationConfig makeCGI()
{
	LocationConfig loc;
	loc.path = "/cgi-bin/";
	loc.root = "./www/cgi-bin";
	loc.cgiExt = ".py";
	loc.cgiPath = "/usr/bin/python3";
	loc.methods.clear();
	loc.methods.push_back("GET");
	loc.methods.push_back("POST");
	return loc;
}

static LocationConfig makeRedirect()
{
	LocationConfig loc;
	loc.path = "/old/";
	loc.redirect = "/";
	loc.redirectCode = 301;
	return loc;
}

ServerConfig mockConfig()
{
	ServerConfig config;

	config.port = 8080;
	config.host = "0.0.0.0";
	config.serverNames.push_back("localhost");
	config.root = "./www";
	config.clientMaxBody = 1048576;
	config.errorPages[404] = "./www/errors/404.html";
	config.errorPages[500] = "./www/errors/500.html";

	config.locations.push_back(makeStatic());
	config.locations.push_back(makeAutoindex());
	config.locations.push_back(makeUpload());
	config.locations.push_back(makeCGI());
	config.locations.push_back(makeRedirect());

	return config;
}

/*
** The real parser returns one entry per server block, so hand back a vector
** even while there is only one — Ivan's startup loop won't need rewriting.
*/
std::vector<ServerConfig> mockConfigs()
{
	std::vector<ServerConfig> configs;
	configs.push_back(mockConfig());
	return configs;
}
