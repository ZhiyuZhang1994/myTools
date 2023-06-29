### 任务概述

将Actor(如: vtkAxesActor)、png图片数据以像素大小显示

1）参考资料：https://discourse.vtk.org/t/is-it-possible-to-use-vtkdistancetocamera-to-keep-an-actor-size-fixed-without-using-vtkglyph3d/1559

2）vtkGlyph3DMapper与vtkDataSetMapper、vtkPolyDataMapper的区别

3）完成代码实现

4）vtkAxesActor：使用vtkGlyph3DMapper；

​		png图片：使用vtkDistanceToCamera与vtkGlyph3D



### 来源

https://discourse.vtk.org/t/is-it-possible-to-use-vtkdistancetocamera-to-keep-an-actor-size-fixed-without-using-vtkglyph3d/1559/2

Hi,

First of all, there’s this example : [https://vtk.org/Wiki/VTK/Examples/Cxx/Visualization/DistanceToCamera 25](https://kitware.github.io/vtk-examples/site/Cxx/Visualization/DistanceToCamera/) that shows how to keep 2 arrows size fixed, whatever the camera zoom level using vtkDistanceToCamera and vtkGlyph3D.

I took this last example and I successfully replaced the arrows by a PNG image file texture mapped on a plane. I used this example to create the PNG glyphs : [https://vtk.org/Wiki/VTK/Examples/Cxx/Visualization/TextureMapPlane 5](https://vtk.org/Wiki/VTK/Examples/Cxx/Visualization/TextureMapPlane/)

My question is : Is there another way other than using vtkGlyph3D to use vtkDistanceToCamera to keep the PNG image size fixed (or even a vtkArrowSource).

Thanks.

The answer is yes, I have to use a vtkGlyph3DMapper instead of vtkPolyDataMapper.

### vtkDistanceToCamera实现固定像素大小

#### vtkAlgorithm介绍

vtkAlgorithm：[vtkAlgorithm](https://vtk.org/doc/nightly/html/classvtkAlgorithm.html) is the superclass for all sources, filters, and sinks in VTK

直接的子类有：vtkDataSetAlgorithm、vtkImageAlgorithm、vtkPointSetAlgorithm、vtkPolyDataAlgorithm等。

而vtkDistanceToCamera继承自vtkPointSetAlgorithm→vtkAlgorithm.

#### 结合vtkGlyph3D+vtkPolyDataMapper使用

第一种方式：vtkDistanceToCamera+vtkGlyph3D+vtkPolyDataMapper

vtkPointSource(vtkPolyDataAlgorithm→vtkAlgorithm)→vtkDistanceToCamera(vtkPointSetAlgorithm→vtkAlgorithm)→vtkGlyph3D(vtkPolyDataAlgorithm→vtkAlgorithm,SetInputConnection(distanceToCamera->GetOutputPort()))。

与vtkGlyph3D::SetSourceConnection(arrow->GetOutputPort())一起，传入vtkPolyDataMapper中

代码：

```C++
//-------------------------1.vtkDistanceToCamera+vtkGlyph3D+vtkPolyDataMapper，实现固定像素显示
    vtkNew<vtkGlyph3D> fixedGlyph;
    fixedGlyph->SetInputConnection(distanceToCamera->GetOutputPort());
    fixedGlyph->SetSourceConnection(arrow->GetOutputPort());
    // Scale each point.
    fixedGlyph->SetScaleModeToScaleByScalar(); //可以没有
    fixedGlyph->SetInputArrayToProcess(
        0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "DistanceToCamera"); //不可缺少，name不可改变长度与大小写
    // Create a mapper.
    vtkNew<vtkPolyDataMapper> fixedMapper;
    fixedMapper->SetInputConnection(fixedGlyph->GetOutputPort());
    fixedMapper->SetScalarVisibility(false);
```

#### 结合vtkGlyph3DMapper使用

源码对vtkGlyph3DMapper的解释：

```C++
Do the same job than vtkGlyph3D but on the GPU. For this reason, it is a mapper not a vtkPolyDataAlgorithm. Also, some methods of vtkGlyph3D don't make sense in vtkGlyph3DMapper: GeneratePointIds, old-style SetSource, PointIdsName, IsPointVisible.
```



第二种方式：vtkDistanceToCamera+vtkGlyph3DMapper

vtkPointSource(vtkPolyDataAlgorithm→vtkAlgorithm)→vtkDistanceToCamera→vtkGlyph3DMapper。

```C++
 //-------------------------2.vtkDistanceToCamera+vtkGlyph3DMapper，实现固定像素显示
    vtkNew<vtkGlyph3DMapper> glyph3DMapper;
    glyph3DMapper->SetInputConnection(distanceToCamera2->GetOutputPort());
    glyph3DMapper->SetSourceConnection(sphere->GetOutputPort());
    //要有SetScaleModeToScaleByMagnitude().
    // SetScaleModeToScaleNoDataScaling不起作用，SetScaleModeToScaleByVectorComponents需要DistanceToCamera have 3 components,
    glyph3DMapper->SetScaleModeToScaleByMagnitude();
    glyph3DMapper->SetScalarVisibility(false);
    glyph3DMapper->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "DistanceToCamera");//不可缺少，name不可改变长度与大小写

```

#### 注意事项

1. vtkGlyph3DMapper本身只是一种融入了vtkGlypgh3D的mapper，想要实现固定像素显示还需要vtkDistanceToCamara.
2. 两种方式都需要额外的一步：    distanceToCamera->SetRenderer(renderer);
3. 顺序不能颠倒： 必须先设置渲染器`distanceToCamera->SetRenderer(renderer);`，再添加固定像素的actor，`renderer->AddActor(fixedActor);`



### 图片以固定像素显示

#### 纹理贴图相关知识

官方示例：[https://vtk.org/Wiki/VTK/Examples/Cxx/Visualization/TextureMapPlane 5](https://vtk.org/Wiki/VTK/Examples/Cxx/Visualization/TextureMapPlane/)

这个示例读取一个jpg图像文件，并使用它对平面进行纹理贴图.

 先读入一幅JPEG的二维纹理图;

然后定义一个纹理类vtkTexture对象，接着把读入的JPEG图像输入到vtkTexture里，作为它即将“贴”到平面上的一个纹理图;

再定义一个vtkPlaneSource对象，类vtkPlaneSource可以生成一个平面，也就是纹理图要“贴”图的地方。

简而言之，在做纹理贴图时，先要有东西可以“贴”，也就是要准备一幅二维的纹理图；然后再确定这幅纹理图要“贴”到哪里。

管道1，从外部图片开始：

外部图片→`vtkJPEGReader`(vtkImageReader2→vtkImageAlgorithm→vtkAlgorithm)→`vtkTexture`(vtkImageAlgorithm→vtkAlgorithm)→`vtkActor`(SetTexture())

管道2，从定义平面开始：

`vtkPlaneSource`(vtkPolydataAlgorithm→vtkAlgorithm)→`vtkTextureMapToPlane`(vtkDataSetAlgorithm→vtkAlgorithm)→`vtkPolyDataMapper`(...>vtkAbstractMapper→vtkAlgorithm)→`vtkActor`(SetMapper());

或者不用vtkTextureMapToPlane

vtkPlaneSource`(vtkPolydataAlgorithm→vtkAlgorithm)`→`vtkPolyDataMapper`(...>vtkAbstractMapper→vtkAlgorithm)→`vtkActor`(SetMapper());

#### 贴图改为固定像素显示

主要是加入vtkDistanceToCamera、把vtkPolyDataMapper改为vtkGlyph3DMapper，再增加一条管道：

更改后的管道2，从定义平面开始，把vtkPolyDataMapper改为vtkGlyph3DMapper：

`vtkPlaneSource`(vtkPolydataAlgorithm→vtkAlgorithm)→`vtkTextureMapToPlane`(vtkDataSetAlgorithm→vtkAlgorithm)→`vtkGlyph3DMapper`;

管道3：

vtkPointSource(vtkPolyDataAlgorithm→vtkAlgorithm)→vtkDistanceToCamera→vtkGlyph3DMapper

### 3种Mapper的区别

vtkGlyph3DMapper与vtkDataSetMapper、vtkPolyDataMapper的区别。

首先，几种mapper都继承自vtkMapper**抽象类**,官方对vtkMapper的说明：

vtkMapper is an abstract class to specify interface between data and graphics primitives. Subclasses of vtkMapper map data through a lookuptable and control the creation of rendering primitives that interface to the graphics library. The mapping can be controlled by supplying a lookup table and specifying a scalar range to map data through.

https://blog.csdn.net/charce_you/article/details/94627490讲述了几种Mapper的继承关系以及用法

https://blog.csdn.net/hit1524468/article/details/104887781展示了vtkGlyph3DMapper与vtkDataSetMapper一起使用

两个vtkGlyph3DMapper的算例中标注了特有方法。

#### 总结

vtkGlyph3DMapper本身只是一种融入了vtkGlypgh3D的mapper，想要实现固定像素显示还需要vtkDistanceToCamara. 可以传入vtkDataSetAlgorithm类型的数据（vtkTextureMapToPlane），也可以传入vtkPolydataAlgorithm（vtkPlaneSource）类型的数据。



### 问题

vtkAxesActor与vtkActor平级，都继承自vtkProp3D，没有getMapper()函数，如何通过使用vtkGlyph3DMapper来固定像素？

vtkMapper也属于vtkAlgorithm，可视化管线结束于Mapper？