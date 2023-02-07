#include <string>

struct game_connector {
  virtual ~game_connector() = 0;
  virtual void sendto(std::string const& data) = 0;
  virtual std::string recvfrom() = 0;
};

game_connector::~game_connector() {}

#include <vector>

class test_connector
  : public game_connector
{
  std::size_t m_iteration;
  std::vector<std::string> m_input;
  std::vector<std::string> m_expected_output;

public:
  test_connector(std::vector<std::string> const& input,
                 std::vector<std::string> const& expected_output)
    : m_iteration(0)
    , m_input(input)
    , m_expected_output(expected_output)
  { }

  ~test_connector() override {
  }

  void sendto(std::string const& data) override {
  }

  std::string recvfrom() override {
    return m_input.at(m_iteration);
  }
};

#include <unistd.h>

class network_connector
  : public game_connector
{
  int m_socket;
public:
  ~network_connector() override {
    close(m_socket);
  }

  void sendto(std::string const&) override {
    //ssize_t retval = sendto(m_socket, const void buf[.len], size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
    //if (-1 == retval) {
      //perror("Error: Call to sendto() failed");
    //}
  }

  std::string recvfrom() override {
    //int recvfrom(m_socket, void *buf, int len, unsigned int flags, struct sockaddr *from, int *fromlen);
    return 0;
  }
};

#include <memory>

class game {
  game_connector& m_connector;

public:
  explicit game(game_connector& conn)
    : m_connector(conn) { }

  void run() {
    // if address is provided
    //    sendto(address, "game_id")
    // while(true)
    //    (game_id, move) = recvfrom()
    //    game = map[game_id];
    //    if (valid != game.apply(move))
    //       sendto(game.remote_ip, "invalid move")
    //       delete map[game_id]
    //       continue
    //    if (!game.finished())
    //       my_move = make_move(game)
    //       sendto(game.remote_ip, my_move)
  }
};

int main() {
  test_connector conn({},{});
  game my_game(conn);
  my_game.run();
}
