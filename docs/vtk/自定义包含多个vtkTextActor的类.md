### 会议笔记

如果要改变不透明度，对于普通actor，搞多个标量属性，搞成多个单元就可以了。一条线的话，再拆分成多个单元，加标量值+查找表就过于复杂了。方案：利用多个vtkTextActor，每个的文本内容是-----，每个的不透明度不同，就可以模拟不透明度渐变。可以将多个vtkTextActor封装成一个类。

vtkTextActor→vtkTexturedActor2D→vtkActor2D→vtkProp，vtkCaptionActor2D→vtkActor2D，vtkCaptionActor2D里面就包含了vtkTextActor。

可以参考vtkCaptionActor2D：

```C++
  vtkTextActor* TextActor;
  vtkTextProperty* CaptionTextProperty;

  vtkPolyData* BorderPolyData;
  vtkPolyDataMapper2D* BorderMapper;
  vtkActor2D* BorderActor;

  vtkPolyData* HeadPolyData;       // single attachment point for glyphing
  vtkGlyph3D* HeadGlyph;           // for 3D leader
  vtkPolyData* LeaderPolyData;     // line represents the leader
  vtkAppendPolyData* AppendLeader; // append head and leader

  // for 2D leader
  vtkCoordinate* MapperCoordinate2D;
  vtkPolyDataMapper2D* LeaderMapper2D;
  vtkActor2D* LeaderActor2D;

  // for 3D leader
  vtkPolyDataMapper* LeaderMapper3D;
  vtkActor* LeaderActor3D;

  vtkCaptionActor2DConnection* LeaderGlyphConnectionHolder;
```

自定义一个继承自vtkActor2D的类，里面放5个vtkTextActor对象，每个的文本内容是-----，但不透明度不同，就可以模拟不透明度渐变。

分解：

VTK显示渐变实线：
 1）重载vtkActor2D管理多个vtkTextActor同步渲染
 2）如何把多个线段连起来显示为一条实线 

![1683456823342](自定义包含多个vtkTextActor的类\zhangzy代码.png)

可以通过98行代码实现定义起始位置。

先研究如何实现重写一个vtkActor层面的类。

渲染的时候会调用以下接口：

![1683457472415](自定义包含多个vtkTextActor的类\zhangzy代码2.png)

在这里就要把chuangjiandeactor渲染掉。



有用笔记：

### textActor类的学习

#### 相关接口

```C++
textActor->GetProperty()->SetColor(1, 1, 1);//设置颜色
textActor->GetProperty()->SetOpacity(1);//设置不透明度

textActor->GetTextProperty()->SetFontSize(18);//设置字体大小
textActor->SetInput("————————————");//设置文本为直线
```

#### 获取像素尺寸

```C++
double size[2] = {0, 0};
textActor->GetSize(renderer, size);
```

```C++
/**
   * Syntactic sugar to get the size of text instead of the entire bounding box.
   */
  virtual void GetSize(vtkViewport* vport, double size[2]);
```

获取文本的大小，而不是整个边框的大小的语法糖。

经测试，size指的是矩形框的像素尺寸长与宽，size[0]有用。

#### 设置直线长度

```C++
textActor->GetProperty()->SetColor(1, 1, 1);
textActor->GetTextProperty()->SetFontSize(18);
textActor->SetInput("————————————");//共12条短线
```

#### 验证算例1

12条短线，FontSize：18



![1683467238351](自定义包含多个vtkTextActor的类\vtkTextActor显示线的效果.png)

实测像素：217

![1683467191030](自定义包含多个vtkTextActor的类\实测像素.png)

代码打印尺寸：size: 215  17。

215与217相差2像素，可看作测量误差。

#### 验证算例2

短线减半，尺寸不变：

```C++
textActor->GetProperty()->SetColor(1, 1, 1);
textActor->GetTextProperty()->SetFontSize(18);
textActor->SetInput("——————");//6条短线
```

实测像素：109

![1683468548126](自定义包含多个vtkTextActor的类\实测像素2.png)



打印像素：107

```C++
size: 107  17
```

105与107相差2像素，可看作测量误差。

说明输出的像素值准确，接下来不再测量。

#### 验证算例3

只有2条小短线

```C++
textActor->SetInput("——");
```



![1683507322716](自定义包含多个vtkTextActor的类\vtkTextActor显示线的效果-2条小短线.png)

打印像素：35

```C++
size: 35  17
```

#### 验证算例4

12条短线，fontSize减半，从18变为9：

```C++
textActor->GetTextProperty()->SetFontSize(9);
textActor->SetInput("————————————");
```

测量值：

![1683509103899](自定义包含多个vtkTextActor的类\实测像素4.png)

打印

```C++
size: 107  8
```

测量值为109，真实值为107，FontSize取18时真实值为215，恰好是1/2

#### 结论

可以得到横线的长度的像素尺寸，但需要用到renderer.



### vtkCaptionActor2D类的学习

vtkCaptionActor2D→vtkActor2D→vtkProp→vtkObject

vtkTextActor→vtkTexturedActor2D→vtkActor2D

vtkCaptionActor2D包含的元素：

```C++
protected:
    vtkMyTextActor();
    ~vtkMyTextActor() override;

    vtkCoordinate* AttachmentPointCoordinate;

    vtkTypeBool Border;
    vtkTypeBool Leader;
    vtkTypeBool ThreeDimensionalLeader;
    double LeaderGlyphSize;
    int MaximumLeaderGlyphSize;

    int Padding;
    vtkTypeBool AttachEdgeOnly;

private:
    vtkTextActor* TextActor;
    vtkTextProperty* CaptionTextProperty;

    vtkPolyData* BorderPolyData;
    vtkPolyDataMapper2D* BorderMapper;
    vtkActor2D* BorderActor;

    vtkPolyData* HeadPolyData;       // single attachment point for glyphing
    vtkGlyph3D* HeadGlyph;           // for 3D leader
    vtkPolyData* LeaderPolyData;     // line represents the leader
    vtkAppendPolyData* AppendLeader; // append head and leader

    // for 2D leader
    vtkCoordinate* MapperCoordinate2D;
    vtkPolyDataMapper2D* LeaderMapper2D;
    vtkActor2D* LeaderActor2D;

    // for 3D leader
    vtkPolyDataMapper* LeaderMapper3D;
    vtkActor* LeaderActor3D;

    vtkCaptionActor2DConnection* LeaderGlyphConnectionHolder;

private:
    vtkMyTextActor(const vtkMyTextActor&) = delete;
    void operator=(const vtkMyTextActor&) = delete;
```



vtkCaptionActor2D和vtkTextActor是VTK中用于显示文本的两个类，它们在功能和用法上有一些区别。

1. 功能和用途：
   - vtkCaptionActor2D：用于在图形场景中显示带有注释或标题的文本，通常用于添加注释或标签到图形对象上。它支持设置文本内容、位置、字体、大小、对齐方式、边框等属性，并且可以自动调整位置以避免与其他图形对象重叠。
   - vtkTextActor：用于在图形场景中显示普通的文本信息，可以显示任意文本内容，通常用于显示自定义的文本标注、说明或其他信息。它支持设置文本内容、位置、字体、大小、对齐方式等属性，但不提供自动调整位置的功能。

2. 定位方式：
   - vtkCaptionActor2D：可以根据所附加的图形对象自动调整位置，使文本不与图形对象重叠。可以使用SetAttachmentPoint()方法设置要附加的图形对象的坐标点，文本将根据该点自动调整位置。
   - vtkTextActor：需要手动指定文本的位置，使用SetPosition()方法设置文本的坐标点，文本将始终显示在指定的位置。

3. 显示样式：
   - vtkCaptionActor2D：提供了更多的显示样式选项，包括设置边框、边框颜色、背景颜色、背景不透明度等属性，可以创建更具视觉效果的文本标注。
   - vtkTextActor：提供了基本的文本显示功能，没有提供设置边框、背景等样式属性。

综上所述，vtkCaptionActor2D适合在图形场景中添加注释或标题，并具有自动调整位置和更多显示样式选项的功能。vtkTextActor适合显示普通的文本信息，位置需要手动指定，没有提供自动调整位置和额外的显示样式选项。选择使用哪个类取决于您的具体需求和应用场景。



#### 渲染函数

vtkCaptionActor2D→vtkActor2D→vtkProp→vtkObject，vtkProp是渲染函数的源头，从上往下看：

##### vtkProp

vtkProp是虚基类，官网介绍：

vtkProp is an abstract superclass for any objects that can exist in a rendered scene (either 2D or 3D). Instances of vtkProp may respond to various render methods (e.g., RenderOpaqueGeometry()). vtkProp also defines the API for picking, LOD manipulation, and common instance variables that control visibility, picking, and dragging.

vtkProp是一个抽象超类，用于可以存在于渲染场景（2D或3D）中的任何对象。vtkProp的实例可能会响应各种渲染方法（例如RenderOpaqueGeometry（））。vtkProp还定义了用于拾取、LOD操作的API，以及控制可见性、拾取和拖动的通用实例变量。

源代码以及描述：

```C++
/**
   * WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
   * DO NOT USE THESE METHODS OUTSIDE OF THE RENDERING PROCESS
   * All concrete subclasses must be able to render themselves.
   * There are four key render methods in vtk and they correspond
   * to four different points in the rendering cycle. Any given
   * prop may implement one or more of these methods.
   * The first method is intended for rendering all opaque geometry. The
   * second method is intended for rendering all translucent polygonal
   * geometry. The third one is intended for rendering all translucent
   * volumetric geometry. Most of the volume rendering mappers draw their
   * results during this third method.
   * The last method is to render any 2D annotation or overlays.
   * Each of these methods return an integer value indicating
   * whether or not this render method was applied to this data.
   */
  virtual int RenderOpaqueGeometry(vtkViewport*) { return 0; }
  virtual int RenderTranslucentPolygonalGeometry(vtkViewport*) { return 0; }
  virtual int RenderVolumetricGeometry(vtkViewport*) { return 0; }
  virtual int RenderOverlay(vtkViewport*) { return 0; }
```

警告：内部方法-不用于一般用途不要在渲染过程之外使用这些方法

所有具体的子类都必须能够呈现自己。

vtk中有四种关键渲染方法，它们对应于渲染周期中的四个不同点。任何给定的prop都可能实现这些方法中的一个或多个。

第一种方法用于渲染所有不透明的几何体。

第二种方法用于渲染所有半透明多边形几何体。

第三个用于渲染所有半透明的体积几何体。大多数体渲染映射器在第三种方法中绘制结果。

最后一种方法是渲染任何二维注释或覆盖。

这些方法中的每一个都返回一个整数值，指示此渲染方法是否应用于此数据。

chatGPT:

在VTK中，vtkProp是表示可渲染对象的基类。它定义了一些关键的渲染方法，包括：

1. RenderOpaqueGeometry()：该方法用于绘制对象的不透明几何体。在渲染过程中，它会计算对象的位置、大小和外观属性，并生成相应的几何体，然后将其添加到渲染器的不透明几何体列表中。这些几何体将与其他不透明几何体一起进行混合、光照等处理，最终在渲染窗口中显示出来。

2. RenderTranslucentPolygonalGeometry()：该方法用于绘制对象的半透明几何体。与RenderOpaqueGeometry()类似，它会生成对象的几何体并将其添加到渲染器的半透明几何体列表中。这些几何体将与其他半透明几何体一起进行混合和排序，以正确呈现半透明效果。

3. RenderOverlay()：该方法用于绘制对象的叠加层几何体。叠加层几何体通常是显示在其他几何体之上的额外内容，例如标注、边框或交互式元素。RenderOverlay()方法将生成并添加叠加层几何体到渲染器的叠加层几何体列表中。

4. Pick()：该方法用于在渲染过程中进行拾取操作。当用户与渲染窗口交互并选择一个对象时，渲染器会调用该方法来检测选择的对象。Pick()方法将根据用户选择的坐标位置与对象的几何体进行相交测试，并返回与之相交的对象。

这些渲染方法在VTK中的各种派生类中具体实现，以满足不同类型的对象的渲染需求。通过重写这些方法，可以自定义对象的渲染行为，实现特定的绘制效果和交互行为。



##### vtkActor2D

重写了这三种：

```C++
 ///@{
  /**
   * Support the standard render methods.
   */
  int RenderOverlay(vtkViewport* viewport) override;
  int RenderOpaqueGeometry(vtkViewport* viewport) override;
  int RenderTranslucentPolygonalGeometry(vtkViewport* viewport) override;
  ///@}
```



vtkActor2D的RenderOpaqueGeometry()方法

```C++
// Renders an actor2D's property and then it's mapper.
int vtkActor2D::RenderOpaqueGeometry(vtkViewport* viewport)
{
  vtkDebugMacro(<< "vtkActor2D::RenderOpaqueGeometry");

  if (!this->Property)
  {
    vtkDebugMacro(<< "vtkActor2D::Render - Creating Property2D");
    // Force creation of default property
    this->GetProperty();
  }

  this->Property->Render(viewport);

  if (!this->Mapper)
  {
    vtkErrorMacro(<< "vtkActor2D::Render - No mapper set");
    return 0;
  }

  this->Mapper->RenderOpaqueGeometry(viewport, this);

  return 1;
}
```



##### vtkCaptionActor2D

跟vtkActor2D一样，重写了三种：

```C++
  /**
   * WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
   * DO NOT USE THIS METHOD OUTSIDE OF THE RENDERING PROCESS.
   * Release any graphics resources that are being consumed by this actor.
   * The parameter window could be used to determine which graphic
   * resources to release.
   */
    void ReleaseGraphicsResources(vtkWindow*) override;//释放资源，较简单

    ///@{
    /**
   * WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
   * DO NOT USE THIS METHOD OUTSIDE OF THE RENDERING PROCESS.
   * Draw the legend box to the screen.
   */
	//渲染不透明几何体，很长，难
    int RenderOpaqueGeometry(vtkViewport* viewport) override;
	//渲染半透明的多边形几何体，不用重写
    int RenderTranslucentPolygonalGeometry(vtkViewport*) override { return 0; }
	//渲染叠加层几何体，将真正出现的actor的各个RenderOverlay(viewport)相加，较简单
    int RenderOverlay(vtkViewport* viewport) override;
    ///@}
```

chatGPT:

vtkCaptionActor2D类的RenderOpaqueGeometry()函数是用于渲染标注的不透明几何体的方法。在VTK的渲染过程中，渲染器会调用该函数来生成标注的不透明几何体，并将其添加到渲染场景中。

具体来说，RenderOpaqueGeometry()函数的作用如下：

1. 根据标注的属性（如文本内容、附着点、箭头指示器等），计算标注框的位置和大小。
2. 创建表示标注框的几何体，通常是一个矩形。
3. 根据标注的背景颜色、边框等属性，设置标注框的外观。
4. 将标注框的几何体添加到渲染器的几何体列表中，以便在渲染过程中进行绘制。

RenderOpaqueGeometry()函数在VTK的渲染流水线中的适当阶段被调用，以确保标注在正确的时间和顺序进行渲染。它负责生成标注的可见部分，并与其他几何体一起进行混合、光照等处理，最终在渲染窗口中显示出来。

注意：RenderOpaqueGeometry()函数只负责绘制标注的不透明部分，对于标注的透明部分，需要使用RenderTranslucentPolygonalGeometry()函数进行渲染。

##### vtkScalarBarActor

vtkScalarbarActor.h也是重写了这三个

```C++
  ///@{
  /**
   * Draw the scalar bar and annotation text to the screen.
   */
  int RenderOpaqueGeometry(vtkViewport* viewport) override;
  int RenderTranslucentPolygonalGeometry(vtkViewport*) override { return 0; }
  int RenderOverlay(vtkViewport* viewport) override;
  ///@}
```

它的RenderOpaqueGeometry()函数较为规整：

判断对应分量是否显示，然后调用相应分量的RenderOpaqueGeometry()函数，+=

```C++
//------------------------------------------------------------------------------
int vtkScalarBarActor::RenderOpaqueGeometry(vtkViewport* viewport)
{
  if (!this->RebuildLayoutIfNeeded(viewport))
  {
    return 0;
  }

  int renderedSomething = 0;

  // Everything is built, just have to render
  if (this->Title != nullptr)
  {
    renderedSomething += this->TitleActor->RenderOpaqueGeometry(viewport);
  }
  // Draw either the scalar bar (non-indexed mode) or
  // the annotated value boxes (indexed mode).
  if (!this->LookupTable->GetIndexedLookup())
  {
    if (this->DrawColorBar)
    {
      renderedSomething += this->ScalarBarActor->RenderOpaqueGeometry(viewport);
    }
    vtkScalarBarActorInternal::ActorVector::iterator ait;
    for (ait = this->P->TextActors.begin(); ait != this->P->TextActors.end(); ++ait)
    {
      renderedSomething += (*ait)->RenderOpaqueGeometry(viewport);
    }
  }
  else
  {
    if (this->DrawColorBar)
    {
      renderedSomething += this->P->AnnotationBoxesActor->RenderOpaqueGeometry(viewport);
    }
  }

  if (this->DrawNanAnnotation)
  {
    renderedSomething += this->P->NanSwatchActor->RenderOpaqueGeometry(viewport);
  }

  if (this->DrawBelowRangeSwatch)
  {
    renderedSomething += this->P->BelowRangeSwatchActor->RenderOpaqueGeometry(viewport);
  }

  if (this->DrawAboveRangeSwatch)
  {
    renderedSomething += this->P->AboveRangeSwatchActor->RenderOpaqueGeometry(viewport);
  }

  // Draw the annotation leaders and labels
  if (this->DrawAnnotations)
  {
    if (!this->P->AnnotationLabels.empty())
    {
      renderedSomething += this->P->AnnotationLeadersActor->RenderOpaqueGeometry(viewport);
      for (size_t i = 0; i < this->P->AnnotationLabels.size(); ++i)
      {
        renderedSomething += this->P->AnnotationLabels[i]->RenderOpaqueGeometry(viewport);
      }
    }
  }

  renderedSomething = (renderedSomething > 0) ? (1) : (0);

  return renderedSomething;
}

```



#### 阅读编写代码过程中的问题与笔记

- 在哪里设置captiontActor2D的位置？

textActor->SetDisplayPosition(150, 150);，vtkActor2D的方法

- 在哪里设置textActor的位置？

同上

- textActor与哪个坐标系相关联？

textActor里面的GetPositionCoordinate()，是vtkActor2D的方法，是DisPlay坐标系，它的setValue()用于设置像素位置，控制整个actor2D的左下角；

textActor里面的GetPosition2Coordinate()，是vtkActor2D的方法，是normalized DisPlay坐标系，它的setValue()用于设置大小，控制整个actor2D的右上角；



- undefined reference to `vtable for vtkMultiTextActor'是什么错？

有override关键字的函数没有进行实现！！！！

`multiTextActor->SetDisplayPosition(100, 100);`在视图上不起作用，但打印出来是这个信息。

如何根据对vtkActor2D的设置更新到对大actor的起始位置设置？

-与—的区别：

前者是英文字符，后者是中文字符。

前者，字体太大时，多个排布成一行会有间隙；后者不存在：

字体是"C:\\Windows\\Fonts\\simkai.ttf"，字号（FontSize）是20时：

![1684288863675](自定义包含多个vtkTextActor的类\中文字符-FontSize=20.png)

![1684288999239](自定义包含多个vtkTextActor的类\英文字符-FontSize=20.png)

字体是VTK_ARIAL，字号是20时：

![1684289253625](自定义包含多个vtkTextActor的类\中文字符-FontSize=20-arial.png)

![1684289158360](自定义包含多个vtkTextActor的类\英文字符-FontSize=20-arial.png)



渲染自定义actor时的执行顺序：

```C++
gouzaohanshu()
SetCaption()
before add actor
after add actor
before render
RenderOpaqueGeometry()
after render
before renderWindowInteractor
RenderOpaqueGeometry()
after renderWindowInteractor//点击关闭之后
xigouhanshu()
```

多打印ReleaseGraphicsResources()、RenderOverlay()：

```C++
gouzaohanshu()
SetCaption()
SetCaption()
before add actor
after add actor
before render
ReleaseGraphicsResources()
RenderOpaqueGeometry()
RenderOverlay()
after render
before renderWindowInteractor
RenderOpaqueGeometry()
RenderOverlay()
//旋转鼠标。交互
RenderOpaqueGeometry()
RenderOverlay()       
RenderOpaqueGeometry()
RenderOverlay()       
RenderOpaqueGeometry()
RenderOverlay()  
//关闭窗体
ReleaseGraphicsResources()
after renderWindowInteractor
ReleaseGraphicsResources()
ReleaseGraphicsResources()
xigouhanshu()
```

所以更新函数可以写在RenderOpaqueGeometry()或RenderOverlay()。参考vtkCaptionActor2D，写在RenderOpaqueGeometry()中

初步效果：

![1683855971898](自定义包含多个vtkTextActor的类\三个textactor文本四条横线.png)



![1683860536451](自定义包含多个vtkTextActor的类\Courier.png)



### vector控制数量

控制opacity出错：

![1684202379167](自定义包含多个vtkTextActor的类\opacity不能控制.png)

把vtkProperty改成数组:

```C++
void vtkMultiTextActor::SetProperty(vtkProperty2D* p) {
    for (size_t i = 0; i < this->Count; i++) {
        vtkNew<vtkProperty2D> tempProperty2D;
        tempProperty2D->DeepCopy(p);
        this->TextActorVector[i]->SetProperty(tempProperty2D);
    }
}
```

用不用一开始就把成员变量改成数组？

```C++
private:
    //ABC...TextActorVector
    std::vector<vtkTextActor*> TextActorVector; //构造函数默认5个，之后开放接口设置个数
    int Count;
    vtkTextProperty* CaptionTextProperty;
    vtkProperty2D* Property;//?????????????
    int Gap; //各个textActor之间间隔的横向像素距离
```



更改数量时出错：

```C++
void vtkMultiTextActor::ReleaseGraphicsResources(vtkWindow* win) {
    cout << "ReleaseGraphicsResources()" << endl;
    for (size_t i = 0; i < this->Count; i++) {
        this->TextActorVector[i]->ReleaseGraphicsResources(win);
    }
    cout << "Release over" << endl;
}
```

如果之前改过Count，那就循环过多了。

之后考虑不用Set宏，自定义SetCount函数，改了Count之后接着把TextActorVector数量更新过来



