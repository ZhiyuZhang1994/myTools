/**
 * @brief 自定义VTK类，包含多个vtkTextActor对象持有相同内容，一行排列
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2023-05-22
 */

#ifndef GRAPHIC_VTK_WIDGET_VTK_MULTI_TEXT_ACTOR_VECTOR_H
#define GRAPHIC_VTK_WIDGET_VTK_MULTI_TEXT_ACTOR_VECTOR_H

#include <vtkActor2D.h>
#include <vtkProperty2D.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkViewport.h>

class vtkMultiTextActor : public vtkActor2D {
public:
    vtkTypeMacro(vtkMultiTextActor, vtkActor2D);
    static vtkMultiTextActor* New();
    void PrintSelf(ostream& os, vtkIndent indent) override;

    /**
     * Define the text to be placed in the caption. The text can be multiple
     * lines (separated by "\n").
     */
    virtual void SetCaption(const char* caption);
    virtual char* GetCaption();

    /**
     * Set/Get the gap between each textActor.
     */
    vtkSetClampMacro(Gap, int, 0, 300);
    vtkGetMacro(Gap, int);

    /**
     * Set/Get the Count of textActor.
     */
    vtkSetClampMacro(Count, int, 1, 100);
    vtkGetMacro(Count, int);

    /**
     * 将Gap设置成caption文本像素值，使得多个textActor无缝连接
     */
    virtual void SetGapToFit(vtkViewport* vport);

    /**
     * Set/Get the text property.
     */
    virtual void SetCaptionTextProperty(vtkTextProperty* p);
    vtkGetObjectMacro(CaptionTextProperty, vtkTextProperty);

    /**
     * Set/Get the property.
     */
    virtual void SetProperty(vtkProperty2D* p);
    vtkGetObjectMacro(Property, vtkProperty2D);

    /**
     * Shallow copy of this scaled text actor. Overloads the virtual
     * vtkProp method.
     */
    void ShallowCopy(vtkProp* prop) override;

    /**
     * WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
     * DO NOT USE THIS METHOD OUTSIDE OF THE RENDERING PROCESS.
     * Release any graphics resources that are being consumed by this actor.
     * The parameter window could be used to determine which graphic
     * resources to release.
     */
    void ReleaseGraphicsResources(vtkWindow*) override;

    /**
     * WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
     * DO NOT USE THIS METHOD OUTSIDE OF THE RENDERING PROCESS.
     * Draw the text actor vector to the screen.
     * RenderOpaqueGeometry()函数，渲染不透明几何体，最主要，将更新配置写在其中
     * RenderTranslucentPolygonalGeometry()函数，渲染半透明多边形几何体
     * RenderOverlay()函数，渲染层叠
     */
    int RenderOpaqueGeometry(vtkViewport* viewport) override;
    int RenderTranslucentPolygonalGeometry(vtkViewport*) override { return 0; };
    int RenderOverlay(vtkViewport* viewport) override;

    /**
     * Does this prop have some translucent polygonal geometry?  答案：无
     */
    vtkTypeBool HasTranslucentPolygonalGeometry() override { return 0; };

protected:
    vtkMultiTextActor();
    ~vtkMultiTextActor() override;

private:
    std::vector<vtkTextActor*> TextActorVector; // 放textActor的容器
    int Count;                                  // TextActorVector中 textActor的数量

    vtkTextProperty* CaptionTextProperty;       // 各个textActor的文本属性
    vtkProperty2D* Property;                    // 各个textActor的属性
    int Gap;                                    // 各个textActor之间间隔的横向像素距离
    const char* CaptionText;                    // 文本内容

private:
    /**
     * 将Gap设置到各个Actor，用于RenderOpaqueGeometry()，用户不需要看到，只需要Set/Get Gap值
     */
    virtual void SetGapToCaption();

    /**
     * 将Cout更新到存放TextActorVector的Vector
     */
    virtual void SetCountIntoTextActorVector();

    /**
     * 将数量为Count的TextActor分布Opacity.若Count=3，那Opacity分别为: 1/3,2/3,3/3
     */
    virtual void SetOpacityIntoTextActorVector();

private:
    vtkMultiTextActor(const vtkMultiTextActor&) = delete;
    void operator=(const vtkMultiTextActor&) = delete;
};

#endif // GRAPHIC_VTK_WIDGET_VTK_MULTI_TEXT_ACTOR_VECTOR_H