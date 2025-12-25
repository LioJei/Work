//
// Created by ThinkBook on 2025/11/4.
//

#include "MyBoostTest.h"

#include <utility>

MyBoostTest::MyBoostTest(std::string name) : _name(std::move(name)) {
    std::cout << "MyBoostTest::MyBoostTest(" << _name << ") construction." << std::endl;
}

MyBoostTest::~MyBoostTest() {
    std::cout << "MyBoostTest::MyBoostTest(" << _name << ") destruction." << std::endl;
}

void MyBoostTest::StringModify(std::string str, E_STRING_MODIFY type) const {
    switch (type) {
        case E_STRING_MODIFY_NONE:
            std::cout << "MyBoostTest::StringModify(" << _name << ") method print (" << str << ")" << std::endl;
            return;
        case E_STRING_MODIFY_TO_UPPER:
            boost::to_upper(str);
            std::cout << "MyBoostTest::StringModify(" << _name << ") method print (" << str << ")" << std::endl;
            return;
        case E_STRING_MODIFY_TO_LOWER:
            boost::to_lower(str);
            std::cout << "MyBoostTest::StringModify(" << _name << ") method print (" << str << ")" << std::endl;
            return;
        default: return;
    }
}
