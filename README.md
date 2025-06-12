# challenge_chimere

A recruitment test I made the goal was to determine from a log file the number of paquet send between 2 IPv4 and port, and sort them.

The program is based on a radix tree that store the IP adress in a double link list.

I prefered to use radic tree as a hash table. I have a O(24n) and minimize memory allocation.

The leafs of the radix tree are lnk to a double link list.

