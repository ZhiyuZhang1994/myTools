/**
 * @brief 显示节点与单元ID编号
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2022-08-23
 */

#include <vtkLongArray.h>
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
#include <vtkVertex.h>
#include <vtkUnstructuredGrid.h>
#include <vtkCellData.h>

int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;
    // 定义数据源:16-point 6-cell
    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetResolution(4);
    cylinder->Update();
    vtkPolyData* polyData = cylinder->GetOutput();

    {
        // 点数据集添加属性：用于label拾取的属性
        auto count = polyData->GetNumberOfPoints();  // 16
        long *arr=new long[count];
        for (int i = 0; i < count; i++) {
            arr[i] = 33;
        }
        vtkNew<vtkLongArray> pointIndex;
        pointIndex->SetArray(arr, count, 1);
        pointIndex->SetName("PointsIDSet");
        polyData->GetPointData()->AddArray(pointIndex);
    }

    {
        // 单元数据集添加属性：用于label拾取的属性
        auto count = polyData->GetNumberOfCells(); // 6
        long *arr=new long[count];
        for (int i = 0; i < count; i++) {
            arr[i] = i + 1;
        }
        vtkNew<vtkLongArray> pointIndex;
        pointIndex->SetArray(arr, count, 1);
        pointIndex->SetName("CellsIDSet");
        polyData->GetCellData()->AddArray(pointIndex);
    }

    polyData->PrintSelf(std::cout,vtkIndent(2));

    // 创建mapper：显示带label属性的actor，框选时才能知道选中了哪些label
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polyData);
    // mapper->ScalarVisibilityOn();
    mapper->SetScalarModeToUseCellData();

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

    {
        // 显示节点标量值
        vtkSmartPointer<vtkSelectVisiblePoints> visPts = vtkSmartPointer<vtkSelectVisiblePoints>::New();
        visPts->SetInputData(polyData);
        visPts->SetRenderer(renderer);
        vtkSmartPointer<vtkLabeledDataMapper> nodeLabelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
        nodeLabelMapper->SetInputData(polyData);
        nodeLabelMapper->SetFieldDataName("PointsIDSet");
        nodeLabelMapper->SetLabelModeToLabelFieldData();

        nodeLabelMapper->SetLabelFormat("%d");  // 设置格式为整数
        vtkSmartPointer<vtkActor2D> actor2D = vtkSmartPointer<vtkActor2D>::New();
        nodeLabelMapper->GetLabelTextProperty()->SetColor(0.0, 1.0, 0.0);
        actor2D->SetMapper(nodeLabelMapper);
        renderer->AddActor(actor2D);
    }

    {
        // 显示cell标量值
        vtkSmartPointer<vtkCellCenters> cellCenters = vtkSmartPointer<vtkCellCenters>::New();
        cellCenters->SetInputData(polyData);
        vtkSmartPointer<vtkLabeledDataMapper> cellLabelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
        cellLabelMapper->SetInputConnection(cellCenters->GetOutputPort());
        cellLabelMapper->SetFieldDataName("CellsIDSet");
        cellLabelMapper->SetLabelModeToLabelFieldData();

        cellLabelMapper->SetLabelFormat("%d");  // 设置格式为整数
        vtkSmartPointer<vtkActor2D> actor2D = vtkSmartPointer<vtkActor2D>::New();
        cellLabelMapper->GetLabelTextProperty()->SetColor(0.0, 1.0, 0.0);
        actor2D->SetMapper(cellLabelMapper);
        renderer->AddActor(actor2D);
    }

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}