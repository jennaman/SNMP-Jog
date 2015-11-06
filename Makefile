CC=gcc -std=c11

TARGETS=SNMP_jog monitor_tester

CFLAGS=-I. `net-snmp-config --cflags`
BUILDLIBS=`net-snmp-config --libs`
BUILDAGENTLIBS=`net-snmp-config --agent-libs`

# shared library flags (assumes gcc)
DLFLAGS=-fPIC -shared

all: $(TARGETS)

SNMP_jog:
	$(CC) -g test_monitor.c $(BUILDLIBS) -o SNMP_jog

#SNMP_jog:
	#$(CC) -g main.c $(BUILDLIBS) -o SNMP_jog 

clean:
	rm $(TARGETS)
