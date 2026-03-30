#include <iostream>
#include <fstream>

#pragma pack(2) 

typedef unsigned short WORD;  // 2字节
typedef unsigned int   DWORD; // 4字节
typedef int            LONG;  // 4字节
typedef unsigned char  BYTE;  // 1字节

// 文件头
struct BITMAPFILEHEADER {
    WORD    bfType;      // 文件标识，为字母 ASCII 码“BM”
    DWORD   bfSize;      // 文件大小
    WORD    bfReserved1; // 保留，每字节以“00”填写
    WORD    bfReserved2; // 保留，每字节以“00”填写
    DWORD   bfOffBits;   // 记录图像数据区的起始位置
};

// 信息头
struct BITMAPINFOHEADER {
    DWORD biSize;           // 图像描述信息块的大小
    DWORD biWidth;
    DWORD biHeight;
    WORD biPlanes;          // 图像的 plane 总数（恒为 1）
    WORD biBitCount;        // 记录像素的位数
    DWORD biCompression;    // 数据压缩方式
    DWORD biSizeImage;      // 图像区数据的大小
    DWORD biXPelsPerMeter;  // 水平每米有多少像素
    DWORD biYPelsPerMeter;  // 垂直每米有多少像素
    DWORD biClrUsed;        // 此图像所用的颜色数
    DWORD biClrImportant;   
};

// 24位像素单元
struct RGBTRIPLE {
    BYTE rgbtBlue;  // 蓝色 
    BYTE rgbtGreen; // 绿色 
    BYTE rgbtRed;   // 红色 
};

int main(int argc, char* argv[]) {
    // 检查参数个数是否正确
    if (argc != 3) {
        std::cout << "Usage: rotatebmp src.bmp dest.bmp" << std::endl;
        return 1;
    }

    // 2. 以二进制只读方式打开源文件
    FILE* fpSrc = fopen(argv[1], "rb");
    if (fpSrc == NULL) return 1;

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fpSrc);
    fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fpSrc);

    // 验证是否为真彩色 BMP
    if (fileHeader.bfType != 0x4D42 || infoHeader.biBitCount != 24) {
        fclose(fpSrc);
        return 1;
    }

    // 计算原图每行的字节数（含补齐字节）
    int oldW = infoHeader.biWidth;
    int oldH = infoHeader.biHeight;
    int oldRowSize = ((int)oldW * 24 + 31) / 32 * 4;

    // 在开始读取像素循环前
    fseek(fpSrc, fileHeader.bfOffBits, SEEK_SET);

    // 使用一维数组模拟二维：data[y * width + x]
    RGBTRIPLE* oldData = new RGBTRIPLE[oldW * oldH];
    for (int i = 0; i < oldH; i++) {
        // 读取一行真实的像素数据 [cite: 93]
        fread(&oldData[i * oldW], sizeof(RGBTRIPLE), oldW, fpSrc);
        // 跳过每行末尾的补齐字节 [cite: 41, 49]
        fseek(fpSrc, oldRowSize - oldW * 3, SEEK_CUR);
    }
    fclose(fpSrc);

    // 准备旋转后的参数
    int newW = oldH; 
    int newH = oldW; 
    int newRowSize = ((int)newW * 24 + 31) / 32 * 4;

    // 更新头信息以写入目标文件
    infoHeader.biWidth = newW;
    infoHeader.biHeight = newH;
    infoHeader.biSizeImage = newRowSize * newH; 
    fileHeader.bfSize = 54 + infoHeader.biSizeImage;

    // 执行坐标映射旋转
    RGBTRIPLE* newData = new RGBTRIPLE[newW * newH];
    for (int y = 0; y < oldH; y++) {
        for (int x = 0; x < oldW; x++) {
            int newX = y;
            int newY = (oldW - 1) - x;
            newData[newY * newW + newX] = oldData[y * oldW + x];
        }
    }

    // 写入目标文件
    FILE* fpDest = fopen(argv[2], "wb");
    if (!fpDest) {
        std::cout << "无法创建目标文件！" << std::endl;
        delete[] oldData;
        delete[] newData;
        return 1;
    }

    // 写入头信息
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fpDest);
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fpDest);

    // 逐行写入旋转后的像素并手动补零
    BYTE padding[4] = { 0, 0, 0, 0 };
    int paddingSize = newRowSize - newW * 3;

    for (int i = 0; i < newH; i++) {
        fwrite(&newData[i * newW], sizeof(RGBTRIPLE), newW, fpDest);
        if (paddingSize > 0) {
            fwrite(padding, 1, paddingSize, fpDest);
        }
    }

    // 清理内存并退出
    fclose(fpDest);
    delete[] oldData;
    delete[] newData;
    
    return 0;
}