#include <iostream>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 5001

int main(void)
{
	long max_fd = 1;
	struct sockaddr_in server_addr, client_addr;
	long server_sd;
	long max_sd;
	int client_sd;
	int ready_count;
	int ret;
	ssize_t recv_ret;
	int new_client_sd;
	fd_set master_set;
	fd_set readfds_set;
	// fd_set writefds;
	struct timeval tv;
	tv.tv_sec = 50;
	tv.tv_usec = 0;

	//  1. create a socket
	if ((server_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::cerr << "socket failed\n";
		exit(EXIT_FAILURE);
	}
	// std::cout << server_sd << "\n";

	// 1.1 - make the socket non-blocking using ioctl
	//    set socket to be nonblocking. All of the sockets for
	//    the incoming connections will also be nonblocking since
	// 	they will inherit that state from the listening socket.

	// 2. server address struct
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0

	// 3. Binding  socket to a specific IP  and port
	if ((ret = bind(server_sd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in))) == -1)
	{
		std::cerr << "bind failed\n";
		exit(EXIT_FAILURE);
	}

	// 4. listening on server socket
	if (listen(server_sd, 20) == -1) // 20 - queue limit for incoming connections
	{
		std::cerr << "listen failed\n";
		exit(EXIT_FAILURE);
	}

	std::cout << "server listening on port " <<  PORT << "\n";

	FD_ZERO(&master_set);
	FD_ZERO(&readfds_set); // Clears all file descriptors in the set, initializing it to an empty set.
	// FD_ZERO(&writefds);
	FD_SET(server_sd, &master_set);

	for (;;)
	{
		readfds_set = master_set;
		if ((ready_count = select(FD_SETSIZE, &readfds_set, 0, 0, &tv)) == -1)
		{
			std::cout << "-1\n";
			exit(EXIT_FAILURE);
		}
		else if (ready_count == 0)
		{
			std::cout << "time passed.\n";
			exit(EXIT_FAILURE);
		}
		// loop on the socket descriptors
		// change to max_sd
		for (size_t sd = 0; sd < FD_SETSIZE; ++sd)
		{
			if (FD_ISSET(sd, &readfds_set))
			{
				// is the socket descriptor is the server sd
				if (sd == server_sd)
				{
					// accept new connections - clients - and update the master_set
					socklen_t client_len = sizeof(struct sockaddr_in);
					if ((new_client_sd = accept(sd, (struct sockaddr *)&client_addr, &client_len)) == -1)
					{
						std::cout << "time passed.\n";
						exit(EXIT_FAILURE);
					}

					std::cout << ("Accepted a client connection on socket ") << new_client_sd << "\n";

					FD_SET(new_client_sd, &master_set);
					if (new_client_sd > max_sd)
					{
						max_sd = new_client_sd;
					}
				}
				else
				{
					// a ready to read socket descriptor, receive data (request)
					char buf[200];
					if ((recv_ret = recv(sd, buf, 200, 0)) == -1)
					{
						std::cout << "recv failed.\n";
						exit(EXIT_FAILURE);
					}
					else if (recv_ret == 0)
					{
						std::cout << "Connection closed by the remote  on socket " << sd << "\n";
						FD_CLR(sd, &master_set);
					}
					else
					{
						std::cout << "client data :  " << buf;
					}

				}
			}
			// else if (FD_ISSET(sd, &write_fds))
			// {

			// }
		}
	}

	return (0);
}

/*
select returns ready count
select() returns the number of sockets that had things going on with them -- i.e. they're readable.
nce select() returns, the original fd_set has been modified so it now reflects the state of why select() woke up. i.e. If file descriptor 4 was originally in the fd_set, and then it became readable, the fd_set contains file descriptor 4 in it.
After select() has returned, readfds will be cleared of all file descriptors except for those that are ready for reading.
After select() has returned, writefds will be cleared of all file descriptors except for those that are ready for writing.
FD_SETSIZE, change it later to max_sds

*/
