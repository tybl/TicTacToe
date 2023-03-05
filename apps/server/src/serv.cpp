#include "tictactoe.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <thread>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static const int port_no = 9987;

// B A TTT L E

auto main() -> int {
  int sockfd = 0;
  int newsockfd = 0;
  int bindfd = 0;
  int listenfd = 0;
  char cli_ip[16];
  char sname[64];
  char cname[64];
  sockaddr_in serv_addr;
  sockaddr_in cli_addr;
  socklen_t cli_size;

  int inp_true = 0, count = 0, inp, ni, x, y, toss;
  char serv_choice, cli_choice, nc;
  char choice_buffer[2], co_ordinates_buffer[16];

  system("clear");

  // creating sever side socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("Server side listening Socket could not be created!");
    return 1;
  }

  memset(&serv_addr, 0, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port_no);
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  // binding socket
  bindfd = bind(sockfd, reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof(serv_addr));
  if (bindfd == -1) {
    perror("Failed to bind!");
    return 1;
  }

  // listening for incoming connections
  std::cout << "Enter your Name : ";
  std::cin >> sname;
  std::cout << "Server created!" << std::endl << "Waiting for a Player..." << std::endl;

  listenfd = listen(sockfd, 5);
  if (listenfd == -1) {
    perror("Failed to listen!");
    return 1;
  }

  cli_size = sizeof(cli_addr);
  newsockfd = accept(sockfd, reinterpret_cast<struct sockaddr*>(&cli_addr), &cli_size);

  if (newsockfd == -1) {
    perror("Failed to accept from client!");
    return 1;
  }

  inet_ntop(AF_INET, &cli_addr.sin_addr, cli_ip, cli_size);
  std::cout << "Server received connections from " << cli_ip << std::endl;

  memset(&cname, 0, sizeof(cname));
  ssize_t bytes_recvd = 0;
  ssize_t bytes_sent = 0;
  do {
    static int flag = 0;
    bytes_recvd = recv(newsockfd, &cname, sizeof(cname), 0);
    if (bytes_recvd == -1 && flag == 0) {
      memset(&cname, 0, sizeof(cname));
      std::cout << "Could not ACQUIRE Player Information!" << std::endl << "Trying again..." << std::endl;
      continue;
    }
    flag = 1;
    bytes_sent = send(newsockfd, &sname, sizeof(sname), 0);
    if (bytes_sent == -1) {
      std::cout << "Could not SEND Player Data!" << "Trying Again..." << std::endl;
    } else {
      std::cout << cname << " has joined the game." << std::endl;
    }
  } while (bytes_recvd == -1 || bytes_sent == -1);

  std::cout << "Creating Game. Please wait..." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::cout << std::endl << "Game created!" << std::endl << std::endl << "Doing a toss...";

  srand(static_cast<unsigned int>(time(nullptr)));
  toss = rand() % 2;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  char toss_buffer = (rand() % 2) ? '0' : '1';
  bytes_sent = send(newsockfd, &toss_buffer, sizeof(toss_buffer), 0);
  if (bytes_sent == -1) {
    perror("TOSS BUFFER not sent!");
    return 1;
  }

  if (toss == 0) {
    std::cout << std::endl << "You WON the toss!" << std::endl;
    do {
      std::cout << sname << " Enter Your Choice (X or O): ";
      std::cin >> serv_choice;
      if (serv_choice == 'X' || serv_choice == 'x') {
        serv_choice = 'X';
        cli_choice = 'O';
        inp_true = 1;
        std::cout << std::endl << cname << " gets O." << std::endl << std::endl << "Lets Play!" << std::endl << std::endl;
      } else if (serv_choice == 'O' || serv_choice == 'o' || serv_choice == '0') {
        serv_choice = 'O';
        cli_choice = 'X';
        inp_true = 1;
        std::cout << std::endl << cname << " gets X." << std::endl << std::endl << "Lets Play!" << std::endl << std::endl;
      } else {
        std::cout << "\nInvalid Choice! Please Choose Again..." << std::endl;
      }
    } while (inp_true == 0);

    memset(&choice_buffer, 0, sizeof(choice_buffer));
    choice_buffer[0] = serv_choice;
    choice_buffer[1] = cli_choice;

    bytes_sent = send(newsockfd, &choice_buffer, sizeof(choice_buffer), 0);
    if (bytes_sent == -1) {
      perror("CHOICE BUFFER could not be sent!");
      return 1;
    }
  } else {
    std::cout << std::endl << cname << " WON the toss." << std::endl;
    std::cout << cname << " is choosing. Please wait..." << std::endl << std::endl;

    memset(&choice_buffer, 0, sizeof(choice_buffer));
    bytes_recvd = recv(newsockfd, &choice_buffer, sizeof(choice_buffer), 0);
    if (bytes_recvd == -1) {
      perror("CHOICE BUFFER not received!");
      return 1;
    } else {
      serv_choice = choice_buffer[0];
      cli_choice = choice_buffer[1];
      std::cout << sname << " has chosen " << serv_choice << std::endl << std::endl << "You will play with " << cli_choice << std::endl;
      std::cout << std::endl << "Lets Play!" << std::endl << std::endl;
    }
  }

  if (serv_choice == 'X') {
    inp = 1;
    std::cout << "You  will play first." << std::endl << std::endl;
  } else {
    inp = 2;
    std::cout << cname << " will play first." << std::endl << std::endl;
  }

  init();
  std::cout << std::endl << "Starting Game..." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(3));
  display();

  while (count < 9) {
    memset(&co_ordinates_buffer, 0, sizeof(co_ordinates_buffer));

    if (inp % 2 != 0) {
      std::cout << std::endl << "Your turn. Enter co-ordinates separated by a space : ";
      std::cin >> x >> y;
      ni = input(serv_choice, x, y);
      if (ni == 0) {
        inp++;
        sprintf(&co_ordinates_buffer[0], "%d", x);
        sprintf(&co_ordinates_buffer[1], "%d", y);
        std::cout << std::endl << "Updating Matrix..." << std::endl;

        bytes_sent = send(newsockfd, &co_ordinates_buffer, 2, 0);
        if (bytes_sent == -1) {
          perror("CO-ORDINATES BUFFER not sent!");
          return 1;
        }
      }
    } else {
      std::cout << std::endl << cname << "'s turn. Please wait..." << std::endl;
      bytes_recvd = recv(newsockfd, &co_ordinates_buffer, sizeof(co_ordinates_buffer), 0);
      if (bytes_recvd == -1) {
        perror("CO-ORDINATES BUFFER not recieved!");
        return 1;
      }
      x = co_ordinates_buffer[0] - '0';
      y = co_ordinates_buffer[1] - '0';
      ni = input(cli_choice, x, y);
      if (ni == 0) {
        inp++;
        std::cout << std::endl << cname << " has played. Updating Matrix..." << std::endl;
      }
    }

    count++;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    system("clear");
    display();

    if (count >= 5) {
      nc = check();
      if (nc == 'f') {
        continue;
      }
      if (serv_choice == nc) {
        std::cout << std::endl << "Congrats! You have won!!!" << std::endl << cname << " lost." << std::endl;
        break;
      }
      if (cli_choice == nc) {
        std::cout << std::endl << "You loose." << std::endl << cname << " has won." << std::endl;
        break;
      }
    }
  }

  if (nc == 'f') {
    std::cout << std::endl << "Game ends in a draw." << std::endl;
  }
  std::cout << std::endl << "Thank You for playing Tic-tac-Toe" << std::endl;
  close(newsockfd);
  close(sockfd);
  return 0;
}
