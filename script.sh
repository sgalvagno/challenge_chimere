rm -rf chimere chimere.o  packet.o  radix.o  list.o
#CFLAGS="-g"
CFLAGS="-O3"
#OPTIONS="-D__SHOW_RADIX__"
gcc -c -o packet.o packet.c $CFLAGS
gcc -c -o radix.o radix.c $CFLAGS
gcc -c -o list.o list.c $CFLAGS
gcc -c -o chimere.o chimere.c $CFLAGS $OPTIONS
gcc -o chimere chimere.o packet.o radix.o list.o
