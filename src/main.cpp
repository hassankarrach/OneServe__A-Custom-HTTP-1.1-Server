#include "../includes/webserv.hpp"
#include "../includes/http/request.hpp"

Parse_Config Config;

int main(int ac, char **av)
{
	if (ac != 2)
		return 1;


	Server server;

	try
	{
		Config.ParseFile(av[1]);

		// return 1;
		server.CreatServer(Config.getHosts());


		//  access  error pages or info server   use  ==> _config_res["body_size"]  _config_res["403"]
		//  access locations   use  ==> _location_res["root"]  _location_res.find("GET")   

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

	✅, but the connection ends after the redirectin.
	4 - Redirections (301, 302, 303, 307, 308)


	TODO
	5 - on HTTP/1.0 reqs as well as 1 Serv connections :
			=> Send Connection: close in the response to force the client to close the connection.

	TODO
	6 - Expect: 100-continue : The client sends this header to check if the server will accept the request body before sending it.
			=> If the server responds with 100 Continue, the client proceeds to send the body.
			=> cases : - Content-Length limits
					   - unsuported Content-Type
					   - Unauthorized request
					   - 429 Too Many Requests
	TODO
	7 - allowed methods

	TODO
	8 - read - write - idle Timeout

 */