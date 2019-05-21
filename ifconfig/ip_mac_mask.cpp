QString GetLocalIp()  
{  
  
    int sock_get_ip;  
    char ipaddr[50];  
  
    struct   sockaddr_in *sin;  
    struct   ifreq ifr_ip;     
  
    if ((sock_get_ip=socket(AF_INET, SOCK_STREAM, 0)) == -1)  
    {  
         printf("socket create failse...GetLocalIp!/n");  
         return "";  
    }  
     
    memset(&ifr_ip, 0, sizeof(ifr_ip));     
    strncpy(ifr_ip.ifr_name, "eth0", sizeof(ifr_ip.ifr_name) - 1);     
   
    if( ioctl( sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0 )     
    {     
         return "";     
    }       
    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;     
    strcpy(ipaddr,inet_ntoa(sin->sin_addr));         
      
    printf("local ip:%s /n",ipaddr);      
    close( sock_get_ip );  
      
    return QString( ipaddr );  
}

int SetLocalIp( const char *ipaddr )  
{  
  
    int sock_set_ip;  
      
    struct sockaddr_in sin_set_ip;  
    struct ifreq ifr_set_ip;  
  
    bzero( &ifr_set_ip,sizeof(ifr_set_ip));  
   
    if( ipaddr == NULL )  
        return -1;  
  
    if(sock_set_ip = socket( AF_INET, SOCK_STREAM, 0 ) == -1);  
    {  
        perror("socket create failse...SetLocalIp!/n");  
        return -1;  
    }  
   
    memset( &sin_set_ip, 0, sizeof(sin_set_ip));  
    strncpy(ifr_set_ip.ifr_name, "eth0", sizeof(ifr_set_ip.ifr_name)-1);     
      
    sin_set_ip.sin_family = AF_INET;  
    sin_set_ip.sin_addr.s_addr = inet_addr(ipaddr);  
    memcpy( &ifr_set_ip.ifr_addr, &sin_set_ip, sizeof(sin_set_ip));  
  
    if( ioctl( sock_set_ip, SIOCSIFADDR, &ifr_set_ip) < 0 )  
    {  
        perror( "Not setup interface/n");  
        return -1;  
    }  
  
    //设置激活标志  
    ifr_set_ip.ifr_flags |= IFF_UP |IFF_RUNNING;  
  
    //get the status of the device  
    if( ioctl( sock_set_ip, SIOCSIFFLAGS, &ifr_set_ip ) < 0 )  
    {  
         perror("SIOCSIFFLAGS");  
         return -1;  
    }  
  
    close( sock_set_ip );  
    return 0;  
}

QString GetLocalMac()  
{  
    int sock_mac;  
      
    struct ifreq ifr_mac;  
    char mac_addr[30];     
      
    sock_mac = socket( AF_INET, SOCK_STREAM, 0 );  
    if( sock_mac == -1)  
    {  
        perror("create socket falise...mac/n");  
        return "";  
    }  
      
    memset(&ifr_mac,0,sizeof(ifr_mac));     
    strncpy(ifr_mac.ifr_name, "eth0", sizeof(ifr_mac.ifr_name)-1);     
  
    if( (ioctl( sock_mac, SIOCGIFHWADDR, &ifr_mac)) < 0)  
    {  
        printf("mac ioctl error/n");  
        return "";  
    }  
      
    sprintf(mac_addr,"%02x%02x%02x%02x%02x%02x",  
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[0],  
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[1],  
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[2],  
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[3],  
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[4],  
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[5]);  
  
    printf("local mac:%s /n",mac_addr);      
      
    close( sock_mac );  
    return QString( mac_addr );  
}
QString GetLocalNetMask()  
{  
    int sock_netmask;  
    char netmask_addr[50];  
  
    struct ifreq ifr_mask;  
    struct sockaddr_in *net_mask;  
          
    sock_netmask = socket( AF_INET, SOCK_STREAM, 0 );  
    if( sock_netmask == -1)  
    {  
        perror("create socket failture...GetLocalNetMask/n");  
        return "";  
    }  
      
    memset(&ifr_mask, 0, sizeof(ifr_mask));     
    strncpy(ifr_mask.ifr_name, ifname, sizeof(ifr_mask.ifr_name )-1);     
  
    if( (ioctl( sock_netmask, SIOCGIFNETMASK, &ifr_mask ) ) < 0 )   
    {  
        printf("mac ioctl error/n");  
        return "";  
    }  
      
    net_mask = ( struct sockaddr_in * )&( ifr_mask.ifr_netmask );  
    strcpy( netmask_addr, inet_ntoa( net_mask -> sin_addr ) );  
      
    printf("local netmask:%s/n",netmask_addr);      
      
    close( sock_netmask );  
    return QString( netmask_addr );  
}

QString SetLocalNetMask(const char *szNetMask)  
{  
    int sock_netmask;  
    char netmask_addr[32];     
  
    struct ifreq ifr_mask;  
    struct sockaddr_in *sin_net_mask;  
          
    sock_netmask = socket( AF_INET, SOCK_STREAM, 0 );  
    if( sock_netmask == -1)  
    {  
        perror("Not create network socket connect/n");  
        return "";  
    }  
      
    memset(&ifr_mask, 0, sizeof(ifr_mask));     
    strncpy(ifr_mask.ifr_name, "eth0", sizeof(ifr_mask.ifr_name )-1);     
    sin_net_mask = (struct sockaddr_in *)&ifr_mask.ifr_addr;  
    sin_net_mask -> sin_family = AF_INET;  
    inet_pton(AF_INET, szNetMask, &sin_net_mask ->sin_addr);  
  
    if(ioctl(sock_netmask, SIOCSIFNETMASK, &ifr_mask ) < 0)   
    {  
        printf("sock_netmask ioctl error/n");  
        return "";  
    }  
}
QString GetGateWay()  
{  
    FILE *fp;  
    char buf[512];  
    char cmd[128];  
    char gateway[30];  
    char *tmp;  
  
    strcpy(cmd, "ip route");  
    fp = popen(cmd, "r");  
    if(NULL == fp)  
    {  
        perror("popen error");  
        return "";  
    }  
    while(fgets(buf, sizeof(buf), fp) != NULL)  
    {  
        tmp =buf;  
        while(*tmp && isspace(*tmp))  
            ++ tmp;  
        if(strncmp(tmp, "default", strlen("default")) == 0)  
            break;  
    }  
    sscanf(buf, "%*s%*s%s", gateway);         
    printf("default gateway:%s/n", gateway);  
    pclose(fp);  
      
    return QString(gateway);  
}
int SetGateWay(const char *szGateWay)  
{  
    int ret = 0;      
    char cmd[128];  
    QString DefGW = GetGateWay();  
  
    const char *strGW = DefGW.latin1();   
      
    strcpy(cmd, "route del default gw ");  
    strcat(cmd, strGW);  
    ret = system(cmd);  
    if(ret < 0)  
    {  
        perror("route error");  
        return -1;  
    }  
    strcpy(cmd, "route add default gw ");  
    strcat(cmd, szGateWay);  
      
    ret = system(cmd);  
    if(ret < 0)  
    {  
        perror("route error");  
        return -1;  
    }  
  
    return ret;  
}
#include <stdio.h>
#include <string.h>
#include <errno.h>

int GetNetCardCount()
{
    int nCount = 0;
    FILE* f = fopen("/proc/net/dev", "r");
    if (!f)
    {
        fprintf(stderr, "Open /proc/net/dev failed!errno:%d\n", errno);
        return nCount;
    }

    char szLine[512];

    fgets(szLine, sizeof(szLine), f);    /* eat line */
    fgets(szLine, sizeof(szLine), f);

    while(fgets(szLine, sizeof(szLine), f))
    {
        char szName[128] = {0};
        sscanf(szLine, "%s", szName);
        int nLen = strlen(szName);
        if (nLen <= 0)continue;
        if (szName[nLen - 1] == ':') szName[nLen - 1] = 0;
        if (strcmp(szName, "lo") == 0)continue;
        nCount++;
    }

    fclose(f);
    f = NULL;
    return nCount;
}
void DispNetInfo(const char* szDevName)
{
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        fprintf(stderr, "Create socket failed!errno=%d", errno);
        return;
    }

    struct ifreq ifr;
    unsigned char mac[6];
    unsigned long nIP, nNetmask, nBroadIP;

    printf("%s:\n", szDevName);

    strcpy(ifr.ifr_name, szDevName);
    if (ioctl(s, SIOCGIFHWADDR, &ifr) < 0)
    {
        return;
    }
    memcpy(mac, ifr.ifr_hwaddr.sa_data, sizeof(mac));
    printf("\tMAC: %02x-%02x-%02x-%02x-%02x-%02x\n",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    strcpy(ifr.ifr_name, szDevName);
    if (ioctl(s, SIOCGIFADDR, &ifr) < 0)
    {
        nIP = 0;
    }
    else
    {
        nIP = *(unsigned long*)&ifr.ifr_broadaddr.sa_data[2];
    }
    printf("\tIP: %s\n", inet_ntoa(*(in_addr*)&nIP));

    strcpy(ifr.ifr_name, szDevName);
    if (ioctl(s, SIOCGIFBRDADDR, &ifr) < 0)
    {
        nBroadIP = 0;
    }
    else
    {
        nBroadIP = *(unsigned long*)&ifr.ifr_broadaddr.sa_data[2];
    }
    printf("\tBroadIP: %s\n", inet_ntoa(*(in_addr*)&nBroadIP));

    strcpy(ifr.ifr_name, szDevName);
    if (ioctl(s, SIOCGIFNETMASK, &ifr) < 0)
    {
        nNetmask = 0;
    }
    else
    {
        nNetmask = *(unsigned long*)&ifr.ifr_netmask.sa_data[2];
    }
    printf("\tNetmask: %s\n", inet_ntoa(*(in_addr*)&nNetmask));
    close(s);
}
