//
// Created by ThinkBook on 2025/11/4.
//

#ifndef BOOST_DEMO_MYBOOSTTEST_H
#define BOOST_DEMO_MYBOOSTTEST_H
#include <boost/algorithm/string.hpp>
#include <boost/config.hpp>                 //跨平台的配置支持和编译器特性检测
#include <boost/assert.hpp>                 //断言工具，用于在程序运行时检查条件是否满足
#include <boost/static_assert.hpp>          //用于在编译时进行断言检查,如类型检查
#include <boost/type_traits.hpp>            //用于在编译时查询和操作类型信息
#include <boost/utility.hpp>                //实用工具集合，提供了一些小型但非常有用的组件
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/tokenizer.hpp>
#include <boost/optional.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/chrono.hpp>
#include <boost/random.hpp>
#include <boost/numeric/conversion/converter.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/timer/timer.hpp>
#include <boost/process.hpp>
#include "commm_header.h"

namespace qi = boost::spirit::qi;
enum E_STRING_MODIFY {
    E_STRING_MODIFY_NONE = 0,
    E_STRING_MODIFY_TO_UPPER,
    E_STRING_MODIFY_TO_LOWER,
};

class MyBoostTest {
public:
    explicit MyBoostTest() = delete;

    explicit MyBoostTest(std::string name);

    ~MyBoostTest();

    MyBoostTest(const MyBoostTest &) = delete;

    MyBoostTest &operator=(const MyBoostTest) = delete;

    void StringModify(std::string str, E_STRING_MODIFY type) const;

private:
    std::string _name;
};


#endif //BOOST_DEMO_MYBOOSTTEST_H
