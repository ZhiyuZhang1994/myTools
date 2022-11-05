/**
 * @brief 组装数据源:将箭头与锥体组装表示力矩
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2022-11-5
 */

#include "vtkAppendPolyData.h"
#include "vtkConeSource.h"
#include "vtkFiltersSourcesModule.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkTransformFilter.h"
#include <vtkArrowSource.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>

class vtkArrowWithCone : public vtkPolyDataAlgorithm {
public:
    static vtkArrowWithCone* New();

    vtkTypeMacro(vtkArrowWithCone, vtkPolyDataAlgorithm);

    void PrintSelf(ostream& os, vtkIndent indent) override;

    vtkBooleanMacro(Invert, bool);

    vtkSetMacro(Invert, bool);

    vtkGetMacro(Invert, bool);

protected:
    vtkArrowWithCone();

    ~vtkArrowWithCone() override = default;

    int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

protected:
    vtkSmartPointer<vtkArrowSource> arrow = nullptr;
    vtkSmartPointer<vtkConeSource> cone = nullptr;
    int Resolution;
    double Length;
    double Radius;
    bool Invert;

private:
    vtkArrowWithCone(const vtkArrowWithCone&) = delete;
    void operator=(const vtkArrowWithCone&) = delete;
};

vtkStandardNewMacro(vtkArrowWithCone);

vtkArrowWithCone::vtkArrowWithCone() {
    arrow = vtkSmartPointer<vtkArrowSource>::New();
    cone = vtkSmartPointer<vtkConeSource>::New();
    this->Invert = false;
    this->Resolution = 6;
    this->Radius = 0.1;
    this->Length = 0.35; // 锥体初始长度
    this->SetNumberOfInputPorts(0);
}

int vtkArrowWithCone::RequestData(vtkInformation* vtkNotUsed(request), vtkInformationVector** vtkNotUsed(inputVector),
                                  vtkInformationVector* outputVector) {
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    vtkPolyData* output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    cone->SetRadius(Radius);
    cone->SetResolution(Resolution);
    cone->SetHeight(Length);
    arrow->Update(); // 更新箭头(必须)

    // 移动锥体位置到箭头上方
    vtkTransform* transCone = vtkTransform::New();
    transCone->Translate(1 + Length / 2, 0, 0);
    vtkTransformFilter* tfCone = vtkTransformFilter::New();
    tfCone->SetTransform(transCone);
    tfCone->SetInputConnection(cone->GetOutputPort());

    // 组装锥体与箭头形成一个图形
    vtkAppendPolyData* append = vtkAppendPolyData::New();
    append->AddInputData(arrow->GetOutput());
    append->AddInputConnection(tfCone->GetOutputPort());
    append->Update();

    // 将整个图形缩放到单位长度
    vtkTransform* transWhole = vtkTransform::New();
    vtkTransformFilter* transFilterWhole = vtkTransformFilter::New();
    transWhole->Scale(1 / (1 + Length), 1, 1);
    transFilterWhole->SetTransform(transWhole);
    transFilterWhole->SetInputConnection(append->GetOutputPort());
    transFilterWhole->Update();

    // 反转时用：将图形移动到单位方向后，沿X轴翻转则达到图形反转效果
    vtkTransform* transInvert = vtkTransform::New();
    vtkTransformFilter* transFilterInvert = vtkTransformFilter::New();
    transInvert->Translate(1, 0.0, 0.0);
    transInvert->Scale(-1, 1, 1);
    transFilterInvert->SetTransform(transInvert);

    if (this->Invert) {
        transFilterInvert->SetInputConnection(transFilterWhole->GetOutputPort());
        transFilterInvert->Update();
        output->ShallowCopy(transFilterInvert->GetOutput());
    } else {
        output->ShallowCopy(transFilterWhole->GetOutput());
    }
    return 1;
}

void vtkArrowWithCone::PrintSelf(ostream& os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);
    os << indent << "Resolution: " << this->Resolution << "\n";
    os << indent << "Radius: " << this->Radius << "\n";
    os << indent << "Length: " << this->Length << "\n";
    os << indent << "Invert: " << this->Invert << "\n";
}

int main()
{
    // 创建箭头带文本对象
    vtkSmartPointer<vtkArrowWithCone> arrowWithCone = vtkSmartPointer<vtkArrowWithCone>::New();
    arrowWithCone->Update(); // 更新对象

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(arrowWithCone->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkAxesActor> axes;

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetWindowName("vtkArrowWithCone");
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(300, 300);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    vtkNew<vtkNamedColors> colors;
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    // 通用显示部分
    renderer->AddActor(actor);
    renderer->AddActor(axes);
    renderer->ResetCamera();
    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}
