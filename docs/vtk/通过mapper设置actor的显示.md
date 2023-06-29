# 通过mapper设置actor的显示

### 综述

The vtkMapper class has static methods for setting application-wide offsets for polygons, lines and points. ( SetResolveCoincidentTopologyPolygonOffsetParameters, etc.) The vtkMapper classes also have methods for setting actor-specific offsets for polygons, lines and points ( SetRelativeCoincidentTopologyPolygonOffsetParameters, etc.). The net offset is the sum of both, for the type of primitive. The offsets are specified by factor and unit, but the value for factor is not used anymore. Positive values shift the primitive away from the camera, negative values shift the primitive to towards the camera.

The defaults for the static offset-units are 0, -4, -8 for polygons, lines and points respectively, meaning that polygons are not shifted, lines are brought forward by some amount and points are brought forward by double that amount. The relative offset-values are all 0 by default.

If you have two actors that render overlaying polygons you can specify a negative relative polygon-offset unit-value for the actor that you want to appear in front, or a positive value for the one that should be pushed back, or a bit of both. You’ll have to experiment a bit with which values are most appropriate for your situation.

翻译：

vtkMapper类具有用于设置多边形、直线和点的应用程序范围偏移的静态方法。（SetResolveConflicentTopologyPolygonOffsetParameters等）vtkMapper类还具有为多边形、直线和点设置特定于演员的偏移的方法（SetRelativeCoincidentTopologyPolygon OffsetParameters，等）。对于基元类型，净偏移是这两者的总和。偏移由因子和单位指定，但不再使用因子的值。正值将基本体从摄影机移开，负值将基本体移向摄影机。

多边形、直线和点的静态偏移单位的默认值分别为0、-4、-8，这意味着多边形不会移动，直线向前移动一定量，点向前移动两倍。默认情况下，相对偏移值均为0。

如果有两个渲染重叠多边形的角色，可以为要显示在前面的角色指定一个负的相对多边形偏移单位值，或为应该向后推的角色指定正值，或两者兼而有之。您必须尝试一下哪些值最适合您的情况。

### 三种设置

```C++
#define VTK_RESOLVE_OFF 0
#define VTK_RESOLVE_POLYGON_OFFSET 1
#define VTK_RESOLVE_SHIFT_ZBUFFER 2
```

#### 官方原文说明

Set/Get a global flag that controls whether **coincident topology** (e.g., a line on top of a polygon) is shifted to avoid z-buffer resolution (and hence rendering problems).   If **not off**, there are two methods to choose from.   **PolygonOffset** uses graphics systems calls to shift polygons, lines and points from each other.   **ShiftZBuffer** is a legacy method that used to remap the z-buffer to distinguish vertices, lines, and polygons, but does not always produce acceptable results.   You should only use the PolygonOffset method (or none) at this point

#### 翻译

设置/获取一个全局标志，用于控制是否移动 **重合拓扑**（例如，多边形顶部的线）以避免 z 缓冲区分辨率（从而避免渲染问题）。  如果未关闭，则有两种方法可供选择。  多边形偏移量使用图形系统调用来相互移动多边形、线和点。  ShiftZBuffer 是一种传统方法，用于重新映射 z 缓冲区以区分顶点、线和多边形，但并不总是产生可接受的结果。  此时应仅使用 PolygonOffset 方法（或不使用）

#### 结论

想要避免Z-buffer(深度缓冲)分辨率引起的渲染问题，有两种方法：PolygonOffset与ShiftZBuffer。官方推荐前者。

### 如何使用PolygonOffset

#### 面的设置

定义：

```C++
void vtkMapper::SetRelativeCoincidentTopologyPolygonOffsetParameters(double factor, double units)
{
  if (factor == this->CoincidentPolygonFactor && units == this->CoincidentPolygonOffset)
  {
    return;
  }
  this->CoincidentPolygonFactor = factor;
  this->CoincidentPolygonOffset = units;
  this->Modified();
}
```

调用：

```C++
const double units0 = -200; //偏移量
mapper1->SetRelativeCoincidentTopologyPolygonOffsetParameters(2000, units0);
```



 SetRelativeCoincidentTopologyPolygonOffsetParameters函数是VTK图形函数用来解决视觉叠加图元抢占问题的。调用SetRelativeCoincidentTopologyPolygonOffsetParameters函数可以设定某个特定的形状将与重合的另一形状的距离。例如，一个三角形和一个正方形重叠在一起时，可以调用SetRelativeCoincidentTopologyPolygonOffsetParameters函数来设定三角形的距离，以便它不会被正方形“抢占”。

该函数的参数factor和units，是用来设置叠加图元之间的间隙距离。factor参数是用来设置偏移量（在vtk8.1之后，该参数已弃用，参考[update and improve the coincident rendering code (3052137e) · Commits · VTK / VTK · GitLab (kitware.com)](https://gitlab.kitware.com/vtk/vtk/-/commit/3052137e6621684f9184b293b82c677e976c2f89)），units参数是用来设置偏移量的单位。  

如果有两个渲染重叠多边形的角色，可以为要显示在前面的角色指定一个负的相对多边形偏移单位值（units），或为应该向后推的角色指定正值，或两者兼而有之。  

使用：actor表现为面时，该方法有效。

#### 线的设置

 ```C++
void vtkMapper::SetRelativeCoincidentTopologyLineOffsetParameters(double factor, double units)
{
  if (factor == this->CoincidentLineFactor && units == this->CoincidentLineOffset)
  {
    return;
  }
  this->CoincidentLineFactor = factor;
  this->CoincidentLineOffset = units;
  this->Modified();
}
 ```

调用：

```C++
mapper1->SetRelativeCoincidentTopologyLineOffsetParameters(2000, units0);
```

 SetRelativeCoincidentTopologyLineOffsetParameters函数，用于设置VtkMapper的线的相对重合拓扑偏移量，主要用于处理同一平面上偏移线条重叠情况。

该函数的参数factor和units，是用来设置叠加图元之间的间隙距离。factor参数是用来设置偏移量（在vtk8.1之后，该参数已弃用，参考[update and improve the coincident rendering code (3052137e) · Commits · VTK / VTK · GitLab (kitware.com)](https://gitlab.kitware.com/vtk/vtk/-/commit/3052137e6621684f9184b293b82c677e976c2f89)），units参数是用来设置偏移量的单位。  

如果有两个渲染重叠多边形的角色，可以为要显示在前面的角色指定一个负的相对多边形偏移单位值（units），或为应该向后推的角色指定正值，或两者兼而有之。

使用：actor表现为线`SetRepresentationToWireframe()`时，该方法有效。

#### 点的设置

定义：

```C++
void vtkMapper::SetRelativeCoincidentTopologyPointOffsetParameter(double units)
{
  if (units == this->CoincidentPointOffset)
  {
    return;
  }
  this->CoincidentPointOffset = units;
  this->Modified();
}
```



调用：

```C++
mapper2->SetRelativeCoincidentTopologyPointOffsetParameter(units0);

```

SetRelativeCoincidentTopologyPointOffsetParameter函数是vtkMapper中的函数，用来设置相对重合的拓扑点偏移量的参数。

该函数的参数units，是用来设置叠加图元之间的间隙距离。

如果有两个渲染重叠多边形的角色，可以为要显示在前面的角色指定一个负的相对多边形偏移单位值（units），或为应该向后推的角色指定正值，或两者兼而有之。

使用：actor表现为点`actor1->GetProperty()->SetRepresentationToPoints()`时，该方法有效。

#### 测试

测试算例：两个mapper:mapper1与mapper2,对应两个actor:actor1，actor2;

##### 只对1个mapper设置参数

- units（偏移量）取正值，效果是将对应的actor后移；
- units取0，效果不变；
- units取负数时，效果是将对应的actor前移。
  - 取绝对值很大的负数（>100）时，很稳定的实现指定actor置顶的效果；
  - actor表现为面时，取负的0.1/1时，刚渲染出来，会将对应actor置顶，一旦稍微挪动camera，就恢复之前的效果
  - actor表现为面时，取-20时，随着相机角度不同，有渐变效果。
  - actor表现为线/点时，取很小的负数，效果也很稳定
- factor的取值范围（无论正负、大小，设置之后都能成功打印出来）与影响，暂时未看出来。

##### 对2个mapper设置参数

两个mapper对应的actor都表现为面，**units都取-200，测试factor的影响**；

先加入actor1，再加入actor2；

- factor取值相同（都取0/0.2/-1）时，显示为actor2
- mapper1取0.2，mapper2取0.1，显示为actor2；
- mapper1取0.1，mapper2取0.2，显示为actor2；
- mapper1取0.5，mapper2取-0.5，显示为actor2;
- mapper1取-0.5，mapper2取0.5，显示为actor2;
- mapper1取-0.1，mapper2取-0.2，显示为actor2；
- mapper1取-0.2，mapper2取-0.1，显示为actor2；

mapper1的units取-201，mapper2的取-200：当两者的factor取值相同（-1，-0.5，0，0.5，1）时，都是稳定的actor1在前面

#### 结论

- 在mapper中，units代表偏移量，取值越小，对应的actor越靠前；

- vtk9.1版本中，factor对actor的显示没有影响，可直接设置为0；

- 经测试，要置顶的actor如果包含面的话，对应的mapper对应的偏移量最好绝对值>=100；线与点不作要求

### 参考资料

1. actor置顶的方法介绍 https://blog.csdn.net/weixin_41838721/article/details/125285597
2. 通过mapper置顶actor(https://blog.csdn.net/wanyongtai/article/details/106519551)
3. 通过renderer设置图层置顶actor(https://blog.csdn.net/lpsl1882/article/details/52107117)
4. vtkMapper：https://vtk.org/doc/nightly/html/classvtkMapper.html
5. 关于定义CoincidentPolygonFactor：https://vtk.org/Wiki/VTK/PolygonOffsetting
6. 从vtk8.1开始，不再使用factor,只使用unit(https://gitlab.kitware.com/vtk/vtk/-/commit/3052137e6621684f9184b293b82c677e976c2f89)
7. How to correctly use the coincidenttopology related methods on a mapper ? - Web - VTK(https://discourse.vtk.org/t/how-to-correctly-use-the-coincidenttopology-related-methods-on-a-mapper/6888)
8. VTK官方文档：https://www.vtk.org/doc/ 