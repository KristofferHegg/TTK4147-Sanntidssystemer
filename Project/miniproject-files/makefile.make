 # build an executable named myprog from myprog.c
  all: main.c 
    sudo ../buildroot/output/staging/usr/bin/avr32-linux-gcc -o /export/nfs/miniproject main.c pi.c network/udp.c time/time.h

  clean: 
    $(RM) main