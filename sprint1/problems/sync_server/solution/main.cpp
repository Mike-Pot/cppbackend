
#ifdef WIN32
#include <sdkddkver.h>
#endif
// boost.beast áóäåò èñïîëüçîâàòü std::string_view âìåñòî boost::string_view
#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <thread>
#include <optional>

namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::literals;
namespace beast = boost::beast;
namespace http = beast::http;
// Çàïðîñ, òåëî êîòîðîãî ïðåäñòàâëåíî â âèäå ñòðîêè
using StringRequest = http::request<http::string_body>;
// Îòâåò, òåëî êîòîðîãî ïðåäñòàâëåíî â âèäå ñòðîêè
using StringResponse = http::response<http::string_body>;

std::optional<StringRequest> ReadRequest(tcp::socket& socket, beast::flat_buffer& buffer) {
    beast::error_code ec;
    StringRequest req;
    // Ñ÷èòûâàåì èç socket çàïðîñ req, èñïîëüçóÿ buffer äëÿ õðàíåíèÿ äàííûõ.
    // Â ec ôóíêöèÿ çàïèøåò êîä îøèáêè.
    http::read(socket, buffer, req, ec);

    if (ec == http::error::end_of_stream) {
        return std::nullopt;
    }
    if (ec) {
        throw std::runtime_error("Failed to read request: "s.append(ec.message()));
    }
    return req;
}
void DumpRequest(const StringRequest& req) {
    std::cout << req.method_string() << ' ' << req.target() << std::endl;
    // Âûâîäèì çàãîëîâêè çàïðîñà
    for (const auto& header : req) {
        std::cout << "  "sv << header.name_string() << ": "sv << header.value() << std::endl;
    }
}

// Ñòðóêòóðà ContentType çàäà¸ò îáëàñòü âèäèìîñòè äëÿ êîíñòàíò,
// çàäàþùèé çíà÷åíèÿ HTTP-çàãîëîâêà Content-Type
struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;
    // Ïðè íåîáõîäèìîñòè âíóòðü ContentType ìîæíî äîáàâèòü è äðóãèå òèïû êîíòåíòà
};

// Ñîçäà¸ò StringResponse ñ çàäàííûìè ïàðàìåòðàìè
StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
    bool keep_alive,
    std::string_view content_type = ContentType::TEXT_HTML) {
    StringResponse response(status, http_version);
    response.set(http::field::content_type, content_type);
    response.body() = body;
    response.content_length(body.size());
    response.keep_alive(keep_alive);
    return response;
}

StringResponse HandleRequest(StringRequest&& req) {
    const auto text_response = [&req](http::status status, std::string_view text) {
        return MakeStringResponse(status, text, req.version(), req.keep_alive());
    };
    auto method = req.method();
    //StringResponse res;
    if (method == http::verb::get)
    {
         std::string ans = std::string(req.target().substr(1));
        ans = "Hello, " + ans;
        return text_response(http::status::ok, ans);
    }
    if (method == http::verb::head)
    {
        return text_response(http::status::ok, "");
    }
    // Çäåñü ìîæíî îáðàáîòàòü çàïðîñ è ñôîðìèðîâàòü îòâåò, íî ïîêà âñåãäà îòâå÷àåì: Hello
    StringResponse res = text_response(http::status::method_not_allowed, "Invalid method"sv);
    res.set(http::field::allow, "GET, HEAD");
    return res;
}


void HandleConnection(tcp::socket& socket) {
    try {
        // Áóôåð äëÿ ÷òåíèÿ äàííûõ â ðàìêàõ òåêóùåé ñåññèè.
        beast::flat_buffer buffer;

        // Ïðîäîëæàåì îáðàáîòêó çàïðîñîâ, ïîêà êëèåíò èõ îòïðàâëÿåò
        while (auto request = ReadRequest(socket, buffer)) {
            DumpRequest(*request);
            StringResponse response = HandleRequest(*std::move(request));
            http::write(socket, response);
            if (response.need_eof()) {
                break;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    beast::error_code ec;
    // Çàïðåùàåì äàëüíåéøóþ îòïðàâêó äàííûõ ÷åðåç ñîêåò
    socket.shutdown(tcp::socket::shutdown_send, ec);
}
int main() {
    net::io_context ioc;

    const auto address = net::ip::make_address("0.0.0.0");
    constexpr unsigned short port = 8080;

    tcp::acceptor acceptor(ioc, { address, port });
    std::cout << "Server has started..."sv << std::endl;
    while (true) {
        tcp::socket socket(ioc);
        acceptor.accept(socket);

        // Çàïóñêàåì îáðàáîòêó âçàèìîäåéñòâèÿ ñ êëèåíòîì â îòäåëüíîì ïîòîêå
        std::thread t(
            // Ëÿìáäà-ôóíêöèÿ áóäåò âûïîëíÿòüñÿ â îòäåëüíîì ïîòîêå
            [](tcp::socket socket) {
                HandleConnection(socket);
            },
            std::move(socket));  // Ñîêåò íåëüçÿ ñêîïèðîâàòü, íî ìîæíî ïåðåìåñòèòü

        // Ïîñëå âûçîâà detach ïîòîê ïðîäîëæèò âûïîëíÿòüñÿ íåçàâèñèìî îò îáúåêòà t
        t.detach();
    }
}
