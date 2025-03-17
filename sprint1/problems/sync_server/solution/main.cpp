/*
#include <sdkddkver.h>
#define BOOST_DISABLE_CURRENT_LOCATI
#include <boost/asio.hpp>

#include <iostream>
#include <string>
#include <string_view>



namespace net = boost::asio;
using net::ip::tcp;

using namespace std::literals;

int serv() {
    static const int port = 3333;

    net::io_context io_context;

    // ���������� ����������� tcp::v4 �� ��������� ��� ������ 0.0.0.0
    tcp::acceptor acceptor(io_context, tcp::endpoint(net::ip::make_address("127.0.0.1"), port));
    std::cout << "Waiting for connection..."sv << std::endl;
    

    boost::system::error_code ec;
    tcp::socket socket{io_context};
    acceptor.accept(socket, ec);

    if (ec) {
        std::cout << "Can't accept connection"sv << std::endl;
        return 1;
    }

    // �������� � socket   

    net::streambuf stream_buf;
    net::read_until(socket, stream_buf, '\n', ec);
    std::string client_data{std::istreambuf_iterator<char>(&stream_buf),
        std::istreambuf_iterator<char>()};

    if (ec) {
        std::cout << "Error reading data"sv << std::endl;
        return 1;
    }

    std::cout << "Client said: "sv << client_data << std::endl;

    socket.write_some(net::buffer("Hello, I'm server!\n"sv), ec);

    if (ec) {
        std::cout << "Error sending data"sv << std::endl;
        return 1;
    }
}

int clnt(const char* ip) {
    static const int port = 3333;

    std::cout << "Client started" << std::endl;
    // �������� endpoint - ������ � ����������� �� ������ � �����.
    // ��� ������� IP-������ ���������� �������� net::ip::make_address.
    boost::system::error_code ec;
    auto endpoint = tcp::endpoint(net::ip::make_address(ip, ec), port);

    if (ec) {
        std::cout << "Wrong IP format"sv << std::endl;
        return 1;
    }

    // int main(...)
    net::io_context io_context;
    tcp::socket socket{io_context};
    socket.connect(endpoint, ec);

    if (ec) {
        std::cout << "Can't connect to server"sv << std::endl;
        return 1;
    }
   

// ���������� ������ � ���������, ��� ��� ������.
    socket.write_some(net::buffer("Hello, I'm client!\n"sv), ec);
    if (ec) {
        std::cout << "Error sending data"sv << std::endl;
        return 1;
    }

    net::streambuf stream_buf;
    net::read_until(socket, stream_buf, '\n', ec);
    std::string server_data{std::istreambuf_iterator<char>(&stream_buf),
        std::istreambuf_iterator<char>()};

    if (ec) {
        std::cout << "Error reading data"sv << std::endl;
        return 1;
    }

    std::cout << "Server responded: "sv << server_data << std::endl;
}

using net::ip::udp;
void servUDP()
{
    static const int port = 3333;
    static const size_t max_buffer_size = 1024;

    try {
        boost::asio::io_context io_context;

        udp::socket socket(io_context, udp::endpoint(net::ip::make_address("127.0.0.1"), port));

        // ��������� ������ � �����, ����� ����� ���� �������� �� ������� ���������
        for (;;) {
            // ������ ����� ������������ �������, ����� �������� ����������.
            std::array<char, max_buffer_size> recv_buf;
            udp::endpoint remote_endpoint;

            // �������� �� ������ ������, �� � endpoint �������
            auto size = socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint);

            std::cout << "Client said "sv << std::string_view(recv_buf.data(), size) << std::endl;

            // ���������� ����� �� ���������� endpoint, ��������� ������.
            // �� ���� ��� �� ���������� ������� ������: ������ ���������� ����� ������� �������������.
            boost::system::error_code ignored_error;
            socket.send_to(boost::asio::buffer("Hello from UDP-server"sv), remote_endpoint, 0, ignored_error);
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;

    }
}

void clientUDP(const char* ip)
{
    static const int port = 3333;
    static const size_t max_buffer_size = 1024;
   

    try {
        net::io_context io_context;

        // ����� ��������� ������ ����� ������� �����. 
        // ��� �������� ��������� �������� (IPv4 ��� IPv6) ������ endpoint.
        udp::socket socket(io_context, udp::v4());

        boost::system::error_code ec;
        auto endpoint = udp::endpoint(net::ip::make_address(ip, ec), port);
        socket.send_to(net::buffer("Hello from UDP-client"sv), endpoint);

        // �������� ������ � endpoint.
        std::array<char, max_buffer_size> recv_buf;
        udp::endpoint sender_endpoint;
        size_t size = socket.receive_from(net::buffer(recv_buf), sender_endpoint);

        std::cout << "Server responded "sv << std::string_view(recv_buf.data(), size) << std::endl;
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
*/
/*
int main(int argc, char** argv)
{
    std::cout << "Hello" << std::endl;
    if(argv[1][0] == 'c')
    {
        clnt("127.0.0.1");
    }     
    if (argv[1][0] == 's')
    {
        serv();
    }
    if (argv[1][0] == 'd')
    {
        clientUDP("127.0.0.1");
    }
    if (argv[1][0] == 't')
    {
        servUDP();
    }
}
*/