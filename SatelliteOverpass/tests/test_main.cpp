/**
 * @file test_main.cpp
 * @brief Google Test 测试框架入口
 *
 * @author kerwin_zhang
 * @version 2.0.0
 * @date 2026-02-11
 */

#include <gtest/gtest.h>

/**
 * @brief 主函数
 * 
 * 运行所有Google Test测试用例
 */
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
