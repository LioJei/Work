import random
import sys


class TestDemo:
    ##
    # @brief:           测试类初始化函数
    # @param[in]:       name(用例名)
    # #
    def __init__(self, name):
        self.__name = str(name)

    ##
    # @brief:           打印函数
    # @param[in]:       msg(要打印的信息)
    # #
    def test_print(self, msg):
        print(self.__name + ":" + str(msg))

    ##
    # @brief:           输入测试函数
    # @param[in]:       m_num(匹配数)
    # #
    def input_test(self, m_num):
        while True:
            num = int(input(self.__name + ":please guess a number:"))
            if num == int(m_num):
                break
            else:
                self.test_print("try again!")

        self.test_print("ok!")

    ##
    # @brief:           random测试
    # @param[in]:       m_num(测试次数)
    # #
    def random_test(self, m_num):
        for i in range(m_num):
            self.test_print(random.randint(1, 10))

        self.test_print("finish random test.")

    ##
    # @brief:           sys_exit测试
    # @param[in]:       m_str(匹配字串)
    # #
    def sys_exit_test(self, m_str):
        while True:
            self.test_print("Type str to exit:")
            res = input()
            if res == m_str:
                sys.exit("sys_exit!test end.")

            self.test_print("please try again.")

    ##
    # @brief:           异常处理测试
    # @param[in]:       m_num(被除数)
    # #
    def div_catch_test(self, m_num):
        try:
            div_num = int(input("please enter a number:"))
            return m_num / div_num
        except ZeroDivisionError:
            self.test_print("Error! please check your number.")
