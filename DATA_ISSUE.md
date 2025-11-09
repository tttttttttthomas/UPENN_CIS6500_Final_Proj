# NYC出租车数据格式问题与解决方案

## 问题描述

从2017年7月开始，NYC TLC不再在数据中提供经纬度信息（`pickup_longitude`, `pickup_latitude`），改为使用**出租车区域ID**（`PULocationID`, `DOLocationID`）。这是出于隐私保护的考虑。

你下载的2024和2025年数据都使用新格式，因此没有经纬度字段。

## 解决方案

### 方案1：下载旧数据（推荐，最简单）

下载**2016年或更早**的数据，这些数据包含完整的经纬度信息。

**步骤**：
1. 访问 https://www.nyc.gov/site/tlc/about/tlc-trip-record-data.page
2. 向下滚动到2016年或2015年
3. 下载Yellow Taxi Trip Records（CSV格式）
4. 使用我们的工具处理

**推荐月份**：
- 2016年6月（包含经纬度，数据质量好）
- 2015年1月（包含经纬度）

**数据列（旧格式）**：
```
vendor_id, pickup_datetime, dropoff_datetime, passenger_count, trip_distance, 
pickup_longitude, pickup_latitude, rate_code, store_and_fwd_flag, 
dropoff_longitude, dropoff_latitude, payment_type, fare_amount, ...
```

### 方案2：使用LocationID + Taxi Zone Lookup Table（需要额外工作）

如果你想使用新数据，需要：

1. **下载Taxi Zone Lookup Table**
   - 文件：`taxi_zone_lookup.csv`
   - 位置：NYC TLC网站的"Taxi Zone Maps and Lookup Tables"部分
   - 这个表将LocationID映射到区域中心点的经纬度

2. **下载Taxi Zone Shapefile**（可选，更精确）
   - 包含每个区域的详细边界
   - 可以计算区域中心或使用质心

3. **修改数据加载器**
   - 读取Lookup Table
   - 将PULocationID/DOLocationID转换为经纬度
   - 使用区域中心点作为近似位置

### 方案3：使用LocationID作为"虚拟坐标"（不推荐）

直接使用LocationID作为坐标值（例如，LocationID=1 → (1.0, 1.0)），但这会失去真实的空间含义。

## 推荐实施步骤

### 立即行动：使用旧数据

```bash
# 1. 下载2016年的数据
# 访问 NYC TLC网站，下载 yellow_tripdata_2016-06.csv

# 2. 放到项目目录
mv ~/Downloads/yellow_tripdata_2016-06.csv data/nyc_taxi/

# 3. 处理数据
./build/bin/process_data data/nyc_taxi/yellow_tripdata_2016-06.csv data/nyc_taxi/processed.bin
```

### 如果网站上没有2016年数据

可以从以下备用源下载：
- NYC Open Data Portal: https://data.cityofnewyork.us/
- 搜索"Yellow Taxi Trip Records 2016"

或者使用我为你准备的方案2实现。

## 需要修改的代码

如果选择方案2（使用新数据+LocationID转换），我需要修改：

1. **创建LocationID到经纬度映射表**
2. **修改DataLoader**：
   - 添加`loadTaxiZoneLookup()`函数
   - 在`loadFromCSV()`中查找LocationID对应的坐标
   - 提取正确的列

3. **修改CSV解析逻辑**：
   - 从`PULocationID`列获取区域ID
   - 查表获取对应的经纬度
   - 从`tpep_pickup_datetime`解析时间戳
   - 从`passenger_count`和`trip_distance`获取其他维度

## 数据格式对比

### 旧格式（2016年及之前）- ✅ 可直接使用
```
pickup_longitude, pickup_latitude, pickup_datetime, passenger_count, trip_distance
-73.9876, 40.7489, 2016-06-01 00:00:00, 1, 2.5
```

### 新格式（2017年7月之后）- ⚠️ 需要转换
```
PULocationID, DOLocationID, tpep_pickup_datetime, passenger_count, trip_distance
161, 236, 2024-01-01 00:00:00, 1, 2.5
```

需要查表：LocationID 161 → (某经度, 某纬度)

## 我的建议

**现阶段**：使用旧数据（2016年）最简单，可以立即开始工作。

**Phase 4实验阶段**（如果有时间）：可以实现LocationID转换，使用更新的数据做额外实验。

## 快速测试

下载2016年数据后，可以这样快速验证：

```bash
# 查看文件前几行
head -3 data/nyc_taxi/yellow_tripdata_2016-06.csv

# 应该能看到类似：
# vendor_id,pickup_datetime,...,pickup_longitude,pickup_latitude,...
# 2,2016-06-01 00:00:00,...,-73.98,40.74,...
```

要我帮你实现方案2（LocationID转换）吗？还是先用旧数据快速推进项目？


