#include <pcap.h>
#include <stdio.h>

// 패킷을 처리할 콜백 함수
void packet_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    // 패킷 처리 로직
    printf("Captured a packet with length of [%d]\n", pkthdr->len);
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    
    // 네트워크 인터페이스를 열고 패킷 캡처를 시작 (타임아웃을 1000밀리초로 설정)
    handle = pcap_open_live("eth0", BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device: %s\n", errbuf);
        return 2;
    }

    // 필터 설정 (예: TCP 패킷만 캡처)
    struct bpf_program fp;
    char filter_exp[] = "tcp";
    if (pcap_compile(handle, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return 2;
    }
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return 2;
    }

    // 패킷 캡처 시작 (10개의 패킷을 캡처)
    pcap_loop(handle, 10, packet_handler, NULL);

    // 캡처 종료 후 리소스 해제
    pcap_freecode(&fp);
    pcap_close(handle);

    return 0;
}
