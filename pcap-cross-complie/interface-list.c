#include <pcap.h>
// #include "/home/kali/Android/pcap/sysroot/include/pcap.h"
#include <stdio.h>

int main() {
    char error[PCAP_ERRBUF_SIZE];
    pcap_if_t* all;
    pcap_if_t* temp;
    int i;
    if(pcap_findalldevs(&all, error) == -1) {
        printf("error in pcap_findalldevs(%s)\n", error);
        return -1;    
    }
    i = 1;
    for(temp = all; temp; temp = temp->next) {
        printf("%d : %s\n",i++,temp->name);
    }
    return 0;
}
