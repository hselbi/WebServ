#include "webServ.hpp"
#include "readFiles.hpp"
#include "config.hpp"


int		main(int ac, char **av)
{
	if (ac == 2)
	{
		config	cluster(av[1]);

		try {
			cluster.print();
			cluster.setup();
			// cluster.print();
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
		std::cerr << "\033[0;31m" << "Invalid number of arguments." << "\033[0m" << std::endl;
		return (1);
	}
	return (0);
}
