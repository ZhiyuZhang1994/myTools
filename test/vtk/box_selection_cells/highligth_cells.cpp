/**
 * @brief 基于官方文档的单元选取交互
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2022-08-23
 */

#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkExtractGeometry.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVersion.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkSphereSource.h>
#include <vtkLabeledDataMapper.h>
#include <vtkActor2D.h>
#include <vtkIndent.h>
#include <vtkCellCenters.h>
#include <vtkTextProperty.h>
#include <vtkCylinderSource.h>

namespace {
// Define interaction style
class InteractorStyle : public vtkInteractorStyleRubberBandPick
{
public:
  static InteractorStyle* New();
  vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);

  InteractorStyle()
  {
    this->SelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    this->SelectedActor = vtkSmartPointer<vtkActor>::New();
    this->SelectedActor->SetMapper(SelectedMapper);
    this->SelectedMapper->ScalarVisibilityOff();
  }

  virtual void OnLeftButtonUp() override
  {
    vtkNew<vtkNamedColors> colors;

    // Forward events
    vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

    vtkPlanes* frustum = static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker()) ->GetFrustum();

    // 获取points对象：点集
    vtkNew<vtkExtractGeometry> extractGeometry;
    extractGeometry->SetImplicitFunction(frustum);
    extractGeometry->SetInputData(this->Points);
    extractGeometry->Update();

    auto oriData = extractGeometry->GetOutput();

    // 获取几何对象数据画图
    this->SelectedMapper->SetInputData(oriData);

    this->SelectedActor->GetProperty()->SetColor(
        colors->GetColor3d("Red").GetData());
    this->SelectedActor->GetProperty()->SetPointSize(5);
    SelectedActor->PrintSelf(std::cout,vtkIndent(4));
    this->CurrentRenderer->AddActor(SelectedActor);
    this->GetInteractor()->GetRenderWindow()->Render();
  }

  void SetPoints(vtkSmartPointer<vtkPolyData> points)
  {
    this->Points = points;
  }

private:
  vtkSmartPointer<vtkPolyData> Points;
  vtkSmartPointer<vtkActor> SelectedActor;
  vtkSmartPointer<vtkDataSetMapper> SelectedMapper;
};

vtkStandardNewMacro(InteractorStyle);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;
    // 定义数据源:16-point 6-cell
    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetResolution(4);
    cylinder->Update();

    // 获取点的label，命名label数组
    vtkNew<vtkIdFilter> idFilter;
    idFilter->SetInputConnection(cylinder->GetOutputPort());
    idFilter->SetPointIdsArrayName("OriginalIds");
    idFilter->Update();

    // 获取points集合:16-point 6-cell
    vtkNew<vtkDataSetSurfaceFilter> surfaceFilter;
    surfaceFilter->SetInputConnection(idFilter->GetOutputPort());
    surfaceFilter->Update();

    // 给原数据集添加label属性：获取到有label属性数据的数据集
    vtkPolyData* input = surfaceFilter->GetOutput();
    {
        vtkIdTypeArray* ids = dynamic_cast<vtkIdTypeArray*>(
        input->GetPointData()->GetArray("OriginalIds"));
        if (!ids) {
            std::cout << "nullptr" << std::endl;
        }
    }
    // 创建mapper：显示带label属性的actor，框选时才能知道选中了哪些label
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(input);
    mapper->ScalarVisibilityOff();

    //  创建actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

    // Visualize
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("HighlightSelectedPoints");
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

    // 拾取功能开始：
    vtkNew<vtkAreaPicker> areaPicker;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetPicker(areaPicker);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<InteractorStyle> style;
    style->SetPoints(input);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}