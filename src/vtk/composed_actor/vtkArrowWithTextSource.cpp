/**
 * @brief 组装数据源:将箭头与文本数据组装
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2022-11-3
 */

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkArrowSource.h>
#include <vtkVectorText.h>

#include "vtkFiltersSourcesModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"
#include "vtkAppendPolyData.h"
#include "vtkTransformFilter.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include <vtkAxesActor.h>

class vtkArrowWithTextSource : public vtkPolyDataAlgorithm {
public:
    static vtkArrowWithTextSource* New();

    vtkTypeMacro(vtkArrowWithTextSource, vtkPolyDataAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent) override;

    void SetText(const char* _arg) {
        vecText->SetText(_arg);
    }
    virtual char* GetText() {
        return vecText->GetText();
    }

protected:
    vtkArrowWithTextSource();
    ~vtkArrowWithTextSource() override = default;

    int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

protected:
    vtkSmartPointer<vtkArrowSource> arrow = nullptr;
    vtkSmartPointer<vtkVectorText> vecText = nullptr;

private:
    vtkArrowWithTextSource(const vtkArrowWithTextSource&) = delete;
    void operator=(const vtkArrowWithTextSource&) = delete;
};

vtkStandardNewMacro(vtkArrowWithTextSource);

//------------------------------------------------------------------------------
vtkArrowWithTextSource::vtkArrowWithTextSource() {
    arrow = vtkSmartPointer<vtkArrowSource>::New();
    vecText = vtkSmartPointer<vtkVectorText>::New();
    this->SetNumberOfInputPorts(0);
}

//------------------------------------------------------------------------------
int vtkArrowWithTextSource::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* outputVector) {
    // 拿到输出管线信息对象
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    // 拿到输出数据信息对象
    vtkPolyData* output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    arrow->SetInvert(true); // 箭头翻转，指向远点
    arrow->Update(); // 更新箭头
    vecText->Update(); // 更新文本

    vtkTransform* trans = vtkTransform::New(); // 变换文本位置
    trans->Translate(0,-0.2,0);
    trans->Scale(0.1,0.1,0.1);
    vtkTransformFilter* tf = vtkTransformFilter::New();
    tf->SetTransform(trans);
    tf->SetInputConnection(vecText->GetOutputPort());

    // 添加箭头与文本为一体
    vtkAppendPolyData* append = vtkAppendPolyData::New();
    append->AddInputData(arrow->GetOutput());
    append->AddInputConnection(tf->GetOutputPort());

    // 整体对象做位置变换：当前无变换
    vtkTransform* trans3 = vtkTransform::New();
    vtkTransformFilter* tf3 = vtkTransformFilter::New();
    trans3->Translate(0.0, 0.0, 0.0);
    tf3->SetTransform(trans3);
    tf3->SetInputConnection(append->GetOutputPort());
    tf3->Update();

    // 给输出赋值
    output->ShallowCopy(tf3->GetOutput());
    return 1;
}

//------------------------------------------------------------------------------
void vtkArrowWithTextSource::PrintSelf(ostream& os, vtkIndent indent) {
  this->Superclass::PrintSelf(os, indent);
}

int main() {
    // 创建箭头带文本对象
    vtkSmartPointer<vtkArrowWithTextSource> arrowWithText = vtkSmartPointer<vtkArrowWithTextSource>::New();
    arrowWithText->SetText("Welcome to\r\n my blog!"); // 设置文本内容
    arrowWithText->Update(); // 更新对象

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(arrowWithText->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkAxesActor> axes;

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetWindowName("vtkArrowWithTextSource");
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
