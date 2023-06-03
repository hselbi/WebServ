#include <stdio.h>
#include <arpa/inet.h>

int main() {
    struct in_addr addr;

    // Assign an IPv4 address
    addr.s_addr = inet_addr("192.168.0.1");

    // Example usage: printing the IP address
    printf("IPv4 Address: %s\n", inet_ntoa(addr));  // Convert IP address to a dotted-decimal string

    return 0;
}
