


# 基于OpenGL和Qt的3D点云渲染

## 代码组织结构
 - <a href="annotated.html">class概览</a>
 - <a href="hierarchy.html">按继承关系查看</a>
 - <a href="https://github.com/xdedss/gldemo" target="_blank">源码仓库</a>

## 项目概述

  本项目的主要任务是基于OpenGL和QT完成三维点云可视化漫游界面开发，进行漫游渲染器开发。即实现一个能完成三维模型加载、可视化交互、漫游订制功能的3D点云查看器。本项目可以应用于游戏开发、3D全景展示、VR全景直播、汽车模拟驾驶、三维设计软件的应用等多个领域。

## 功能说明

1. 模型加载：将模型文件（\*.txt，\*.ply）拖入到程序窗口中，程序将会加载模型，并将其显示在界面中；
2. 窗口的交互：鼠标左键点击模型，可将模型选中；鼠标右键可对界面进行旋转操作；WASD键可以对界面进行平移操作；鼠标滚轮可以对界面进行缩放操作；
3. 模型参数调整：选中模型（或轨迹点）后，右侧可以改变模型的平移、缩放和旋转参数；也可以调整一些其他参数，如模型是否显示，模型内点的形状大小等；
4. 窗口左侧树形控件的使用：左键点击节点名称可以选中该模型及其附属的所有子模型，对节点（模型）进行任何操作，则其所有的子节点（模型）也会进行同样的操作。鼠标左键拖动可以调整节点间的从属关系，右键可以选择删除模型或者给其新增一个子节点；
5. 漫游轨迹：系统会根据左侧树形控件中的trailTest内的点，按照从上往下的顺序拟合出轨迹曲线。轨迹点可以进行单独的参数调整，也可以进行添加或删除，从而改变漫游的轨迹；
6. 3d漫游：菜单栏Record->videoRecord，滑动条可以调整漫游的速度，下方三个按钮中Preview是仅漫游不录制，Record是漫游并录制，在录制后，可以点击Save将漫游的过程保存为视频。

## 搭建过程

### 图形界面搭建

软件的图形界面采用Qt Creatord搭建。主要包含6个部分：

\image html m-00.png

1. 菜单栏
 
 文件打开、屏幕录制功能
 
2. 工具栏
 
 一共有四个工具。
 
 - 选择工具：可以使用鼠标在场景中选中物体
 
 - 移动工具：选中物体后鼠标拖动即可移动物体
 
 - 旋转工具：选中物体后鼠标拖动即可旋转物体
 
 - 缩放工具：选中物体后鼠标上下拖动即可整体调节物体缩放
 
3. 场景树
 
 用于显示当前3D场景中的各个节点。节点是以树形结构存储的，拖动节点并放到另一个节点之上可以调整节点的附属关系。右键单击节点可以添加或删除节点。
 
4. 渲染界面

 显示实时3D渲染结果。可以在图形界面内拖动鼠标或使用键盘快捷键调整摄像机视角。
 
5. 节点基本属性编辑框

 可以编辑当前选中节点的位移、旋转和缩放。

6. 组件属性框

 列出当前选中节点上所有组件（Component）的所有属性，可以双击编辑。
 
### 渲染管线搭建

OpenGL提供了直接绘制图形的`glBegin`、`glVertex..`等函数，但是这些函数效率并不高，考虑到我们需要实时渲染十万量级以上的点云，且需要考虑相机、模型的运动，我们搭建了场景中模型的树形结构和配套的渲染管线。

渲染管线主要由Widget类进行调度。初始化过程中Widget类的实例会初始化OpenGL上下文、编译shader以及创建坐标轴等辅助线的渲染器。一个QTimer每20毫秒触发一次场景更新及重绘。

渲染时，从场景根节点开始，将其transform矩阵入栈，依次乘以子节点的transform矩阵，并递归渲染子节点。每个节点在渲染过程中会检查其上是否存在继承自Renderer类的组件，如果有就调用Renderer类的统一接口进行渲染。每一个Renderer的子类都会实现一套自身的渲染流程，目前一共实现了PointCloudRenderer，SkyboxRenderer和LineRenderer三种渲染器。这种物体-组件的设计使得我们能够很容易地扩展新的渲染器而不用对渲染管线的底层进行修改。

具体说明情参考技术细节中的点云渲染一节。

### 事件传递搭建

Qt提供了非常方便的信号槽机制。我们利用信号槽实现了渲染窗口和主窗口之间各种信号的同步。

信号槽的有两种方式，一种是在QtCreator中编辑，一种是在初始化时使用connect函数。

- 按钮回调函数

 UI上的按钮被点击后，产生特定信号。使用QtCreator的信号槽编辑功能将这些信号连接到自定义的事件处理代码中。

- 物体选择信号

 用户既可以在渲染窗口中使用鼠标左键选择物体，也可以在主窗口左侧的场景结构中选中物体节点。所以用户在渲染窗口中选择物体之后，会有一个信号发出，通知场景结构更新用户选取的物体，用户在场景结构中选择节点后，也会有一个信号发出，通知渲染窗口高亮选中的物体。
 
- 漫游录制信号

 漫游录制的具体设定在一个新的弹出窗口中。用户确认设定之后，弹出窗口要通过信号槽通知主窗口用户进行了什么操作。主窗口则需要给渲染窗口发送信号实施对应的操作。当录制/保存完成后，渲染窗口还需要通知主窗口任务完成。具体信号流程情参考技术细节中的漫游录制一节。

### 用户输入系统搭建

如果对每个鼠标、键盘事件都分别监听，代码分散在不同的槽函数中，难以维护，且涉及到组合操作和拖动操作时需要设立大量成员变量来临时记录上一帧的输入值，很不方便。所以我们将所有用户输入放入静态的Input类进行统一的管理。每一次渲染更新之前Input类会将上一帧到这一帧的时间范围内记录到的所有事件都记录下来并放入缓存，同时开始接收新的输入事件。所有需要用到用户输入的代码逻辑都可以直接调用Input类的查询接口来获取当前帧发生的事件。



## 技术细节
 - [点云渲染](group__tech-pointcloud.html)
 - [天空渲染](group__tech-skybox.html)
 - [鼠标拾取物体](group__tech-mousepick.html)
 - [漫游插值](group__tech-roveint.html)
 - [漫游录制](group__tech-roverec.html)

## 使用的框架
 - Qt
 - OpenGL
 - glm
 - OpenCV （录像功能）
 - nanoflann （kdtree）


