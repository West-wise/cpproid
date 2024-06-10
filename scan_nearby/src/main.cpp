#include <cstdio>
#include <cstdint>
#include <pcap.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <map>
#include <set>
#include <atomic>

#include "mac.h"
#include "ip.h"
#include "gtrace.h"
#include "ethhdr.h"
#include "arphdr.h"
#include "utill.h"

void handleSignal(int signal);
void cleanup();
void arp_request(pcap_t *handle, char *dev, Mac my_mac, Ip my_ip, Ip target_ip, std::atomic<bool>& stop);
void arp_reply_listener(char *dev, Mac my_mac, Ip my_ip, Ip target_ip, std::atomic<bool>& stop);
pcap_t *global_handle;

std::set<pcap_t *> g_handle;
std::mutex handle_mutex;
std::vector<std::thread> threads;
std::atomic<bool> stop_threads(false);

// AddressMap 구조체 정의
struct AddressMap {
    Mac mac;
    Ip ip;
};

std::string get_base_ip(const std::string& ip_str) {
    size_t rdot = ip_str.rfind('.');
    if (rdot == std::string::npos) {
        throw std::invalid_argument("Invalid IP format");
    }
    return ip_str.substr(0, rdot + 1);
}

int main(int argc, char *argv[]) {
    gtrace_default("127.0.0.1", 8908, 0 /*stderr*/, "scan.log");

    if (argc <= 4) {
        GTRACE("ARGUMENT COUNT ERROR");
        return -1;
    }
    signal(SIGINT, handleSignal);
    char *dev = argv[1]; // 네트워크 인터페이스 명

    AddressMap atk, sender, target;

    target.ip = Ip(argv[2]);
    atk.ip = Ip(argv[3]);
    atk.mac = Mac(argv[4]);

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = pcap_open_live(dev, BUFSIZ, 1, 1, errbuf);
    if (handle == nullptr) {
        GTRACE("couldn't open device %s(%s)\n", dev, errbuf);
        return -1;
    }
    global_handle = handle;

    {
        std::lock_guard<std::mutex> lock(handle_mutex);
        g_handle.insert(handle);
    }

    std::atomic<bool> stop(false);

    threads.push_back(std::thread(arp_request, handle, dev, atk.mac, atk.ip, target.ip, std::ref(stop)));
    threads.push_back(std::thread(arp_reply_listener, dev, atk.mac, atk.ip, target.ip, std::ref(stop)));

    for (auto& t : threads) {
        t.join();
    }

    cleanup();
    return 0;
}

void handleSignal(int signal) {
    GTRACE("Entered Ctrl+C, exit program\n");
    stop_threads.store(true);

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    cleanup();
    exit(0);
}

void cleanup() {
    GTRACE("pcap close!\n");
    std::lock_guard<std::mutex> lock(handle_mutex);
    for (auto handle : g_handle) {
        pcap_close(handle);
    }
    g_handle.clear();
}

void arp_request(pcap_t *handle, char *dev, Mac my_mac, Ip my_ip, Ip target_ip, std::atomic<bool>& stop) {
    std::string base_ip;
    try {
        base_ip = get_base_ip(std::string(target_ip));
    } catch (const std::invalid_argument& e) {
        GTRACE("%s", e.what());
        return;
    }

    for (int i = 1; i < 255 && !stop.load(); i++) {  // IP 범위 1~254
        std::string ip_enum = base_ip + std::to_string(i);
        for (int k = 0; k <=3 && !stop.load(); k++) {  // 각 IP에 대해 3번 시도
            Ip ip(ip_enum);
            EthArpPacket packet = normal_packet(dev, my_mac, ip, my_ip);
            if (!sendPacket(handle, reinterpret_cast<const u_char*>(&packet), sizeof(EthArpPacket))) {
                GTRACE("sendPacket error");
                continue;
            }
            usleep(10000); // 10ms 지연을 추가하여 네트워크 트래픽을 조절
        }
    }

    // ARP 요청이 끝난 후 stop 플래그를 설정하여 리스너 스레드 종료 신호
    stop.store(true);
}

void arp_reply_listener(char *dev, Mac my_mac, Ip my_ip, Ip target_ip, std::atomic<bool>& stop) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *listen_handle = pcap_open_live(dev, BUFSIZ, 1, 1, errbuf);
    if (listen_handle == nullptr) {
        GTRACE("couldn't open device %s(%s)\n", dev, errbuf);
        return;
    }

    {
        std::lock_guard<std::mutex> lock(handle_mutex);
        g_handle.insert(listen_handle);
    }

    std::map<Ip, Mac> arp_table;
    while (!stop.load()) {
        struct pcap_pkthdr *header;
        const u_char *pkt_data;
        int res = pcap_next_ex(listen_handle, &header, &pkt_data);
        if (res == 0) continue;
        if (res == -1 || res == -2) {
            GTRACE("pcap_next_ex return %d(%s)\n", res, pcap_geterr(listen_handle));
            break;
        }
        EthHdr *eth = (EthHdr *)pkt_data;
        if (eth->type() != EthHdr::Arp) continue;
        ArpHdr *arp = (ArpHdr *)(pkt_data + sizeof(EthHdr));
        if (arp->op() == ArpHdr::Reply && arp->tmac() == my_mac && arp->tip() == my_ip) {
            if (arp_table.find(arp->sip()) != arp_table.end()) {
                continue;
            }
            arp_table[arp->sip()] = arp->smac();
            std::cout << std::string(arp->sip()) << " " << std::string(arp->smac()) << std::endl;
            GTRACE("DETECT : %s(%s)\n", std::string(arp->sip()).c_str(), std::string(arp->smac()).c_str());
        }
    }

    {
        std::lock_guard<std::mutex> lock(handle_mutex);
        g_handle.erase(listen_handle);
    }
    pcap_close(listen_handle);
}
