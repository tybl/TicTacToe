#include <fmt/core.h>

#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>

struct game_connector {
  virtual ~game_connector() = 0;
  virtual void sendto(std::string const& data) = 0;
  virtual std::string recvfrom() = 0;
};

game_connector::~game_connector() {}


class test_connector
  : public game_connector
{
  std::size_t m_input_iteration;
  std::size_t m_output_iteration;
  std::vector<std::string> m_input;
  std::vector<std::string> m_expected_output;

public:
  test_connector(std::vector<std::string> const& input,
                 std::vector<std::string> const& expected_output)
    : m_input_iteration(0)
    , m_output_iteration(0)
    , m_input(input)
    , m_expected_output(expected_output)
  { }

  ~test_connector() override {
  }

  void sendto(std::string const& data) override {
    if (data != m_expected_output.at(m_output_iteration)) {
      std::cerr << "sendto() called with different value than expected:\n"
                << "\tExpected: " << m_expected_output.at(m_output_iteration)
                << "\n\tReceived: " << data << std::endl;
    }
    m_output_iteration++;
  }

  std::string recvfrom() override {
    return m_input.at(m_input_iteration++);
  }
};


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

namespace ttt {

  class move {
    int m_row;
    int m_col;
    char m_player;

  public:
    move(int row, int col, char player)
      : m_row(row)
      , m_col(col)
      , m_player(player)
    {
      if (0 > m_row || m_row > 2) {
        throw "something";
      }
      if (0 > m_col || m_col > 2) {
        throw "something";
      }
      if (m_player != 'X' && m_player != 'O') {
        throw "something";
      }
    }

    auto row() const -> int { return m_row; }

    auto col() const -> int { return m_col; }

    auto player() const -> int { return m_player; }

  };

  class board {
    char m_board[3][3] = {0};

  public:
    void display() const {
      fmt::print("{} | {} | {}\n", m_board[0][0], m_board[0][1], m_board[0][2]);
      fmt::print("{} | {} | {}\n", m_board[1][0], m_board[1][1], m_board[1][2]);
      fmt::print("{} | {} | {}\n", m_board[2][0], m_board[2][1], m_board[2][2]);
    }

    void apply(move m) {
      m_board[m.row()][m.col()] = m.player();
    }

    auto has_winner() const -> bool {
      return (m_board[0][0] == m_board[0][1] && m_board[0][1] == m_board[0][2])
          || (m_board[1][0] == m_board[1][1] && m_board[1][1] == m_board[1][2])
          || (m_board[2][0] == m_board[2][1] && m_board[2][1] == m_board[2][2])

          || (m_board[0][0] == m_board[1][0] && m_board[1][0] == m_board[2][0])
          || (m_board[0][1] == m_board[1][1] && m_board[1][1] == m_board[2][1])
          || (m_board[0][2] == m_board[1][2] && m_board[1][2] == m_board[2][2])

          || (m_board[0][0] == m_board[1][1] && m_board[1][1] == m_board[2][2])
          || (m_board[0][2] == m_board[1][1] && m_board[1][1] == m_board[2][0]);
    }

  }; // class board

} // namespace ttt

class game {
  game_connector& m_connector;

public:
  explicit game(game_connector& conn)
    : m_connector(conn) { }

  void run() {
    auto input = m_connector.recvfrom();
    m_connector.sendto("output1");
    input = m_connector.recvfrom();
    m_connector.sendto("output2");
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
private:
  ttt::move make_move() {
    return ttt::move(0,0,'O');
  }
};

int main() {
  test_connector conn({"input1", "input2"},{"output1", "output2"});
  game my_game(conn);
  my_game.run();
}
