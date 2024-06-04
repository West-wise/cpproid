#pragma once
#include <cstdint>
#include <string>

#include <sys/types.h>	// 필요한 데이터 타입 정의
#include <sys/socket.h> // socket(), AF_INET, SOCK_DGRAM 등 정의
#include <sys/ioctl.h>	// ioctl() 정의
#include <net/if.h>		// struct ifreq, IFNAMSIZ 등 정의
#include <unistd.h>		// close() 정의
#include <netinet/in.h> // AF_INET 등 정의

struct Ip
{
	static const int SIZE = 4;

	// constructor
	Ip() {}
	Ip(const uint32_t r) : ip_(r) {}
	Ip(const std::string r); // Changed to reference

	// casting operator
	operator uint32_t() const { return ip_; } // default
	explicit operator std::string() const;

	// comparison operator
	bool operator==(const Ip &r) const { return ip_ == r.ip_; }

	bool isLocalHost() const
	{ // 127.*.*.*
		uint8_t prefix = (ip_ & 0xFF000000) >> 24;
		return prefix == 0x7F;
	}

	bool isBroadcast() const
	{ // 255.255.255.255
		return ip_ == 0xFFFFFFFF;
	}

	bool isMulticast() const
	{ // 224.0.0.0 ~ 239.255.255.255
		uint8_t prefix = (ip_ & 0xFF000000) >> 24;
		return prefix >= 0xE0 && prefix < 0xF0;
	}

protected:
	uint32_t ip_;

public:
	static Ip GatewayIp(std::string &);	// Changed to return by value
	static Ip myIp(char *interfaceName); // Changed to return by value
};