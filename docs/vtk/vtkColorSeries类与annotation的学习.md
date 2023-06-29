目的：设置scalarBar各颜色之间有间隙

### 示例来源

vtk官方代码https://kitware.github.io/vtk-examples/site/Cxx/Visualization/ElevationBandsWithGlyphs/

### vtkColorSeries的效果

使用使用vtkColorSeries构建颜色映射表：

```C++
vtkSmartPointer<vtkLookupTable> GetCategoricalLUT() {
    vtkSmartPointer<vtkColorSeries> colorSeries = GetColorSeries();
    // Make the lookup table.
    vtkNew<vtkLookupTable> lut;
    colorSeries->BuildLookupTable(lut, vtkColorSeries::CATEGORICAL);
    lut->SetNanColor(0, 0, 0, 1);

    return lut;
}
```

```C++
vtkSmartPointer<vtkColorSeries> GetColorSeries() {
    vtkNew<vtkColorSeries> colorSeries;
    // Select a color scheme.
    int colorSeriesEnum;
    // colorSeriesEnum = colorSeries->BREWER_DIVERGING_BROWN_BLUE_GREEN_9;
    // colorSeriesEnum = colorSeries->BREWER_DIVERGING_SPECTRAL_10;
    // colorSeriesEnum = colorSeries->BREWER_DIVERGING_SPECTRAL_3;
    // colorSeriesEnum = colorSeries->BREWER_DIVERGING_PURPLE_ORANGE_9;
    // colorSeriesEnum = colorSeries->BREWER_SEQUENTIAL_BLUE_PURPLE_9;
    // colorSeriesEnum = colorSeries->BREWER_SEQUENTIAL_BLUE_GREEN_9;
    colorSeriesEnum = colorSeries->BREWER_QUALITATIVE_SET3;
    // colorSeriesEnum = colorSeries->CITRUS;
    colorSeries->SetColorScheme(colorSeriesEnum);
    return colorSeries;
}
```



![1682386718823](vtkColorSeries类与annotation的学习\自己实现设置lookupTable的annotation.png)

从上到下的.

### vtkColorSeries类与annotation的学习

#### 官方解释

stores a list of colors.

The [vtkColorSeries](https://vtk.org/doc/nightly/html/classvtkColorSeries.html) stores palettes of colors. There are several default palettes (or schemes) available and functions to control several aspects of what colors are returned. In essence a color scheme is set and then the number of colors and individual color values may be requested.

For a web page showcasing the default palettes, see: [VTKColorSeriesPatches](https://htmlpreview.github.io/?https://github.com/Kitware/vtk-examples/blob/gh-pages/VTKColorSeriesPatches.html); [ColorSeriesPatches](https://kitware.github.io/vtk-examples/site/Python/Visualization/ColorSeriesPatches/) was used to generate this table.

It is also possible to add schemes beyond the default palettes. Whenever *SetColorScheme* is called with a string for which no palette already exists, a new, empty palette is created. You may then use *SetNumberOfColors* and *SetColor* to populate the palette. You may not extend default palettes by calling functions that alter a scheme; if called while a predefined palette is in use, they will create a new non-default scheme and populate it with the current palette before continuing.

The "Brewer" palettes are courtesy of Cynthia A. Brewer (Dept. of Geography, Pennsylvania State University) and under the Apache License. See the source code for details.





#### 测试

使用vtkColorSeries的`  colorSeries->BuildLookupTable(lut, vtkColorSeries::CATEGORICAL);`之后，改变lut参数：

```C++
lut->SetNumberOfTableValues(10);
```

效果：还是不变。

结论：不能指定tableValue的number

### CATEGORICAL与ORDINAL的区别

```C++
  /**
   * An enum defining how lookup tables should be used: either as a
   * list of discrete colors to choose from (categorical), or as an
   * ordered list of color set - points to interpolate among (ordinal).
   */
  enum LUTMode
  {
    /// indexed lookup is off
    ORDINAL = 0,
    /// indexed lookup is on
    CATEGORICAL
  };
```

ORDINAL：序数词，序数的，如firstly，secondly。

CATEGORICAL：明确的，绝对的。expressed clearly and in a way that shows that you are very sure about what you are saying

vtkColorSeries中的LUTMode是一个枚举，它定义了如何使用查找表：作为可以选择的**离散**颜色列表（categorical），或作为一个**有序**的颜色点集，可以在其中进行**插值**（ordinal）。换句话说，LUTMode可以设置查找表是用来表示离散的颜色列表还是表示有序的颜色点集。默认是ORDINAL。

chat:

![1682845366196](vtkColorSeries类与annotation的学习\CATEGORICAL与ORDINAL的区别.png)

对应vtkLookupTable的indexedLookup方法。



#### vtkLookupTable类的官网关于categorical 的介绍

This class behaves differently depending on how *IndexedLookup* is set. When true, [vtkLookupTable](https://vtk.org/doc/nightly/html/classvtkLookupTable.html) enters a mode for representing categorical color maps. By setting *IndexedLookup* to true, you indicate that the annotated values are the only valid values for which entries in the color table should be returned. The colors in the lookup *Table* are assigned to annotated values by taking the modulus of their index in the list of annotations. *IndexedLookup* changes the behavior of *GetIndex*, which in turn changes the way *MapScalarsThroughTable2* behaves; when *IndexedLookup* is true, *MapScalarsThroughTable2* will search for scalar values in *AnnotatedValues* and use the resulting index to determine the color. If a scalar value is not present in *AnnotatedValues*, then *NanColor* will be used.

翻译：

根据IndexedLookup的设置方式，此类的行为有所不同。当为true时，vtkLookupTable将进入表示分类颜色图的模式。通过将IndexedLookup设置为true，可以指示带注释的值是颜色表中应返回的唯一有效值。查找表中的颜色是通过在注释列表中取其索引的模数分配给注释值的。IndexedLookup改变了GetIndex的行为，进而改变了MapScalarsThroughTable2的行为方式；当IndexedLookup为true时，MapScalarsThroughTable2将在AnnotatedValues中搜索标量值，并使用生成的索引来确定颜色。如果AnnotatedValues中不存在标量值，则将使用NanColor。

将示例中的LUTMode由CATEGORICAL改为ORDINAL：

```C++
//CATEGORICAL
vtkSmartPointer<vtkLookupTable> GetCategoricalLUT() {
    vtkSmartPointer<vtkColorSeries> colorSeries = GetColorSeries();
    // Make the lookup table.
    vtkNew<vtkLookupTable> lut;
    colorSeries->BuildLookupTable(lut, vtkColorSeries::CATEGORICAL);
    lut->SetNanColor(0, 0, 0, 1);
    return lut;
}
//ORDINAL
vtkSmartPointer<vtkLookupTable> GetOrdinaLUT() {
    vtkSmartPointer<vtkColorSeries> colorSeries = GetColorSeries();
    // Make the lookup table.
    vtkNew<vtkLookupTable> lut;
    colorSeries->BuildLookupTable(lut, vtkColorSeries::ORDINAL);
    lut->SetNanColor(0, 0, 0, 1);
    return lut;
}
```



![1682843270029](vtkColorSeries类与annotation的学习\originalLut+7annotation+7tableValue.png)



​	两者对比图：

![1682844331487](vtkColorSeries类与annotation的学习\originalLut与Categorical的对比+7annotation+7tableValue.png)

与CATEGORICAL区别：

- scalarBarActor颜色序列与annotation都是从下到上的显示效果。
- scalarBarActor有labelText
- scalarBarActor显示时无间隔
- 模型mapper的`dataSetMapper->SetInterpolateScalarsBeforeMapping(1); `方法有效
- lut的annotation标注在对应的值上，而不是对应的颜色上

改成10，起作用：

![1682843090695](vtkColorSeries类与annotation的学习\originalLut+7annotation+10tableValue.png)

结论：平时默认设置的lut都是ORDINAL

##### 为什么官方示例中的CATEGORICAL模式下仍然有颜色界限？

![1682845973902](vtkColorSeries类与annotation的学习\vtk官方示例CATEGORICAL模式仍有颜色界限.png)

答：因为是用的cellData：

```C++
vtkNew<vtkPolyDataMapper> srcMapper;
srcMapper->SetInputConnection(bcf->GetOutputPort());
srcMapper->SetScalarRange(scalarRange);
srcMapper->SetLookupTable(lut);
srcMapper->SetScalarModeToUseCellData();//cellData
```

自己的模型也使用cellData：

![1683102937629](vtkColorSeries类与annotation的学习\使用cellData.png)



### 实现彩虹色谱scalarBar颜色之间有界限

#### 设置annotation

总共设置7个annotation，10个tableValue。

```C++
vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
lut->SetHueRange(0.67, 0);
lut->SetTableRange(-1, 1); //最大值最小值,与自己使用的算例scalar相对应
lut->SetNumberOfTableValues(10);//10个

// Set the annotations，类似于在固定位置贴标签。总共设置7个
lut->SetAnnotation(vtkVariant(-1), "A=-1");
lut->SetAnnotation(vtkVariant(-0.666), "B=-0.6");
lut->SetAnnotation(vtkVariant(-0.333), "C=-0.3");
lut->SetAnnotation(vtkVariant(0), "D=0");
lut->SetAnnotation(vtkVariant(0.333), "E=0.3");
lut->SetAnnotation(vtkVariant(0.666), "F=0.6");
lut->SetAnnotation(vtkVariant(1), "G=1");
lut->Build();

 dataSetMapper->SetLookupTable(lut);
dataSetMapper->SetInterpolateScalarsBeforeMapping(1); //显示有颜色界限的云图
```

显示效果：

![1683104086183](vtkColorSeries类与annotation的学习\自己实现颜色有界限-设置annotation.png)



#### lutMode改为CATEGORICAL

只添加以下代码：

```C++
lut->IndexedLookupOn();
```

![1683104646852](vtkColorSeries类与annotation的学习\自己实现颜色有界限-indexedLookupOn.png)

只取tableValue的前7个，作为annotation的值



#### 更改TableValue数量

```C++
lut->SetNumberOfTableValues(7);//改成与annotation一致
```

只表现出三种颜色：-1，0，1

![1683104850552](vtkColorSeries类与annotation的学习\自己实现颜色有界限-numoftableValue改成与annotation数量一样（7）.png)

#### 更改annotation的数量

##### case1

与tableValue数量一致，都是10，且分布均匀：

##### case2

与tableValue数量一致，都是10，但分布不均匀：

```C++
 lut->SetAnnotation(vtkVariant(-1), "A=-1");
lut->SetAnnotation(vtkVariant(-0.8), "B=-0.8");
lut->SetAnnotation(vtkVariant(-0.7), "C=-0.7");
lut->SetAnnotation(vtkVariant(-0.6), "D=-0.6");
lut->SetAnnotation(vtkVariant(-0.5), "E=-0.5");
lut->SetAnnotation(vtkVariant(-0.333), "F=-0.3");
lut->SetAnnotation(vtkVariant(0), "G=0");
lut->SetAnnotation(vtkVariant(0.333), "H=0.3");
lut->SetAnnotation(vtkVariant(0.666), "I=0.6");
lut->SetAnnotation(vtkVariant(1), "J=1");
lut->Build();
```

![1683110137256](vtkColorSeries类与annotation的学习\自己实现颜色有界限-更改annotation数量.png)

结论：只表现出三种颜色：-1，0，1对应的三种

##### case3

与tableValue数量一致，都是10，分布均匀：从0.1到1（只有1有对应的颜色）

```C++
// Set the annotations，类似于在固定位置贴标签。
lut->SetAnnotation(vtkVariant(0.1), "A=0.1");
lut->SetAnnotation(vtkVariant(0.2), "B=0.2");
lut->SetAnnotation(vtkVariant(0.3), "C=0.3");
lut->SetAnnotation(vtkVariant(0.4), "D=0.4");
lut->SetAnnotation(vtkVariant(0.5), "E=0.5");
lut->SetAnnotation(vtkVariant(0.6), "F=0.6");
lut->SetAnnotation(vtkVariant(0.7), "G=0.7");
lut->SetAnnotation(vtkVariant(0.8), "H=0.8");
lut->SetAnnotation(vtkVariant(0.9), "I=0.9");
lut->SetAnnotation(vtkVariant(1), "J=1");
lut->Build();
```

![1683110442560](vtkColorSeries类与annotation的学习\自己实现颜色有界限-更改annotation排布方式.png)

结论：只有最上层=1的时候颜色是准的。

##### case4

与tableValue数量一致，都是10，分布不均匀：从0到1（-1没有对应颜色，0与1都有）

```C++
// Set the annotations，类似于在固定位置贴标签。
lut->SetAnnotation(vtkVariant(0), "A=0");
lut->SetAnnotation(vtkVariant(0.2), "B=0.2");
lut->SetAnnotation(vtkVariant(0.3), "C=0.3");
lut->SetAnnotation(vtkVariant(0.4), "D=0.4");
lut->SetAnnotation(vtkVariant(0.5), "E=0.5");
lut->SetAnnotation(vtkVariant(0.6), "F=0.6");
lut->SetAnnotation(vtkVariant(0.7), "G=0.7");
lut->SetAnnotation(vtkVariant(0.8), "H=0.8");
lut->SetAnnotation(vtkVariant(0.9), "I=0.9");
lut->SetAnnotation(vtkVariant(1), "J=1");
lut->Build();
```

![1683115389028](vtkColorSeries类与annotation的学习\自己实现颜色有界限-更改annotation排布方式2.png)

结论：只有中间=0、最上层=1的时候颜色是准的。

#### 结论

- lutMOde分为CATEGORICAL与ORDINAL，分别表示独立的和连续的颜色表，前者不进行插值。
- lutMode的CATEGORICAL模式，scalarbar显示时从上到下，如果从下往上，需要自定义反转函数。
- lutMode的CATEGORICAL模式下没有labelText
- lutMode改为CATEGORICAL后，有几种颜色能对的上就显示几种，各颜色之间不进行插值、彼此独立。而等值线图之间的颜色也不需要进行插值，因此lutMode改为CATEGORICAL适合等值线图的标注。

不足之处：annatation与color的排列从上到下。

### 补充：annotation与相关方法

#### vtkScalarsToColors类的方法

vtkScalarsToColors类是vtkLookupTable的父类，在官网中的描述：

Superclass for mapping scalar values to colors.

[vtkScalarsToColors](https://vtk.org/doc/nightly/html/classvtkScalarsToColors.html) is a general-purpose base class for objects that convert scalars to colors. This include [vtkLookupTable](https://vtk.org/doc/nightly/html/classvtkLookupTable.html) classes and color transfer functions. By itself, this class will simply rescale the scalars.

The scalar-to-color mapping can be augmented with an additional uniform alpha blend. This is used, for example, to blend a [vtkActor](https://vtk.org/doc/nightly/html/classvtkActor.html)'s opacity with the lookup table values.

Specific scalar values may be annotated with text strings that will be included in color legends using *SetAnnotations*, *SetAnnotation*, *GetNumberOfAnnotatedValues*, *GetAnnotatedValue*, *GetAnnotation*, *RemoveAnnotation*, and *ResetAnnotations*.

This class also has a method for indicating that the set of annotated values form a categorical color map; by setting *IndexedLookup* to true, you indicate that the annotated values are the only valid values for which entries in the color table should be returned. In this mode, subclasses should then assign colors to annotated values by taking the modulus of an annotated value's index in the list of annotations with the number of colors in the table.

翻译：

`vtkScalarsToColors` 是一个用于将标量映射到颜色的超类，是一个通用的基类，用于将标量转换为颜色的对象。这包括 vtkLookupTable 类和颜色传输函数。本身，这个类只会重新缩放标量。

标量到颜色的映射可以增加额外的统一 alpha 混合。例如，这用于将 vtkActor 的不透明度与查找表值混合。

**可以使用 SetAnnotations、SetAnnotation、GetNumberOfAnnotatedValues、GetAnnotatedValue、GetAnnotation、RemoveAnnotation 和 ResetAnnotations 方法来为特定的标量值添加文本字符串注释，这些字符串将包含在颜色图例中。**

此类还有一种方法：设置annotation值以形成分类颜色映射；通过将 IndexedLookup 设置为 true，您指示注释值是颜色表中应返回条目的唯一有效值。在此模式下，子类应通过取注释值在注释列表中的索引与表中颜色数目的模数来为annotation值分配颜色。

一些方法：

删

```C++
 /**
   * Remove an existing entry from the list of annotated values.

   * Returns true when the entry was actually removed (i.e., it existed before the call).
   * Otherwise, returns false.
   */
  virtual bool RemoveAnnotation(vtkVariant value);
```

删

```C++
  /**
   * Remove all existing values and their annotations.
   */
  virtual void ResetAnnotations();
```

查

```C++
 /**
   * Get the "indexed color" assigned to an index.

   * The index is used in \a IndexedLookup mode to assign colors to annotations (in the order
   * the annotations were set).
   * Subclasses must implement this and interpret how to treat the index.
   * vtkLookupTable simply returns GetTableValue(\a index % \a this->GetNumberOfTableValues()).
   * vtkColorTransferFunction returns the color associated with node \a index % \a this->GetSize().

   * Note that implementations *must* set the opacity (alpha) component of the color, even if they
   * do not provide opacity values in their colormaps. In that case, alpha = 1 should be used.
   */
  virtual void GetIndexedColor(vtkIdType i, double rgba[4]);
```

查

```C++
  /**
   * Allocate annotation arrays if needed, then return the index of
   * the given \a value or -1 if not present.
   */
  virtual vtkIdType CheckForAnnotatedValue(vtkVariant value);
```

取

```C++
 /**
   * Return the index of the given value in the list of annotated values (or -1 if not present).
   */
  vtkIdType GetAnnotatedValueIndex(vtkVariant val);
```

取

```C++
  /**
   * Return the annotated value at a particular index in the list of annotations.
   */
  vtkIdType GetNumberOfAnnotatedValues();

  /**
   * Return the annotated value at a particular index in the list of annotations.
   */
  vtkVariant GetAnnotatedValue(vtkIdType idx);

  /**
   * Return the annotation at a particular index in the list of annotations.
   */
  vtkStdString GetAnnotation(vtkIdType idx);

  /**
   * Obtain the color associated with a particular annotated value (or NanColor if unmatched).
   */
  virtual void GetAnnotationColor(const vtkVariant& val, double rgba[4]);
```



### 总结

完成了设置scalarBar各颜色之间有间隙

讨论了annotation与tableValue的关系

### 下一步计划

研究如何反转scalarbar的annotation？

```C++
vtkSmartPointer<vtkLookupTable> ReverseLUT(vtkLookupTable *lut) {
    // First do a deep copy just to get the whole structure
    // and then reverse the colors and annotations.
    vtkNew<vtkLookupTable> lutr;
    lutr->DeepCopy(lut);
    vtkIdType t = lut->GetNumberOfTableValues() - 1;
    for (vtkIdType i = t; i >= 0; --i) {
        std::array<double, 3> rgb{0.0, 0.0, 0.0};
        std::array<double, 4> rgba{0.0, 0.0, 0.0, 1.0};
        lut->GetColor(i, rgb.data());
        std::copy(std::begin(rgb), std::end(rgb), std::begin(rgba));
        rgba[3] = lut->GetOpacity(i);
        lutr->SetTableValue(t - i, rgba.data());
    }
    t = lut->GetNumberOfAnnotatedValues() - 1;
    for (vtkIdType i = t; i >= 0; --i) {
        lutr->SetAnnotation(t - i, lut->GetAnnotation(i));
    }

    return lutr;
}
```

### 参考文献

[VTK: vtkLookupTable Class Reference](https://vtk.org/doc/nightly/html/classvtkLookupTable.html#details)

[VTK: vtkScalarsToColors Class Reference](https://vtk.org/doc/nightly/html/classvtkScalarsToColors.html)

https://kitware.github.io/vtk-examples/site/Cxx/Visualization/ElevationBandsWithGlyphs/