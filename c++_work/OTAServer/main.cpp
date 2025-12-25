// http_file_server.cpp
#include <cpprest/http_listener.h>
#include <cpprest/filestream.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>
#include <cpprest/asyncrt_utils.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <mutex>

namespace fs = std::filesystem;
using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

class FileServer {
public:
    FileServer(const std::string& host, int port, const std::string& baseDir = "./files")
        : baseDir_(baseDir), host_(host), port_(port) {

        // 创建基础目录
        fs::create_directories(baseDir);

        // 设置监听器
        std::string uri = "http://" + host + ":" + std::to_string(port);
        listener_ = http_listener(uri);

        // 设置请求处理器
        listener_.support(methods::GET, std::bind(&FileServer::handleGet, this, std::placeholders::_1));
        listener_.support(methods::POST, std::bind(&FileServer::handlePost, this, std::placeholders::_1));
        listener_.support(methods::PUT, std::bind(&FileServer::handlePut, this, std::placeholders::_1));
        listener_.support(methods::DEL, std::bind(&FileServer::handleDelete, this, std::placeholders::_1));
    }

    ~FileServer() {
        stop();
    }

    bool start() {
        try {
            listener_.open().wait();
            std::cout << "文件服务器启动成功!" << std::endl;
            std::cout << "服务地址: http://" << host_ << ":" << port_ << std::endl;
            std::cout << "文件目录: " << fs::absolute(baseDir_) << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "服务器启动失败: " << e.what() << std::endl;
            return false;
        }
    }

    void stop() {
        listener_.close().wait();
        std::cout << "服务器已停止" << std::endl;
    }

private:
    void handleGet(http_request request) {
        auto path = request.request_uri().path();

        // 根路径显示文件列表
        if (path == "/" || path == "") {
            listFiles(request);
            return;
        }

        // 下载文件
        downloadFile(request);
    }

    void handlePost(http_request request) {
        // POST用于上传文件（表单方式）
        uploadFile(request);
    }

    void handlePut(http_request request) {
        // PUT用于直接上传文件
        uploadFile(request);
    }

    void handleDelete(http_request request) {
        auto path = request.request_uri().path();
        std::string filename = path.substr(1); // 去除开头的/

        if (filename.empty() || filename.find("..") != std::string::npos) {
            request.reply(status_codes::BadRequest, "无效的文件名");
            return;
        }

        std::string filePath = baseDir_ + "/" + filename;

        std::error_code ec;
        if (!fs::exists(filePath, ec)) {
            request.reply(status_codes::NotFound, "文件不存在");
            return;
        }

        if (fs::remove(filePath, ec)) {
            request.reply(status_codes::OK, "文件删除成功");
        } else {
            request.reply(status_codes::InternalError, "文件删除失败: " + std::string(ec.message()));
        }
    }

    void listFiles(http_request request) {
        json::value fileList = json::value::array();
        int index = 0;

        try {
            for (const auto& entry : fs::directory_iterator(baseDir_)) {
                if (entry.is_regular_file()) {
                    json::value fileInfo;
                    fileInfo["name"] = json::value::string(entry.path().filename().string());
                    fileInfo["size"] = json::value::number(entry.file_size());
                    fileInfo["modified"] = json::value::string(
                        std::to_string(fs::last_write_time(entry).time_since_epoch().count()));

                    fileList[index++] = fileInfo;
                }
            }

            json::value response;
            response["files"] = fileList;
            response["count"] = json::value::number(index);

            request.reply(status_codes::OK, response);

        } catch (const std::exception& e) {
            request.reply(status_codes::InternalError, e.what());
        }
    }

    void downloadFile(http_request request) {
        auto path = request.request_uri().path();
        std::string filename = path.substr(1); // 去除开头的/

        if (filename.empty() || filename.find("..") != std::string::npos) {
            request.reply(status_codes::BadRequest, "无效的文件名");
            return;
        }

        std::string filePath = baseDir_ + "/" + filename;

        std::error_code ec;
        if (!fs::exists(filePath, ec)) {
            request.reply(status_codes::NotFound, "文件不存在");
            return;
        }

        if (!fs::is_regular_file(filePath, ec)) {
            request.reply(status_codes::BadRequest, "不是普通文件");
            return;
        }

        // 设置下载头
        auto headers = request.headers();
        headers.add("Content-Disposition", "attachment; filename=\"" + filename + "\"");

        // 发送文件
        try {
            auto fileStream = concurrency::streams::fstream::open_istream(filePath).get();
            request.reply(status_codes::OK, fileStream, "application/octet-stream", headers)
                .then([fileStream]() { fileStream.close(); });
        } catch (const std::exception& e) {
            request.reply(status_codes::InternalError, e.what());
        }
    }

    void uploadFile(http_request request) {
        auto path = request.request_uri().path();
        std::string filename;

        // 从查询参数或路径中获取文件名
        auto query = uri::split_query(request.request_uri().query());
        if (query.find("filename") != query.end()) {
            filename = uri::decode(query["filename"]);
        } else if (path != "/" && path != "") {
            filename = path.substr(1);
        } else {
            // 从Content-Disposition头获取文件名
            auto headers = request.headers();
            if (headers.has("Content-Disposition")) {
                std::string disposition = headers["Content-Disposition"];
                size_t pos = disposition.find("filename=");
                if (pos != std::string::npos) {
                    filename = disposition.substr(pos + 9);
                    // 去除引号
                    if (filename.front() == '"' && filename.back() == '"') {
                        filename = filename.substr(1, filename.size() - 2);
                    }
                }
            }
        }

        if (filename.empty()) {
            request.reply(status_codes::BadRequest, "需要指定文件名");
            return;
        }

        if (filename.find("..") != std::string::npos || filename.find("/") != std::string::npos) {
            request.reply(status_codes::BadRequest, "无效的文件名");
            return;
        }

        std::string filePath = baseDir_ + "/" + filename;

        try {
            // 创建文件流并保存
            auto fileStream = concurrency::streams::fstream::open_ostream(filePath).get();
            request.body().read_to_end(fileStream.streambuf()).get();
            fileStream.close();

            json::value response;
            response["success"] = json::value::boolean(true);
            response["filename"] = json::value::string(filename);
            response["size"] = json::value::number(fs::file_size(filePath));

            request.reply(status_codes::OK, response);

        } catch (const std::exception& e) {
            request.reply(status_codes::InternalError, e.what());
        }
    }

    http_listener listener_;
    std::string baseDir_;
    std::string host_;
    int port_;
};

int main() {
    std::cout << "=== C++ HTTP文件服务器 ===" << std::endl;

    FileServer server("0.0.0.0", 8080, "./server_files");

    if (!server.start()) {
        return 1;
    }

    std::cout << "按回车键停止服务器..." << std::endl;
    std::cin.get();

    server.stop();
    return 0;
}
