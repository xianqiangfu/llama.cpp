# 媒体文件说明

本目录包含 llama.cpp 项目使用的媒体文件，主要是品牌图像和可视化图表。

## 目录内容

```
media/
├── llama0-banner.png      # 0代横幅图像
├── llama0-logo.png        # 0代 Logo
├── llama1-banner.png      # 1代横幅图像
├── llama1-icon-transparent.png  # 1代透明图标
├── llama1-icon-transparent.svg  # 1代透明图标 (SVG)
├── llama1-icon.png        # 1代图标
├── llama1-icon.svg        # 1代图标 (SVG)
├── llama1-logo.png        # 1代 Logo
├── llama1-logo.svg        # 1代 Logo (SVG)
└── matmul.png/svg         # 矩阵乘法可视化图表
```

## 图像说明

### llama0 系列

llama.cpp 0代的品牌图像：

- **llama0-banner.png** - 项目横幅图像
- **llama0-logo.png** - 项目 Logo

### llama1 系列

llama.cpp 1代的品牌图像：

| 文件 | 格式 | 大小 | 说明 |
|------|------|------|------|
| llama1-banner.png | PNG | 32KB | 项目横幅图像 |
| llama1-icon-transparent.png | PNG | 14KB | 透明背景图标 |
| llama1-icon-transparent.svg | SVG | 2.7KB | 透明背景图标（矢量） |
| llama1-icon.png | PNG | 16KB | 项目图标 |
| llama1-icon.svg | SVG | 2.9KB | 项目图标（矢量） |
| llama1-logo.png | PNG | 32KB | 项目 Logo |
| llama1-logo.svg | SVG | 2.4KB | 项目 Logo（矢量） |

### SVG 优势

推荐使用 SVG 格式，因为：

- ✅ 无损缩放
- ✅ 文件更小
- ✅ 支持透明度
- ✅ 适合网页和打印

### matmul 可视化

- **matmul.png** - 矩阵乘法操作的 PNG 可视化
- **matmul.svg** - 矩阵乘法操作的 SVG 可视化（矢量格式）

## 使用场景

### 文档

在文档中引用图像：

```markdown
![llama.cpp Logo](media/llama1-logo.svg)
```

### 网页

在网页中使用：

```html
<img src="media/llama1-icon.svg" alt="llama.cpp Icon" />
```

### 演示文稿

在演示文稿中使用：

- SVG 适合高分辨率投影
- PNG 适合确保兼容性

## 图像格式

### PNG (Portable Network Graphics)

**优点：**
- 无损压缩
- 支持透明度
- 广泛支持

**用途：**
- 需要透明背景的图像
- 兼容性优先的场景

### SVG (Scalable Vector Graphics)

**优点：**
- 无限缩放不失真
- 文件体积小
- 支持动画和交互

**用途：**
- Logo 和图标
- 图表和示意图
- 需要灵活缩放的图像

## 推荐使用

### Logo 使用

```markdown
# 文档中使用（推荐 SVG）
![Logo](media/llama1-logo.svg)

# 如果 SVG 不支持，使用 PNG
![Logo](media/llama1-logo.png)
```

### 图标使用

```markdown
# 网页导航栏（推荐 SVG）
<img src="media/llama1-icon.svg" width="32" height="32" />

# 社交媒体头像（推荐 PNG）
<img src="media/llama1-icon.png" width="128" height="128" />
```

### 横幅使用

```markdown
# 网页横幅
<img src="media/llama1-banner.png" alt="llama.cpp Banner" />
```

## 颜色规范

llama.cpp 品牌使用以下颜色（根据图像推断）：

- **主色调** - 深灰色/黑色
- **背景** - 白色或透明
- **装饰** - 简洁的设计风格

## 尺寸规范

| 用途 | 推荐尺寸 |
|------|----------|
| 图标 | 32x32, 64x64, 128x128 |
| Logo | 256x256, 512x512 |
| 横幅 | 宽度 ≥ 1200px |

## 修改和定制

### 颜色修改

使用 SVG 编辑器（如 Inkscape）修改颜色：

```bash
# 打开 SVG 文件
inkscape media/llama1-logo.svg
```

### 大小调整

SVG 可以无损调整大小：

```bash
# 使用 ImageMagick 调整 PNG
convert media/llama1-logo.png -resize 512x512 media/llama1-logo-512.png
```

### 格式转换

转换为其他格式：

```bash
# SVG 转 PNG
convert media/llama1-logo.svg media/llama1-logo.png

# PNG 转 SVG（需手动描摹）
# 使用 Inkscape 或其他矢量化工具
```

## 浏览器支持

| 格式 | 支持度 | 备注 |
|------|--------|------|
| PNG | 100% | 全部支持 |
| SVG | 95%+ | 现代浏览器完全支持 |

## 文件大小

| 文件 | 大小 |
|------|------|
| llama0-banner.png | 145KB |
| llama0-logo.png | 180KB |
| llama1-banner.png | 33KB |
| llama1-icon-transparent.png | 14KB |
| llama1-icon-transparent.svg | 2.7KB |
| llama1-icon.png | 16KB |
| llama1-icon.svg | 2.9KB |
| llama1-logo.png | 33KB |
| llama1-logo.svg | 2.4KB |
| matmul.png | 266KB |
| matmul.svg | 54KB |

## 图像编辑工具

### 推荐工具

**SVG 编辑：**
- [Inkscape](https://inkscape.org/) - 开源矢量图形编辑器
- [Figma](https://www.figma.com/) - 在线设计工具
- [Adobe Illustrator](https://www.adobe.com/products/illustrator.html) - 专业矢量编辑

**PNG 编辑：**
- [GIMP](https://www.gimp.org/) - 开源图像编辑器
- [Adobe Photoshop](https://www.adobe.com/products/photoshop.html) - 专业图像编辑
- [ImageMagick](https://imagemagick.org/) - 命令行工具

## 性能优化

### Web 优化

```bash
# 使用 optipng 优化 PNG
optipng media/*.png

# 使用 svgo 优化 SVG
svgo media/*.svg
```

### 响应式图像

根据使用场景选择合适尺寸：

```html
<!-- 响应式图像 -->
<picture>
  <source srcset="media/llama1-logo.svg" type="image/svg+xml">
  <img src="media/llama1-logo.png" alt="llama.cpp Logo" width="256">
</picture>
```

## 版权和许可

llama.cpp 使用 MIT 许可证，包括这些媒体文件：

- ✅ 允许商业使用
- ✅ 允许修改
- ✅ 允许分发
- ✅ 需要保留许可证声明

## 常见问题

### Q: 哪个 Logo 版本应该使用？

A: 使用最新的 llama1 系列图像，特别是 SVG 格式。

### Q: 可以修改颜色吗？

A: 可以，MIT 许可证允许修改。

### Q: SVG 比 PNG 有什么优势？

A: SVG 可以无限缩放不失真，文件更小，适合现代网页。

### Q: 如何在网站中使用？

A: 使用 HTML `<img>` 标签或 Markdown 语法引用图像。

## 相关文档

- [主项目 README](../README.md)
- [文档目录总览](../docs/README_ZH.md)
- [构建指南](../docs/build.md)

## 贡献

欢迎贡献新的媒体文件：

1. 遵循现有设计风格
2. 提供多种格式（PNG + SVG）
3. 保持文件大小合理
4. 更新本文档

## 参考资料

- [SVG 规范](https://www.w3.org/Graphics/SVG/)
- [PNG 规范](https://www.w3.org/TR/PNG/)
- [图像优化指南](https://developer.mozilla.org/en-US/docs/Learn/Performance/Optimizing_images)