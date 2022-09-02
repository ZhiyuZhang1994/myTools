/**
 * @brief 利用点集形成vertex actor显示
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2022-08-23
 */
// 参考：https://blog.csdn.net/Asimov_Liu/article/details/82992319

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
#include <vtkLongArray.h>
#include <vtkVertex.h>
#include <vtkUnstructuredGrid.h>


#if VTK_VERSION_NUMBER >= 89000000000ULL
#define VTK890 1
#endif

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
  }

  virtual void OnLeftButtonUp() override
  {
    vtkNew<vtkNamedColors> colors;

    // Forward events
    vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

    vtkPlanes* frustum = static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker())->GetFrustum();

    // 获取框选出来的点集与单元集
    vtkSmartPointer<vtkExtractGeometry> extractGeometry = vtkSmartPointer<vtkExtractGeometry>::New();
    extractGeometry->SetImplicitFunction(frustum);
    extractGeometry->SetInputData(this->Points);
    extractGeometry->Update();

    {
        // 输出选中的节点ID
        vtkLongArray* ids1 = dynamic_cast<vtkLongArray*>(extractGeometry->GetOutput()->GetPointData()->GetArray("zzy"));
        std::cout << "----------------points id begin" << std::endl;
        if (!ids1) {
            std::cout << "nullptr" << std::endl;
            return;
        }
        for (vtkIdType i = 0; i < ids1->GetNumberOfTuples(); i++)
        {
        std::cout << "Id " << i << " : " << ids1->GetValue(i) << std::endl;
        }
        std::cout << "----------------points id end" << std::endl;
    }


    {
        // 输出获取的节点对象: vtkPoints中id为下标索引，没有节点编号的概念
        vtkPoints* points = extractGeometry->GetOutput()->GetPoints();
        for (uint32_t i = 0; i < points->GetNumberOfPoints(); ++i) {
            auto data_x = points->GetPoint(i);
            std::cout << " data_x" << data_x[0] << data_x[1] << data_x[2] << std::endl;
        }
    }

    // 获取点集合
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->DeepCopy(extractGeometry->GetOutput()->GetPoints());

    // 组装vertex对象
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);
    vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    vertexFilter->SetInputData(polyData);
    vertexFilter->Update();
    
    this->SelectedMapper->SetInputData(vertexFilter->GetOutput());
    SelectedMapper->Update();

    std::cout << "----------------cell count: " << vertexFilter->GetOutput()->GetNumberOfCells() << std::endl;

    this->SelectedActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
    this->SelectedActor->GetProperty()->SetPointSize(10);
    this->CurrentRenderer->AddActor(SelectedActor);
    this->GetInteractor()->GetRenderWindow()->Render();
    this->HighlightProp(NULL);
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
    vtkPolyData* polyData = cylinder->GetOutput();

    // 数据集添加属性：用于label拾取的属性
    auto count = polyData->GetNumberOfPoints();  // 16
    long *arr=new long[count];
    for (int i = 0; i < count; i++) {
        arr[i] = i;
    }
    vtkNew<vtkLongArray> pointIndex;
    pointIndex->SetArray(arr, count, 1);
    pointIndex->SetName("zzy");
    polyData->GetPointData()->AddArray(pointIndex);


    // 创建mapper：显示带label属性的actor，框选时才能知道选中了哪些label
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polyData);
    // mapper->ScalarVisibilityOn();

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
    // renderWindowInteractor->SetKeyCode('r');

    {
        // 加label
        vtkNew<vtkIdFilter> idFilter;
        idFilter->SetInputData(polyData);
        idFilter->Update();
        vtkSmartPointer<vtkLabeledDataMapper> nodeLabelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
        nodeLabelMapper->SetInputConnection(idFilter->GetOutputPort());
        vtkSmartPointer<vtkActor2D> actor2D = vtkSmartPointer<vtkActor2D>::New();
        nodeLabelMapper->GetLabelTextProperty()->SetColor(0.0, 1.0, 0.0);
        actor2D->SetMapper(nodeLabelMapper);
        renderer->AddActor(actor2D);
    }


    vtkNew<InteractorStyle> style;
    style->StartSelect(); // 开始选择按键的设置
    style->SetPoints(polyData);
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}