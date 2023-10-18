/**
 * @brief 显示节点与单元ID编号
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2022-08-23
 */


#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
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
#include <vtkProperty2D.h>
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
#include <vtkSelectVisiblePoints.h>
#include <vtkDoubleArray.h>
#include <vtkHexahedron.h>

using namespace std;

int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;

    // 刘帅自定义数据源 vtkUnstructuredGrid 27-point 8-cell
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (int y = -1; y <= 1; y++) { //循环y,-1到1
        for (int z = 1; z >= -1; z--) { //循环z,1到-1
            for (int x = -1; x <= 1; x++) { //循环x,-1到1
                points->InsertNextPoint(x, y, z);
            }
        }
    }
    // cout << "points num: " << points->GetNumberOfPoints() << endl;
    //模拟标量数据，暂定为结点y坐标
    vtkSmartPointer<vtkDoubleArray> doubleArray = vtkSmartPointer<vtkDoubleArray>::New();
    for (size_t i = 0; i < points->GetNumberOfPoints(); i++) {
        doubleArray->InsertNextTuple1(points->GetPoint(i)[1] / 3); //每个结点的y坐标
    }

    vtkSmartPointer<vtkUnstructuredGrid> grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    {                             //27个结点组成八个单元
        int num[] = {0, 1, 4, 3}; // 1 2 5 4,减一
        int add[] = {0, 1, 3, 4, 9, 10, 12, 13};
        vector<vector<int>> cells; //存8个序列
        const int dif = 9;
        for (size_t i = 0; i < 8; i++) {
            vector<int> elementNumber;
            vtkSmartPointer<vtkHexahedron> hex = vtkSmartPointer<vtkHexahedron>::New();
            // 形成每个单元的结点编号
            for (size_t j = 0; j < 8; j++) {
                if (j < 4) {
                    elementNumber.push_back(num[j] + add[i]);
                } else {
                    elementNumber.push_back(num[j - 4] + dif + add[i]);
                }
            }
            // 利用这些结点编号，插入单元中
            for (size_t j = 0; j < 8; j++) {
                hex->GetPointIds()->SetId(j, elementNumber[j]);
            }

            cells.push_back(elementNumber);
            grid->InsertNextCell(hex->GetCellType(), hex->GetPointIds()); //插入单元
        }
    }
    grid->SetPoints(points);
    grid->GetPointData()->SetScalars(doubleArray);

    {
        // 点数据集添加属性：用于label拾取的属性
        auto count = grid->GetNumberOfPoints();  // 16
        long *arr=new long[count];
        for (int i = 0; i < count; i++) {
            arr[i] = i + 1;
        }
        vtkNew<vtkLongArray> pointIndex;
        pointIndex->SetArray(arr, count, 1);
        pointIndex->SetName("PointsIDSet");
        grid->GetPointData()->AddArray(pointIndex);
    }

    {
        // 单元数据集添加属性：用于label拾取的属性
        auto count = grid->GetNumberOfCells(); // 6
        long *arr=new long[count];
        for (int i = 0; i < count; i++) {
            arr[i] = i + 1;
        }
        vtkNew<vtkLongArray> pointIndex;
        pointIndex->SetArray(arr, count, 1);
        pointIndex->SetName("CellsIDSet");
        grid->GetCellData()->AddArray(pointIndex);
    }

    grid->PrintSelf(std::cout,vtkIndent(2));

    // 创建mapper：显示带label属性的actor，框选时才能知道选中了哪些label
    // vtkDataSetMapper
    vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper->SetInputData(grid);
    mapper->SetInterpolateScalarsBeforeMapping(1);
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
        visPts->SetInputData(grid);
        visPts->SetRenderer(renderer);
        vtkSmartPointer<vtkLabeledDataMapper> nodeLabelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
        nodeLabelMapper->SetInputConnection(visPts->GetOutputPort());
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
        cellCenters->SetInputData(grid);
        vtkSmartPointer<vtkLabeledDataMapper> cellLabelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
        cellLabelMapper->SetInputConnection(cellCenters->GetOutputPort());
        cellLabelMapper->SetFieldDataName("CellsIDSet");
        cellLabelMapper->SetLabelModeToLabelFieldData();

        cellLabelMapper->SetLabelFormat("%d");  // 设置格式为整数
        vtkSmartPointer<vtkActor2D> actor2D = vtkSmartPointer<vtkActor2D>::New();
        cellLabelMapper->GetLabelTextProperty()->SetColor(0.0, 1.0, 0.0);
        actor2D->SetMapper(cellLabelMapper);
        actor2D->GetProperty()->SetColor(colors->GetColor3d("red").GetData());
        renderer->AddActor(actor2D);
    }

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}