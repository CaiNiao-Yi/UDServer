#include <crow.h>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <fmt/format.h>
#include <string>

int main(int argc, char *args[])
{
    int port = 8000;
    if (argc < 2)
    {
        CROW_LOG_WARNING << "使用默认端口号";
        CROW_LOG_WARNING << "可通过UDServer <port>更改";
    }
    else
    {
        port = std::stoi(args[1]);
    }
    crow::SimpleApp app;
    CROW_ROUTE(app, "/files").methods("GET"_method)([](const crow::request &req, crow::response &resp) {
        CROW_LOG_INFO << "/files接收到GET请求";
        auto filename = req.url_params.get("filename");
        CROW_LOG_INFO << fmt::format("请求文件{}", filename);
        auto current_path = std::filesystem::current_path();
        auto filepath = current_path / filename;
        if (std::filesystem::exists(filepath))
        {
            CROW_LOG_INFO << fmt::format("请求文件{}存在", filepath.string());
            resp.set_header("Content-Type", "application/octet-stream");
            resp.set_header("Content-Disposition", fmt::format("attachment; filename=\"{}\"", filename));
            std::ifstream file(filepath.string(), std::ios::binary);
            std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            resp.body = data;
            resp.end();
        }
        else
        {
            CROW_LOG_ERROR << fmt::format("请求文件{}不存在", filename);
            resp.code = 404;
            resp.write("File Not Found");
            resp.end();
        }
    });
    CROW_ROUTE(app, "/files").methods("POST"_method)([](const crow::request &req, crow::response &resp) {
        CROW_LOG_INFO << "/files接收到POST请求";
        auto body = req.body;
        crow::multipart::message msg(req);
        if (msg.parts.size() <= 0)
        {
            CROW_LOG_INFO << "接收文件为空";
            resp.code = 400;
            resp.body = "Empty Files";
            resp.end();
        }
        else
        {
            CROW_LOG_INFO << fmt::format("接收到{}份文件", msg.parts.size());
            for (const auto &part : msg.parts)
            {
                auto filename = part.get_header_object("Content-Disposition").params.at("name");
                std::ofstream file(filename, std::ios::out | std::ios::binary);
                file << part.body;
                file.close();
                CROW_LOG_INFO << fmt::format("文件{}保存成功", filename);
            }
            resp.code = 200;
            resp.body = "File Saved";
            resp.end();
        }
    });

    app.bindaddr("127.0.0.1").port(port).run();
}
