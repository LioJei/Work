#include "TestDemo.h"

template<typename T, typename U>
struct A {
    T t;
    U u;
};
template<typename T>
using B = A<T, int>;

std::shared_timed_mutex mutex;

void try_writer(int id) {
    if(mutex.try_lock_for(std::chrono::milliseconds(100))) {
        std::cout << "Writer " << id << " acquired the lock.\n";
        // 执行写操作
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        mutex.unlock();
    } else {
        std::cout << "Writer " << id << " failed to acquire the lock.\n";
    }
}

int main() {
    TestDemo testDemo("td1");
    // testDemo.TestPrint("abc\bd");
    // TestDemo::MySplit("aa|bb|cc|dd|ss|ff|ggg|hh|kk|rr|tt", "|");
    // testDemo.TestPrint(STR2CHR(calcFunc(3, 4, e_max)));
    // TestDemo::PrintTypeSize();
    // testDemo.TestPrint(STR2CHR(TestDemo::compute(TestDemo::sub,7,4)));
    // TestDemo::time_test();

    // B<double> b1;
    // b1.t = 10;
    // b1.u = 20;
    // std::cout << b1.t << std::endl;
    // std::cout << b1.u << std::endl;
    std::thread t1([](){
        mutex.lock();
        std::this_thread::sleep_for(std::chrono::milliseconds(90));
        mutex.unlock();
    });

    std::thread t2(try_writer, 1);

    t1.join();
    t2.join();

    return 0;
}
