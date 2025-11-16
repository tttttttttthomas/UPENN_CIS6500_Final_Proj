# Performance Comparison Plots

本目录包含 Flood 索引项目的所有性能对比图表。

## 生成的图表

### 1. 索引大小对比 (1_index_size_comparison.png)
**指标**: 索引内存占用 (MB)  
**数据**: 50,000 点测试

展示了四种索引的内存占用情况：
- **Flood**: 1.53 MB 🏆 (最小)
- k-d Tree: 2.67 MB (+75%)
- Z-order: 3.05 MB (+99%)
- R*-tree: 6.87 MB (+350%)

**关键发现**: Flood 索引大小最小，比第二名小 43%，比最大的 R*-tree 小 78%。

---

### 2. 构建时间对比 (2_build_time_comparison.png)
**指标**: 索引构建时间 (ms)  
**数据**: 50,000 点测试，平均值

展示了四种索引的构建速度：
- R*-tree: 7.7 ms 🥇 (最快)
- **Flood**: 9.1 ms 🥈 (第二)
- k-d Tree: 18.1 ms
- Z-order: 20.3 ms

**关键发现**: Flood 构建时间第二快，比 k-d Tree 和 Z-order 快 2-2.5 倍。

---

### 3. 各工作负载查询时间 (3_query_time_by_workload.png)
**指标**: 平均查询时间 (ms)  
**数据**: 3 种工作负载，各 100 个查询

#### Workload A: 空间查询 (0.1% 选择率)
- k-d Tree: 0.008 ms 🥇
- **Flood**: 0.137 ms 🥈
- Z-order: 0.620 ms
- R*-tree: 6.863 ms

#### Workload B: 时空查询 (0.5% 选择率)
- k-d Tree: 0.009 ms 🥇
- **Flood**: 0.153 ms 🥈
- Z-order: 0.576 ms
- R*-tree: 6.860 ms

#### Workload C: 混合查询 (1% 选择率)
- k-d Tree: 0.022 ms 🥇
- **Flood**: 0.164 ms 🥈
- Z-order: 0.607 ms
- R*-tree: 6.932 ms

**关键发现**: Flood 在所有工作负载中都排名第二，性能稳定一致。

---

### 4. 平均查询时间 - 对数尺度 (4_avg_query_time_log.png)
**指标**: 所有工作负载的平均查询时间 (ms, log scale)

对数尺度更清晰地展示了性能差异：
- k-d Tree: 0.013 ms 🥇
- **Flood**: 0.152 ms 🥈 (11.7x slower)
- Z-order: 0.601 ms (46.2x slower)
- R*-tree: 6.885 ms (529.6x slower)

**关键发现**: 虽然 Flood 比 k-d Tree 慢，但仍比 Z-order 快 4x，比 R*-tree 快 45x。

---

### 5. 查询时间百分位数 (5_query_percentiles.png)
**指标**: P95 和 P99 查询延迟 (ms)

#### P95 查询时间
- k-d Tree: 0.019 ms 🥇
- **Flood**: 0.213 ms 🥈
- Z-order: 0.879 ms
- R*-tree: 7.909 ms

#### P99 查询时间
- k-d Tree: 0.025 ms 🥇
- **Flood**: 0.229 ms 🥈
- Z-order: 1.662 ms
- R*-tree: 11.168 ms

**关键发现**: Flood 的尾延迟（P95/P99）表现优秀，说明性能稳定可靠。

---

### 6. 综合性能雷达图 (6_radar_chart.png)
**指标**: 5 个维度的综合对比（归一化到 0-100，越高越好）

维度包括：
- 索引大小（内存效率）
- 构建时间
- 平均查询时间
- P95 查询时间
- P99 查询时间

**关键发现**: 
- **Flood** 在所有维度上都表现均衡，没有明显短板
- k-d Tree 在查询速度上领先，但内存占用较大
- R*-tree 除了构建速度外，其他指标都较差
- Z-order 在所有维度都是中等偏下

---

## 主要结论

### Flood 索引的优势

1. **🏆 内存效率最优**
   - 索引大小最小（1.53 MB）
   - 比其他索引小 43-78%
   - 可支持更大规模数据集

2. **⚡ 构建速度快**
   - 构建时间 9.1 ms（第二名）
   - 适合需要频繁构建的场景
   - 仅比最快的 R*-tree 慢 18%

3. **✅ 查询性能优秀**
   - 所有工作负载中排名第二
   - 性能稳定一致
   - 比 Z-order 快 4x，比 R*-tree 快 45x

4. **🎯 平衡性最好**
   - 无明显短板
   - 综合评分最高
   - 适合多种应用场景

### 适用场景

Flood 索引特别适合：
- 内存受限的环境
- 需要快速构建的场景
- 对查询性能有一定要求但更看重综合性能的场景
- 需要支持大规模数据集的应用

---

## 如何使用这些图表

这些图表可以直接用于：
1. **项目报告** - Experimental Evaluation 部分
2. **演示文稿** - 展示实验结果
3. **学术论文** - 性能对比分析
4. **技术文档** - 说明索引特性

---

## 重新生成图表

如需重新生成图表，运行：

```bash
cd /Users/wangkaiyuan/Desktop/UPENN_CIS6500_Final_Proj
source venv/bin/activate
python tools/generate_plots.py
```

图表将保存到 `plots/` 目录。

---

**生成日期**: 2025年11月9日  
**数据来源**: build/benchmark_results.csv  
**生成脚本**: tools/generate_plots.py

