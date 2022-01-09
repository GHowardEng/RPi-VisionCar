///////////////////////////////////////////////////////////////////
// Prepared for BCIT ELEX4618 by Craig Hennessey
///////////////////////////////////////////////////////////////////
#pragma once

//#define WIN4618
#define PI4618

#include <iostream>
#include <string>
#include <mutex>
#include <vector>

// OpenCV Include
#ifdef WIN4618
#include "opencv.hpp"
#endif
#ifdef PI4618
#include <opencv2/opencv.hpp>
#endif

#ifdef WIN4618
#include "Winsock2.h"
#pragma comment(lib, "ws2_32.lib")
#endif

#ifdef PI4618
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
typedef int SOCKET;
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#endif


#define BUFF_SIZE 65535

class Server
{
private:
  
  cv::Mat _txim;
 
  std::mutex _immutex;
  std::mutex _cmdmutex;
  std::vector<std::string> _cmd_list;

  SOCKET _socket;

  char rxbuff[BUFF_SIZE];
 
public:
  Server();
    ~Server();
    
    bool _exit;
    SOCKET clientsock = 0;
    
    std::string str;
    
    std::string reply;

  // Start server listening (probably best to do in a separate thread)
  void start(int port);

  // Set the image to transmit
  void set_txim (cv::Mat &im);

  // Get a vector of the commands the server has read
  void get_cmd (std::vector<std::string> &cmds);
  
  void tx_str(std::string str);

};


