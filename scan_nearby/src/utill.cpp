#include "utill.h"

#define MAC_ALEN 6
#define MAC_ADDR_LEN 6

bool sendPacket(pcap_t *handle, const u_char *packet, size_t packetSize) {
    char errbuf[PCAP_ERRBUF_SIZE];
    int res = pcap_sendpacket(handle, packet, packetSize);
    if (res != 0) {
        fprintf(stderr, "pcap_sendpacket return %d error=%s\n", res, pcap_geterr(handle));
        return false;
    }
    return true;
}

EthArpPacket normal_packet(char *interfaceName, Mac my_mac, Ip sip, Ip my_ip)
{
    EthArpPacket packet;
    packet = Make_packet(interfaceName, Mac::broadcastMac(), my_mac, my_mac, my_ip, Mac::nullMac(), sip);
    return packet;
}

EthArpPacket Make_packet(char *interfaceName,
                         Mac eth_dmac,
                         Mac eth_smac,
                         Mac arp_smac,
                         Ip arp_sip,
                         Mac arp_tmac,
                         Ip arp_tip)
{

    EthArpPacket packet;

    char errbuf[PCAP_ERRBUF_SIZE];

    packet.eth_.dmac_ = eth_dmac; // Sender MAC
    packet.eth_.smac_ = eth_smac; // 내 MAC
    packet.eth_.type_ = htons(EthHdr::Arp);

    packet.arp_.hrd_ = htons(ArpHdr::ETHER);
    packet.arp_.pro_ = htons(EthHdr::Ip4);
    packet.arp_.hln_ = Mac::SIZE;
    packet.arp_.pln_ = Ip::SIZE;
    packet.arp_.op_ = htons(ArpHdr::Request);

    packet.arp_.smac_ = arp_smac;      // 내 MAC
    packet.arp_.sip_ = htonl(arp_sip); // gateway ip , Input
    packet.arp_.tmac_ = arp_tmac;      // sender MAC
    packet.arp_.tip_ = htonl(arp_tip); // sender IP

    return packet;
}