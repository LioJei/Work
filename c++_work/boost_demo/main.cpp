#include "MyBoostTest.h"
#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>

namespace acc = boost::accumulators;
namespace bp = boost::process;

int main(int argc, char **argv) {
    {
        boost::timer::auto_cpu_timer t;
        // 启动子进程（例如执行 `ls` 命令）
        bp::ipstream pipe_stream;
        bp::child c("ls", bp::std_out > pipe_stream);

        // 读取子进程的输出
        std::string line;
        while (pipe_stream && std::getline(pipe_stream, line)) {
            std::cout << line << std::endl;
        }

        // 等待子进程结束
        c.wait();
    }


    // QApplication app(argc, argv);
    // QWidget *window = new QWidget();
    // window->setMinimumSize(600, 600);
    // QGridLayout *layout = new QGridLayout(window);
    // QPushButton *button = new QPushButton("close");
    // layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    // auto _lambda = [&](){window->resize(800, 600);sleep(5);window->close();};
    // QObject::connect(button, &QPushButton::clicked, window, _lambda);
    // window->show();
    //
    // return app.exec();
    return EXIT_SUCCESS;
}
