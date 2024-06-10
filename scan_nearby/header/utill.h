#pragma once

#include <pcap.h>
#include <iostream>
#include "ethhdr.h"
#include "arphdr.h"
#include  "ip.h"
#include "mac.h"



#pragma pack(push, 1)
struct EthArpPacket final
{
    EthHdr eth_;
    ArpHdr arp_;
};
#pragma pack(pop)

void usage();

void print_info(struct libnet_ipv4_hdr *header,
                u_int8_t *m,
                u_int8_t *m2);


EthArpPacket normal_packet(char *interfaceName, Mac my_mac, Ip sip, Ip my_ip);

EthArpPacket Make_packet(char *interfaceName,
                         Mac eth_dmac,
                         Mac eth_smac,
                         Mac arp_smac,
                         Ip arp_sip,
                         Mac arp_tmac,
                         Ip arp_tip);
bool sendPacket(pcap_t *handle, const u_char *packet, size_t packetSize);