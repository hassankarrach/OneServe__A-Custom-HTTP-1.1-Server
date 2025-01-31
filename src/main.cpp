#include "../includes/webserv.hpp"
#include "../includes/http/request.hpp"

int main(int ac, char **av)
{
	if (ac != 2)
		return 1;

	ParsingConfigFile Config;

	Server server;

	try
	{
		Config.ParseFile(av[1]);

		server.CreatServer(Config.getHosts());
		
		// t_data info = Config("localhost:6000");  // method for choose  server block
		// t_map location = info("pwd1"); // method for choose location block

		// std::cout << info.server["host"][0] << std::endl;
		// std::cout << info.server["port"][0] << std::endl;

		// return 1;
		server.CreatMultiplexing();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}

	return 0;
}

/*
	TO-CHECK/DO LATER :
	1 - content length > body length !?
	2 - HANDLE port, being on uri !!!!!!!!
	3 - use the GOD DAMN & refs everywhere ****

	4 - Redirections (301, 302, 303, 307, 308)
 */