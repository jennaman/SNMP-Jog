/* monitor.c
 * Kenneth Cross
 * Mason Itkin
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include "SNMP_jog.c"

netsnmp_session session, *ss; // holds connection info.
netsnmp_pdu *response; // holds info. that the remote host sends back

int get_int_object(char *desc);
void print_outgoing_stats(long new_val[], long old_val[], long kbs_poll[], int if_size);

void init_session_monitor(netsnmp_session ses, netsnmp_session *sesp, netsnmp_pdu *rep){
    session = ses;
    ss = sesp;
    response = rep;
} //init_session()

void monitor(int seconds, int samples){
    time_t start, end;
    double elapsed;
    int ifNumber = get_int_object("ifNumber.0");
    long new_value[ifNumber], stored_value[ifNumber]; //compiler might not like
    long kbs_poll[ifNumber]; 

    for (int i = 0; i < ifNumber; i++){
        new_value[i] = 0;
        stored_value[i] = 0;
    }

    for (int i = 0; i < samples; i++){
        // For Devices
        //printInter(); // Print information about the Agent's interfaaces

        // For Statistics
        for (int j = 0; j < ifNumber; j++){
            char poll_device[] = "ifOutOctets.1";
            poll_device[12] += j;

            new_value[j] = get_int_object(poll_device);
        }

        print_outgoing_stats(new_value, stored_value, kbs_poll, ifNumber);

        for (int j = 0; j < ifNumber; j++) stored_value[j] = new_value[j];

        // countdown clock for polling rate
        time(&start);
        do{
            time(&end);
            elapsed = difftime(end, start);
        } while(elapsed < seconds);
    }
} // monitor(char*[])

void print_outgoing_stats(long new_val[], long old_val[], long kbs_poll[], int if_size){
    printf("\n|interface |        bytes/rate|");
    printf("\n|----------|------------------|");

    for (int i = 0; i < if_size; i++){
        kbs_poll[i] = new_val[i] - old_val[i];
        printf("\n|%d        |                %ld|", i+1, kbs_poll[i]);
    }
    printf("\n");
}

char interface_list(){
    int count = 0;
    int ifNumber = 0;
    char rD[] = "ifType.1";
    ifNumber = get_int_object("ifNumber.0");

    for (int i = 0; i < ifNumber; i++){

    }
}

void statistic_collector(){

} //statistic_collector()

int get_int_object(char* desc){
    netsnmp_pdu *pdu; // Protocol Data unit - holds info to send to remote host
    oid anOID[MAX_OID_LEN]; // OID for the remote host Size: 'MAX_OID_LEN'
    size_t anOID_len = MAX_OID_LEN;

    netsnmp_variable_list *vars; // List of var to manipulate via SNMP
    int status;
    int count=1;
    
    pdu = snmp_pdu_create(SNMP_MSG_GET);
    get_node(desc, anOID, &anOID_len); 
    snmp_add_null_var(pdu, anOID, anOID_len); // Appends NULL to oid
  
    status = snmp_synch_response(ss, pdu, &response);
    
    // Process the response.
    if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) { 
       // SUCCESS: Print the result variables (Only 1 for now)
        for(vars = response->variables; vars; vars = vars->next_variable){
            if (vars->type == ASN_INTEGER){
                return *vars->val.integer;
            }
            else if (vars->type == ASN_COUNTER){
                return *vars->val.integer;
            }
            else
                vars = vars->next_variable;
        }
    } else {
       // FAILURE: print what went wrong!
      if (status == STAT_SUCCESS){
        fprintf(stderr, "DEBUG:: Error in packet\nReason: %s\n", 
                snmp_errstring(response->errstat));
        return -1;
      }
      else if (status == STAT_TIMEOUT){
        fprintf(stderr, "Timeout: No response from %s.\n", session.peername);
        return -2;
      }
      else
        snmp_sess_perror("snmpdemoapp", ss);
    }
    return 1;
} // END the CREATE SNMPGET PDU Function
