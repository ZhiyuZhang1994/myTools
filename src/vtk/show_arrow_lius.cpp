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
#include <vtkCellData.h>

#include <vtkArrowSource.h>
#include <vtkGlyph3D.h>
#include <vtkDoubleArray.h>
#include <vtkPolyVertex.h>
#include <array>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <boost/algorithm/string.hpp>
using namespace std;

VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType); //新添加

// 多个点单元，显示箭头
namespace
{
  // Define interaction style
class InteractorStyle : public vtkInteractorStyleRubberBandPick
{
public:
    static InteractorStyle *New();
    vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);

    InteractorStyle()
    {
      this->SelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
      this->SelectedActor = vtkSmartPointer<vtkActor>::New();
      this->SelectedActor->SetMapper(SelectedMapper);
      this->ArrowActor = vtkSmartPointer<vtkActor>::New();
    }

    // points点集合；target箭头方向； rgb箭头颜色
    // 参考：自定义方向+设置箭头长度： https://blog.csdn.net/qq_37366618/article/details/124037374
    // 控制箭头参数： https://blog.csdn.net/hit1524468/article/details/104885678
    void DisplayArrowAtPoint(vtkSmartPointer<vtkPoints> points, double *target, double *rgb)
    {
        // 结点数==方向数组normals元素个数==长度数组scalars元素个数
        size_t pointNum = points->GetNumberOfPoints();

        //创建法向量属性，存入向量的朝向target
        vtkDoubleArray *normals = vtkDoubleArray::New();
        normals->SetNumberOfComponents(3);
        for (size_t i = 0; i < pointNum; i++)
        {
            normals->InsertNextTuple(target);
        }

        // 存储单元
        vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
        // 建立拓扑结构

        // 方案一：采用多个vertex单元
        // for (size_t i = 0; i < pointNum; i++)
        // {
        //   vtkVertex *vertex = vtkVertex::New(); //建立起点的拓扑
        //   // vertex->GetPointIds()->SetNumberOfIds(1);
        //   vertex->GetPointIds()->SetId(0, i); // setId(拓扑的id, 顶点的id)
        //   vertices->InsertNextCell(vertex);   //将建立的拓扑用vtkCellArray封装，用于赋予vtkPolyData
        // }

        // 方案二：采用一个vtkPolyVertex单元(多顶点单元)
        vtkNew<vtkPolyVertex> vtkPolyVertex;
        vtkPolyVertex->GetPointIds()->SetNumberOfIds(pointNum);
        for (size_t i = 0; i < pointNum; i++)
        {
            vtkPolyVertex->GetPointIds()->SetId(i, i);
        }
        vertices->InsertNextCell(vtkPolyVertex);

        // 组装数据结构
        vtkPolyData *polydata = vtkPolyData::New();    //创建几何实体
        polydata->SetPoints(points);                   //赋予几何
        polydata->SetVerts(vertices);                  //赋予拓扑
        polydata->GetPointData()->SetNormals(normals); //赋予向量朝向
        // polydata->GetPointData()->SetScalars(scalars); //赋予向量长度

        vtkArrowSource *arrow = vtkArrowSource::New();
        // 这里可以控制箭头的各个参数,轴线半径/辨率 ,端点半径/分辨率
        // arrow->SetShaftRadius(5);
        arrow->Update();

        // 在点的位置
        vtkGlyph3D *glyph = vtkGlyph3D::New();
        glyph->SetInputData(polydata);
        glyph->SetSourceData(arrow->GetOutput());
        glyph->SetScaleFactor(0.1);
        glyph->SetVectorModeToUseNormal(); // 使用法向数据表示图例的方向
        glyph->Update();

        vtkSmartPointer<vtkPolyDataMapper> polyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        polyDataMapper->SetInputData(glyph->GetOutput());
        polyDataMapper->Update();
        this->ArrowActor->SetMapper(polyDataMapper);
        this->ArrowActor->GetProperty()->SetColor(rgb); //暂时无用，原因是因为设置了scalar参数
        this->CurrentRenderer->AddActor(ArrowActor);
    }

    virtual void OnLeftButtonUp() override
    {
        vtkNew<vtkNamedColors> colors;

        // Forward events
        vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

        vtkPlanes *frustum = static_cast<vtkAreaPicker *>(this->GetInteractor()->GetPicker())->GetFrustum();

        // 获取框选出来的点集与单元集
        vtkSmartPointer<vtkExtractGeometry> extractGeometry = vtkSmartPointer<vtkExtractGeometry>::New();
        extractGeometry->SetImplicitFunction(frustum);
        extractGeometry->SetInputData(this->Points);
        extractGeometry->Update();

        {
            // 输出选中的节点ID
            vtkLongArray *ids1 = dynamic_cast<vtkLongArray *>(extractGeometry->GetOutput()->GetPointData()->GetArray("zzy"));
            std::cout << "----------------points id begin" << std::endl;
            if (!ids1)
            {
            std::cout << "nullptr" << std::endl;
            return;
            }
            for (vtkIdType i = 0; i < ids1->GetNumberOfTuples(); i++)
            {
            // std::cout << "Id " << i << " : " << ids1->GetValue(i) << std::endl;
            }
            std::cout << "----------------points id end" << std::endl;
        }

        {
            // 输出获取的节点对象: vtkPoints中id为下标索引，没有节点编号的概念
            vtkPoints *points = extractGeometry->GetOutput()->GetPoints();
            for (uint32_t i = 0; i < points->GetNumberOfPoints(); ++i)
            {
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

        // 增加箭头(开始)--lius
        vtkSmartPointer<vtkPoints> points2 = vtkSmartPointer<vtkPoints>::New();
        points2->DeepCopy(extractGeometry->GetOutput()->GetPoints());
        double target[3] = {-1, -1, -1};
        double length = 1;
        double rgb[3] = {1, 1, 0};
        // points结点集合；target箭头方向；length箭头长度； rgb箭头颜色
        DisplayArrowAtPoint(points2, target, rgb);

        // 增加箭头(结束)--lius
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
    vtkSmartPointer<vtkActor> ArrowActor;
    vtkSmartPointer<vtkDataSetMapper> SelectedMapper;
};

  vtkStandardNewMacro(InteractorStyle);
} // namespace

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;
    // 定义数据源:16-point 6-cell
    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetResolution(4);
    cylinder->Update();
    vtkPolyData *polyData = cylinder->GetOutput();

    // 数据集添加属性：用于label拾取的属性
    auto count = polyData->GetNumberOfPoints(); // 16
    long *arr = new long[count];
    for (int i = 0; i < count; i++)
    {
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
    renderWindow->SetSize(900, 600);
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

    // 拾取功能开始：
    vtkNew<vtkAreaPicker> areaPicker;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetPicker(areaPicker);
    renderWindowInteractor->SetRenderWindow(renderWindow);

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