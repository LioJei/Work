//
// Created by lio on 2024/2/5.
//

#include "TestDemo.h"

[[maybe_unused]] TestDemo::TestDemo(const char* name) : m_name(name)
{
    printf("Construct TestDemo:%s\n", m_name);
}

TestDemo::~TestDemo(){
    printf("Destroy TestDemo:%s\n", m_name);
}

[[maybe_unused]] void TestDemo::TestPrint(const char *msg) {
    printf("%s:%s\n", m_name, msg);
}

[[maybe_unused]] std::vector<std::string> TestDemo::MySplit(std::string str, const char* ch){
    std::vector<std::string> vec;
    auto index = str.find(ch);
    while (-1 != index){
        vec.push_back(str.substr(0,index));
        str = str.substr(index + 1);
        index = str.find(ch);
    }
    vec.push_back(str);

    index = 1;
    std::cout << "[after split]:" << std::endl;
    for(const auto & iter : vec){
        std::cout << iter << " ";
        if (++index > SPLIT_SHOW_NUMBER)
            break;
    }
    std::cout << "..." << std::endl;

    return vec;
}

[[maybe_unused]] void TestDemo::PrintTypeSize() {
    printf("Size of char: %llu\n", sizeof(char));
    printf("Size of char*: %llu\n", sizeof(char*));
    printf("Size of int: %llu\n", sizeof(int));
    printf("Size of int*: %llu\n", sizeof(int*));
    printf("Size of float: %llu\n", sizeof(float));
    printf("Size of float*: %llu\n", sizeof(float*));
    printf("Size of double: %llu\n", sizeof(double));
    printf("Size of double*: %llu\n", sizeof(double*));
    printf("Size of size_t: %llu\n", sizeof(size_t));
}

[[maybe_unused]] int TestDemo::add(int val1, int val2) {
    return val1+val2;
}

[[maybe_unused]] int TestDemo::sub(int val1, int val2) {
    return val1-val2;
}

[[maybe_unused]] int TestDemo::compute(fptr ptr, int val1, int val2) {
    return ptr(val1, val2);
}
