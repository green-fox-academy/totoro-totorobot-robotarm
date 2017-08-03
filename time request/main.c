/*
 *
 * David Lettier (C) 2014.
 *
 * http://www.lettier.com/
 *
 * NTP client.
 *
 * Compiled with gcc version 4.7.2 20121109 (Red Hat 4.7.2-8) (GCC).
 *
 * Tested on Linux 3.8.11-200.fc18.x86_64 #1 SMP Wed May 1 19:44:27 UTC 2013 x86_64 x86_64 x86_64 GNU/Linux.
 *
 * To compile: $ gcc main.c -o ntpClient.out
 *
 * Usage: $ ./ntpClient.out
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define bcopy(b1,b2,len) (memmove((b2), (b1), (len)), (void) 0)
#include <stdint.h>
#include <winsock.h>

#define NTP_TIMESTAMP_DELTA 2208988800ull

int main(int argc,char** argv)
{
   int      sockfd;
   int      n;             // Socket file descriptor and the n return result from writing/reading from the socket.
   int      portno = 123;  // NTP UDP port number.

   char*    host_name = "europe.pool.ntp.org"; // NTP server host-name.

   // Structure that defines the 48 byte NTP packet protocol.
   // Check TWICE size of fields !!
typedef struct
{
      BYTE li   : 2;          // Only two bits. Leap indicator.
      BYTE vn   : 3;          // Only three bits. Version number of the protocol.
      BYTE mode : 3;          // Only three bits. Mode. Client will pick mode 3 for client.

      BYTE stratum;           // Eight bits. Stratum level of the local clock.
      BYTE poll;              // Eight bits. Maximum interval between successive messages.
      BYTE precision;         // Eight bits. Precision of the local clock.

      DWORD rootDelay;        // 32 bits. Total round trip delay time.
      DWORD rootDispersion;   // 32 bits. Max error aloud from primary clock source.
      DWORD refId;            // 32 bits. Reference clock identifier.

      DWORD refTm_s;          // 32 bits. Reference time-stamp seconds.
      DWORD refTm_f;          // 32 bits. Reference time-stamp fraction of a second.

      DWORD origTm_s;         // 32 bits. Originate time-stamp seconds.
      DWORD origTm_f;         // 32 bits. Originate time-stamp fraction of a second.

      DWORD rxTm_s;           // 32 bits. Received time-stamp seconds.
      DWORD rxTm_f;           // 32 bits. Received time-stamp fraction of a second.

      DWORD txTm_s;           // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
      DWORD txTm_f;           // 32 bits. Transmit time-stamp fraction of a second.

}ntp_packet;                         // Total: 384 bits or 48 bytes.

   // Create and zero out the packet. All 48 bytes worth.
   ntp_packet packet = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

   memset(&packet,0,sizeof(ntp_packet));

   // Set the first byte's bits to 00,011,011 for li = 0,vn = 3,and mode = 3. The rest will be left set to zero.
   *((char*)&packet + 0) = 0x1B; // Represents 27 in base 10 or 00011011 in base 2.

   // Create a UDP socket, convert the host-name to an IP address, set the port number,
   // connect to the server,send the packet,and then read in the return packet.
   struct sockaddr_in  serv_addr;  // Server address data structure.
   struct hostent *server;     // Server data structure.

   WORD              wVersionRequested;
   WSADATA           wsaData;
   int               iErr;

   // Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
   wVersionRequested = MAKEWORD(2,2);

   iErr = WSAStartup(wVersionRequested,&wsaData);

   if (iErr)
   {
    printf("error");
   }

   sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP); // Create a UDP socket.

   if (sockfd < 0)
   {
    printf("UDP Socket error");
    }

   server = gethostbyname(host_name); // Convert URL to IP.

   if (!server)
   {
      printf("url ip error");
   }

   // Zero out the server address structure.
   memset(&serv_addr,0,sizeof(serv_addr));

   serv_addr.sin_family = AF_INET;

   // Copy the server's IP address to the server address structure.
   memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);

   // Convert the port number integer to network big-endian style and save it to the server address structure.
   serv_addr.sin_port = htons((WORD)portno);

   // Call up the server using its IP address and port number.
   if (connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
   {
    printf("error");
   }

   // Send it the NTP packet it wants. If n == -1, it failed.
   n = sendto(sockfd,(char*)&packet,sizeof(ntp_packet),0,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

   if (n < 0)
   {
    printf("error");
   }

   iErr = shutdown(sockfd,SD_SEND);

   if (iErr == SOCKET_ERROR)
   {
    printf("error");
   }

   // Wait and receive the packet back from the server. If n == -1, it failed.
   n = recv(sockfd,(char*)&packet,sizeof(ntp_packet),0);

   if (n < 0)
   {
    printf("error");
   }

   // These two fields contain the time-stamp seconds as the packet left the NTP server.
   // The number of seconds correspond to the seconds passed since 1900.
   // ntohl() converts the bit/byte order from the network's to host's "endianness".
   packet.txTm_s = ntohl(packet.txTm_s); // Time-stamp seconds.
   packet.txTm_f = ntohl(packet.txTm_f); // Time-stamp fraction of a second.

   // Extract the 32 bits that represent the time-stamp seconds (since NTP epoch) from when the packet left the server.
   // Subtract 70 years worth of seconds from the seconds since 1900.
   // This leaves the seconds since the UNIX epoch of 1970.
   // (1900)------------------(1970)**************************************(Time Packet Left the Server)
   time_t   txTm = (time_t)(packet.txTm_s - NTP_TIMESTAMP_DELTA);

   // Print the time we got from the server,accounting for local timezone and conversion from UTC time.
   printf("Time: %s",ctime((const time_t*)&txTm));

   closesocket(sockfd);
   WSACleanup();

   return 0;
}
