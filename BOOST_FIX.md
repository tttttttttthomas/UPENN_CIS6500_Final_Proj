# Boost兼容性问题修复指南

## 问题描述

当前系统中的Boost版本(1.73)与macOS 15.2 SDK的C++17标准库存在兼容性问题。主要错误是：

```
error: no template named 'unary_function' in namespace 'std'; 
did you mean '__unary_function'?
```

这是因为`std::unary_function`在C++17中被移除了，而Boost 1.73还在使用它。

## 解决方案

### 方案1：升级Boost (推荐)

安装Boost 1.75或更高版本，该版本与C++17完全兼容：

```bash
# 使用Homebrew安装最新的Boost
brew unlink boost  # 如果已经安装了旧版本
brew install boost

# 或者手动编译
wget https://boostorg.jfrog.io/artifactory/main/release/1.84.0/source/boost_1_84_0.tar.gz
tar xzf boost_1_84_0.tar.gz
cd boost_1_84_0
./bootstrap.sh --prefix=/usr/local
./b2 install
```

### 方案2：使用conda环境的Boost

如果你使用的是conda/anaconda:

```bash
conda install -c conda-forge boost>=1.75
```

然后修改CMakeLists.txt，指定正确的Boost路径：

```cmake
set(BOOST_ROOT "/usr/local")  # 或你的conda环境路径
find_package(Boost 1.75 REQUIRED)
```

### 方案3：暂时不使用Boost R-tree

在实现阶段2之前，R-tree功能可以暂时禁用（已在CMakeLists.txt中注释掉）。

你可以：
1. 先实现k-d tree, Z-order和Flood索引
2. 稍后再修复Boost兼容性问题
3. 或者使用其他R-tree实现（例如：https://github.com/nushoin/RTree）

## 临时禁用R-tree

在`CMakeLists.txt`中，R-tree相关代码已被注释：

```cmake
set(SOURCES
    src/data/data_point.cpp
    src/data/data_loader.cpp
    src/indexes/base_index.cpp
    # src/indexes/rtree_index.cpp  # TODO: Fix Boost compatibility issue
    src/indexes/kdtree_index.cpp
    src/indexes/zorder_index.cpp
    src/indexes/flood_index.cpp
    src/benchmark/workload_generator.cpp
    src/benchmark/benchmark.cpp
)
```

## 验证修复

升级Boost后，取消注释CMakeLists.txt中的R-tree行，然后重新编译：

```bash
cd build
rm -rf *
cmake ..
make -j4
```

如果编译成功，说明问题已解决。

