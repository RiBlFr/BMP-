# rotatebmp - 24位真彩色 BMP 图像旋转工具

这是一个基于 C++ 开发的命令行工具，专门用于将 **24位真彩色（RGB24）** 格式的 `.bmp` 图像顺时针旋转 90 度。

## 系统要求
* **操作系统**：Windows 10/11 (x64)。
* **编译环境**：Microsoft Visual Studio 2022 或更高版本（推荐），或支持 C++11 标准的 GCC/MinGW 编译器。
* **输入限制**：仅支持未压缩的 24 位真彩色 BMP 文件。

---

## 编译说明

### 1. Visual Studio 编译（推荐）
* **预处理器设置**：由于使用了标准 C 库函数 `fopen`，若在 Visual Studio 中编译报错 `C4996`，请在项目属性中添加预处理器定义：`_CRT_SECURE_NO_WARNINGS`。
* **结构对齐选项**：源代码中已包含 `#pragma pack(2)` 指令，确保结构体严格按照 2 字节边界对齐，以匹配 BMP 文件头格式。
* **编译配置**：建议使用 `Release` 模式和 `x64` 平台生成可执行文件，以获得最佳性能。

### 2. 命令行编译 (GCC/g++)
如果你使用 MinGW 或 Linux 环境下的 GCC，可以使用以下命令：
```bash
g++ -o rotatebmp main.cpp
```

---

## 执行与用法

程序通过命令行参数运行，不支持交互式 `cin` 输入。

### 命令格式
```bash
rotatebmp <src.bmp> <dest.bmp>
```

### 参数说明
* `src.bmp`：源图像路径（必须为 24 位真彩色格式）。
* `dest.bmp`：旋转后生成的图像保存路径。

### 运行示例
1.  将 `rotatebmp.exe` 和待处理图片（如 `test.bmp`）放在同一文件夹。
2.  在该文件夹内打开命令提示符（CMD）或 PowerShell。
3.  输入并执行：
```bash
rotatebmp test.bmp output.bmp
```


