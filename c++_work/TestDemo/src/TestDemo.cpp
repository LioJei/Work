//
// Created by Administrator on 2024/2/5.
//

#include "TestDemo.h"

TestDemo::TestDemo(const char* name) : m_name(name)
{
    printf("Construct TestDemo:%s\n", m_name);
}

TestDemo::~TestDemo(){
    printf("Destroy TestDemo:%s\n", m_name);
}

void TestDemo::tPrint(const char *msg) {
    printf("%s:%s\n", m_name, msg);
}

std::vector<std::string> TestDemo::m_split(std::string str, const char* ch){
    std::vector<std::string> vec;
    auto index = str.find(ch);
    while (-1 != index){
        vec.push_back(str.substr(0,index));
        str = str.substr(index + 1);
        index = str.find(ch);
    }
    vec.push_back(str);

    index = 1;
    for(const auto & iter : vec){
        std::cout << iter << " ";
        if (++index > SPLIT_SHOW_NUMBER)
            break;
    }
    std::cout << "..." << std::endl;

    return vec;
}