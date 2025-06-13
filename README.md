# challenge_chimere

A recruitment test I took, the goal was to determine from a log file how many packets were sent between two IPv4 addreses and ports, and to sort them.

The program is based on a radix tree that stores the IP addresses in a double link list.

I prefered to use radic tree instead of a hash table. The complexity is O(24n) and minimizes memory allocation.

The leaves of the radix tree are link to a double link list.

