#include "TestDemo.h"

int main() {
    TestDemo testDemo("td1");
    testDemo.tPrint("abc\bd");
    TestDemo::m_split("aa|bb|cc|dd|ss|ff|ggg|hh|kk|rr|tt", "|");
    testDemo.tPrint(STR2CHR(calcFunc(3, 4, e_max)));

    return 0;
}
