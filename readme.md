
# 技术文档

（github pages） https://xdedss.github.io/gldemo/index.html 

该文档由doxygen生成，Travis CI自动部署[![Build Status](https://travis-ci.com/xdedss/gldemo.svg?branch=main)](https://travis-ci.com/xdedss/gldemo)

# 环境配置

Windows Qt 5.12.x MSVC

IDE建议使用Visual Studio + [Qt Visual Studio Tools](https://marketplace.visualstudio.com/items?itemName=TheQtCompany.QtVisualStudioTools-19123)

# 注意事项

- 目录下bun180.ply、bun000.ply、uwo.txt是运行时动态加载的点云文件，shaders文件夹下是运行时动态加载的着色器代码，如果调试时运行路径并非代码根目录，则需要把上述文件和文件夹复制到运行路径才能正常加载。
- 代码中的中文内容统一采用utf-8编码，请对环境进行相应配置以避免乱码。
- 项目使用了OpenCV，如果是在windows上，需要手动配置相关库文件。
