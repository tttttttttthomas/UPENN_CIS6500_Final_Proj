# 快速入门指南

## 项目概述

这是一个学习型多维索引（Flood）的实现项目，包括：
- 基础数据结构（DataPoint, QueryRange）
- 多种索引实现（Flood, R*-tree, k-d tree, Z-order）
- 基准测试框架
- 数据加载和处理工具

## 当前进度

### ✅ 已完成
1. **项目结构**：完整的目录结构和CMake构建系统
2. **基础类**：DataPoint, QueryRange, BaseIndex接口
3. **数据处理**：DataLoader（支持CSV和二进制格式）
4. **测试框架**：基础单元测试
5. **编译系统**：成功编译和运行

### 🚧 待实现（按优先级）
1. **Week 2**: 基线索引
   - [ ] k-d Tree实现
   - [ ] Z-order索引实现
   - [ ] R*-tree集成（需要先修复Boost兼容性）

2. **Week 2-3**: Flood索引
   - [ ] 数据扁平化（flattening）
   - [ ] 成本模型
   - [ ] 学习算法

3. **Week 4**: 基准测试
   - [ ] 工作负载生成器（A, B, C）
   - [ ] 性能测量
   - [ ] 结果输出

## 快速开始

### 1. 验证环境

```bash
# 进入项目目录
cd "/Users/wangkaiyuan/Desktop/UPenn/Fall 2025/CIS 6500/final project"

# 运行测试
./build/bin/run_tests

# 运行主程序
./build/bin/flood_index
```

### 2. 数据准备

下载NYC出租车数据：
```bash
# 创建数据目录
mkdir -p data/nyc_taxi

# 访问 https://www.nyc.gov/site/tlc/about/tlc-trip-record-data.page
# 下载数据（推荐从一个月开始）
# 
# 推荐下载：
# - 数据类型：Yellow Taxi Trip Records（数据量最大，最适合测试）
# - 格式：CSV格式（项目目前只支持CSV）
# - 月份：2025年1月（或任意一个月）
# - 将文件放在 data/nyc_taxi/ 目录下
```

**注意：** 
- 如果网站只有PARQUET格式，可以使用提供的转换脚本：
```bash
# 安装依赖
pip install pandas pyarrow

# 转换PARQUET到CSV
python tools/convert_parquet_to_csv.py data/nyc_taxi/yellow_tripdata_2025-01.parquet data/nyc_taxi/input.csv
```

### 3. 处理数据

```bash
# CSV转二进制格式（更快的加载速度）
./build/bin/process_data data/nyc_taxi/input.csv data/nyc_taxi/processed.bin
```

### 4. 开发流程

#### 添加新索引

1. 在`include/indexes/`创建头文件
2. 在`src/indexes/`创建实现文件
3. 在`CMakeLists.txt`的SOURCES中添加
4. 继承`BaseIndex`类并实现所有虚函数

示例：

```cpp
// include/indexes/my_index.h
#ifndef MY_INDEX_H
#define MY_INDEX_H

#include "indexes/base_index.h"

namespace flood {

class MyIndex : public BaseIndex {
public:
    void build(const std::vector<DataPoint>& data) override;
    std::vector<DataPoint> query(const QueryRange& range) override;
    double getIndexSize() const override;
    std::string getName() const override { return "MyIndex"; }
};

} // namespace flood

#endif
```

#### 运行基准测试

```bash
# 将在Phase 4实现
./build/bin/run_benchmark data/nyc_taxi/processed.bin results/
```

## 常见问题

### Q: 编译失败，找不到Boost

**A**: 检查Boost是否安装：
```bash
brew list boost
```

如果没有安装：
```bash
brew install boost
```

### Q: R-tree相关的编译错误

**A**: 参考`BOOST_FIX.md`文档修复Boost兼容性问题，或暂时禁用R-tree。

### Q: 如何添加新的测试

**A**: 在`tests/test_indexes.cpp`中添加新的测试函数：

```cpp
void test_my_feature() {
    std::cout << "Testing my feature... ";
    // 你的测试代码
    assert(condition);
    std::cout << "PASSED" << std::endl;
}

// 在run_tests()中调用
int run_tests() {
    test_data_point();
    test_query_range();
    test_data_loader();
    test_my_feature();  // 新测试
    return 0;
}
```

## 下一步

1. **数据获取**（本周）
   - 下载NYC出租车数据
   - 运行`process_data`工具

2. **实现k-d Tree**（Week 2开始）
   - 参考论文或标准算法
   - 实现构建和查询函数
   - 测试性能

3. **阅读Flood论文**
   - 重点理解"flattening"技术
   - 理解成本模型

4. **开始实现Flood**（Week 2-3）
   - 先实现简单版本
   - 逐步添加学习功能

## 有用的资源

- [Boost.Geometry文档](https://www.boost.org/doc/libs/release/libs/geometry/)
- [NYC TLC数据](https://www.nyc.gov/site/tlc/about/tlc-trip-record-data.page)
- [Learning Multi-dimensional Indexes论文](https://dl.acm.org/doi/10.1145/3318464.3380579)
- [CMake文档](https://cmake.org/documentation/)

## 项目时间线

| 时间 | 任务 | 状态 |
|------|------|------|
| Week 1 (11/8-11/14) | 环境搭建、基础框架 | ✅ 完成 |
| Week 2 (11/15-11/21) | 基线索引实现 | 🚧 进行中 |
| Week 3 (11/22-11/28) | Flood索引实现 | ⏳ 待开始 |
| Week 4 (11/29-12/5) | 实验与评估 | ⏳ 待开始 |
| Week 5 (12/6-12/11) | 报告撰写 | ⏳ 待开始 |

祝你实现顺利！🚀

