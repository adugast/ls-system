#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/wireless.h>

// check in /usr/include/bits/ioctls.h
// <include/linux/sockios.h>
// 1) getifaadrs/freeifaddrs -> linkedlist for network interface
// 2) ioctl(fd, SIOCGIFINDEX, struct ifreq *); -> get interface index for each
// 3) ioctl (fd, SIOCGIFNAME, struct ifreq *) -> in all case fd -> AF_INET socket
// is ARP table useful ? arp -an -> link between macaddr and ipaddr
// man (2) ioctl_list
// ioctl to check if wireless or not: include <linux/wireless.h>
// vim /usr/include/linux/wireless.h
// #define SIOCGIWSTATS>---0x8B0F>->---/* Get /proc/net/wireless stats */
// #define SIOCSIWSCAN>0x8B18>->---/* trigger scanning (list cells) */
// #define SIOCGIWSCAN>0x8B19>->---/* get scanning results */


static int get_iface_info(int fd, const char *ifname, int request, struct ifreq *ifr)
{
    strncpy(ifr->ifr_name, ifname, IFNAMSIZ);
    return ioctl(fd, request, ifr);
}


static int entry()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return -1;
    }

    struct ifaddrs *ifap;
    if (getifaddrs(&ifap) == -1) {
        perror("getifaddrs");
        return -1;
    }

    struct ifaddrs *nodep;
    for (nodep = ifap; nodep != NULL; nodep = nodep->ifa_next) {

        /* case where a network interface is removed the name can be null temporarly*/
        if (nodep->ifa_name == NULL)
            continue;

        switch (nodep->ifa_addr->sa_family) {
            case AF_INET:
                printf("ipv4:%s:%d:%d\n", nodep->ifa_name, nodep->ifa_flags, nodep->ifa_addr->sa_family);
                break;
            case AF_INET6:
                printf("ipv6:%s:%d:%d\n", nodep->ifa_name, nodep->ifa_flags, nodep->ifa_addr->sa_family);
                break;
            case AF_LOCAL:
                printf("iplocal:%s:%d:%d\n", nodep->ifa_name, nodep->ifa_flags, nodep->ifa_addr->sa_family);
                break;
            default:
                printf("wat?:%s:%d:%d\n", nodep->ifa_name, nodep->ifa_flags, nodep->ifa_addr->sa_family);
        };

//        struct ifreq ifr = {0};
  //      if (get_iface_info(fd, nodep->ifa_name, SIOCGIFADDR, &ifr) == -1) {
            // perror("get_iface_info");
            // return -1;
    //    }
        //struct sockaddr_in *ipaddr = (struct sockaddr_in *)&ifr.ifr_addr;
       // printf("%s: inet: %s\n", nodep->ifa_name, inet_ntoa(ipaddr->sin_addr));

    }

    freeifaddrs(ifap);
    close(fd);

    return 0;
}

/////////////////////////////////////////////////////////////////////


int usr_get_name(char *buffer, size_t size)
{
    if (getlogin_r(buffer, size) == -1) {
        perror("getlogin_r");
        return -1;
    }

    return 0;
}


/////////////////////////////////////////////////////////////////////

#include <sys/utsname.h>

enum uts_e {
    UTS_SYSNAME,
    UTS_NODENAME,
    UTS_RELEASE,
    UTS_VERSION,
    UTS_MACHINE
};

static int uts_get_field(char *buffer, size_t size, enum uts_e field)
{

    struct utsname u = {0};

    if (uname(&u) == -1) {
        perror("utsname");
        return -1;
    }

    switch (field) {
        case UTS_SYSNAME:
            strncpy(buffer, u.sysname, size);
            break;
        case UTS_NODENAME:
            strncpy(buffer, u.nodename, size);
            break;
        case UTS_RELEASE:
            strncpy(buffer, u.release, size);
            break;
        case UTS_VERSION:
            strncpy(buffer, u.version, size);
            break;
        case UTS_MACHINE:
            strncpy(buffer, u.machine, size);
            break;
        default:
            fprintf(stderr, "uts_get_field: field not found\n");
            return -1;
    }

    return 0;
}

int uts_get_sysname(char **buffer)
{
    char *sysname = calloc(1, _UTSNAME_SYSNAME_LENGTH);
    *buffer = sysname;
    return uts_get_field(sysname, _UTSNAME_SYSNAME_LENGTH, UTS_SYSNAME);
}

int uts_get_nodename(char **buffer)
{
    char *nodename = calloc(1, _UTSNAME_NODENAME_LENGTH);
    *buffer = nodename;
    return uts_get_field(nodename, _UTSNAME_NODENAME_LENGTH, UTS_NODENAME);
}

int uts_get_release(char **buffer)
{
    char *release = calloc(1, _UTSNAME_RELEASE_LENGTH);
    *buffer = release;
    return uts_get_field(release, _UTSNAME_RELEASE_LENGTH, UTS_RELEASE);
}

int uts_get_version(char **buffer)
{
    char *version = calloc(1, _UTSNAME_VERSION_LENGTH);
    *buffer = version;
    return uts_get_field(version, _UTSNAME_VERSION_LENGTH, UTS_VERSION);
}

int uts_get_machine(char **buffer)
{
    char *machine = calloc(1, _UTSNAME_MACHINE_LENGTH);
    *buffer = machine;
    return uts_get_field(machine, _UTSNAME_MACHINE_LENGTH, UTS_MACHINE);
}

/////////////////////////////////////////////////////////////////////

#include <sys/sysinfo.h>

static int sys_get_field()
{
    struct sysinfo s = {0};

    if (sysinfo(&s) == -1) {
        perror("sysinfo");
        return -1;
    }

    // uptime
    // free/total ram
    // free/total swap
    // nbr of current processes

    return 0;
}
