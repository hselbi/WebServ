#include "webServ.hpp"
#include "readFiles.hpp"


int		main(int ac, char **av)
{
	if (ac == 2)
	{
		readFiles		cluster;

		try {
			cluster.config(av[1]);
			// if (cluster.setup() == -1)
			// 	return (1);
			// cluster.run();
			// cluster.clean();			
		}
		catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
	}
	else
	{
		std::cerr << RED << "Invalid number of arguments." << RESET << std::endl;
		return (1);
	}
	return (0);
}
