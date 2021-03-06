/* test_monitor.c
 * Kenneth Cross
 * Mason Itkin
 *
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

#include "SNMP_jog.c"
#include "monitor.c"
// All the monitor helper functions go in the monitor.c file

// Globals for Session and Response
netsnmp_session session, *ss; // holds connection info.
netsnmp_pdu *response; // holds info. that the remote host sends back

int main(int argc, char *argv[]){
  int seconds, samples;
  char *community, *monitor_ip;
  char* mibVar[25];

  if (argc < 5){
    printf("\n##########USING DEFAULTS#############\n\n");
    seconds = 5;
    samples = 2;
    community = "public";
    monitor_ip = "127.0.0.1";
  } else{
    seconds = atoi(argv[1]);
    samples = atoi(argv[2]);
    community = argv[3];
    monitor_ip = argv[4];
  }

  //initializations
  init_snmp("test_monitor");
  snmp_sess_init( &session );
  session.peername = strdup(monitor_ip);

  session.version = SNMP_VERSION_1;
  session.community = community;
  session.community_len = strlen(session.community);

  // initalize variables in SNMP-jog
  init_session(session, ss, response);
  init_session_monitor(session, ss, response);

  // Open the session
  ss = snmp_open(&session);
  if (!ss) { 
    snmp_sess_perror("Failed to open session", &session);
    exit(1);
  }   

  printf("The Agent's System Description is:\n");
  getPdu("sysDescr.0");

  printInter();

  /* 
   * The key is to next grab all the objects and store them
   * in order to poll continuously
   */

  // a few tests on monitor.c
  monitor(seconds, samples);  

  // Close the Session
  snmp_close(ss); // (2)

  return(0);
}
