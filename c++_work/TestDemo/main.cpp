#include "TestDemo.h"

int main() {
    TestDemo testDemo("td1");
    testDemo.TestPrint("abc\bd");
    TestDemo::MySplit("aa|bb|cc|dd|ss|ff|ggg|hh|kk|rr|tt", "|");
    testDemo.TestPrint(STR2CHR(calcFunc(3, 4, e_max)));
    TestDemo::PrintTypeSize();
    testDemo.TestPrint(STR2CHR(TestDemo::compute(TestDemo::sub,7,4)));

    return 0;
}
