/**
 * @brief 自定义VTK类，包含多个vtkTextActor对象持有相同内容，一行排列
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2023-05-22
 */

#include "vtkMultiTextActorVector.h"
#include <vtkObjectFactory.h>
#include <vtkProperty2D.h>
#include <vtkNew.h>

vtkStandardNewMacro(vtkMultiTextActor);

vtkMultiTextActor::vtkMultiTextActor() {
    // 设置坐标属性，继承自vtkActor2D
    this->PositionCoordinate->SetCoordinateSystemToDisplay();

    // 初始化VtkProperty2D
    this->Property = vtkProperty2D::New();

    // 初始化textProperty
    this->CaptionTextProperty = vtkTextProperty::New();
    this->CaptionTextProperty->SetBold(0);   // 不加粗
    this->CaptionTextProperty->SetItalic(0); // 不斜体
    this->CaptionTextProperty->SetShadow(0); // 无阴影
    this->CaptionTextProperty->SetFontFamily(VTK_ARIAL);
    this->CaptionTextProperty->SetFontSize(10);
    this->CaptionTextProperty->SetJustification(VTK_TEXT_LEFT);           // 左对齐
    this->CaptionTextProperty->SetVerticalJustification(VTK_TEXT_BOTTOM); // 底部对齐

    // 设置vector的Count=5
    this->Count = 5;
    // 根据Count初始化TextActorVector
    this->SetCountIntoTextActorVector();
    // CaptionText
    this->CaptionText = "—"; // 中文字符
    // 将CaptionTextProperty设置到TextActorVector的每个Actor
    this->SetCaptionTextProperty(this->CaptionTextProperty);

    // 将Property设置到TextActorVector的每个Actor
    this->SetProperty(this->Property);
    // 逐个设置不透明度
    this->SetOpacityIntoTextActorVector();

    this->SetCaption(this->CaptionText);
    this->Gap = 70; // 默认先设置为70像素，创建renderer之后可以通过SetGapToFit()来调节。
    this->SetGapToCaption();
}
// 析构函数
vtkMultiTextActor::~vtkMultiTextActor() {
    // 忽略bool宏，其它都delete，新增的类还要=nullptr

    for (int i = 0; i < this->Count; i++) {
        this->TextActorVector[i]->Delete();
    }
    this->TextActorVector.clear();
    this->Property->Delete();
    this->CaptionTextProperty->Delete();
}

void vtkMultiTextActor::SetCaptionTextProperty(vtkTextProperty* p) {
    this->CaptionTextProperty = p; // 更新内部变量的值
    for (int i = 0; i < this->Count; i++) {
        this->TextActorVector[i]->SetTextProperty(p);
    }
}

void vtkMultiTextActor::SetProperty(vtkProperty2D* p) {
    this->Property = p; // 更新内部变量的值
    for (int i = 0; i < this->Count; i++) {
        vtkNew<vtkProperty2D> tempProperty2D;
        tempProperty2D->DeepCopy(p);
        this->TextActorVector[i]->SetProperty(tempProperty2D);
    }
}

// 设置文本，几个actor的文本都一样
void vtkMultiTextActor::SetCaption(const char* caption) {
    // 附给CaptionText
    this->CaptionText = caption;
    for (int i = 0; i < this->Count; i++) {
        this->TextActorVector[i]->SetInput(caption);
    }
}

char* vtkMultiTextActor::GetCaption() {
    // 获取文本，几个actor的文本都一样
    return this->TextActorVector[0]->GetInput();
}

void vtkMultiTextActor::SetGapToCaption() {
    // A与actor2D的位置相同，B右移1个Gap，...,以此类推
    for (int i = 0; i < this->Count; i++) {
        this->TextActorVector[i]->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
        this->TextActorVector[i]->GetPositionCoordinate()->SetReferenceCoordinate(this->PositionCoordinate);
        this->TextActorVector[i]->GetPositionCoordinate()->SetValue(i * this->Gap, 0, 0);
    }
}

void vtkMultiTextActor::SetCountIntoTextActorVector() {
    // 如果新指定的Count与原本的一致，直接return
    if (static_cast<int>(this->TextActorVector.size()) == this->Count) {
        return;
    }

    // 不一致时，先清空之前的，再重新添加Count个.
    for (auto each : this->TextActorVector) {
        each->Delete();
    }
    this->TextActorVector.clear();

    for (int i = 0; i < this->Count; i++) {
        vtkTextActor* tempTextActor = vtkTextActor::New();
        this->TextActorVector.push_back(tempTextActor);
    }
}

void vtkMultiTextActor::SetOpacityIntoTextActorVector() {
    // 各个textActor之间的不透明度差距
    double OpacityGap = (double)1 / this->Count;
    // 各个TextActor的不透明度，初始为1，之后逐个递减OpacityGap
    double tempOpacity = 1;
    for (int i = 0; i < this->Count; i++) {
        this->TextActorVector[i]->GetProperty()->SetOpacity(tempOpacity);
        tempOpacity -= OpacityGap;
    }
}

void vtkMultiTextActor::SetGapToFit(vtkViewport* vport) {
    double size[2] = {0, 0};
    this->TextActorVector[0]->GetSize(vport, size);
    // size[0]代表像素宽度
    this->Gap = size[0];
}

//  Release any graphics resources that are being consumed by this actor.
//  The parameter window could be used to determine which graphic
//  resources to release.
void vtkMultiTextActor::ReleaseGraphicsResources(vtkWindow* win) {
    for (auto each : this->TextActorVector) {
        each->ReleaseGraphicsResources(win);
    }
}

// 渲染不透明的几何体
int vtkMultiTextActor::RenderOpaqueGeometry(vtkViewport* viewport) {
    // 根据Count初始化TextActorVector
    this->SetCountIntoTextActorVector();
    this->SetCaption(this->CaptionText);

    // 更新TextProperty(),在构造函数中设置过一次了，这里再来更新一次
    this->SetCaptionTextProperty(this->CaptionTextProperty);

    // 更新Property
    this->SetProperty(this->Property);
    // 逐个设置不透明度，该方法要在SetProperty()之后调用，否则会被覆盖掉
    this->SetOpacityIntoTextActorVector();

    // 将Gap值设置到各TextActorVector中
    this->SetGapToCaption();

    // 开始更新renderedSomething，将显示的子Actor的RenderOpaqueGeometry()值加起来
    int renderedSomething = 0;
    for (int i = 0; i < this->Count; i++) {
        renderedSomething += this->TextActorVector[i]->RenderOpaqueGeometry(viewport);
    }

    return renderedSomething;
}

// 绘制对象的叠加层几何体
int vtkMultiTextActor::RenderOverlay(vtkViewport* viewport) {
    // 将显示的子Actor的RenderOverlay()值加起来
    int renderedSomething = 0;
    for (int i = 0; i < this->Count; i++) {
        renderedSomething += this->TextActorVector[i]->RenderOverlay(viewport);
    }
    return renderedSomething;
}

//------------------------------------------------------------------------------
void vtkMultiTextActor::PrintSelf(ostream& os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);
}
//------------------------------------------------------------------------------
void vtkMultiTextActor::ShallowCopy(vtkProp* prop) {
    vtkMultiTextActor* a = vtkMultiTextActor::SafeDownCast(prop);
    if (a != nullptr) {
        this->SetCaption(a->GetCaption());
    }

    // Now do superclass
    this->vtkActor2D::ShallowCopy(prop);
}
