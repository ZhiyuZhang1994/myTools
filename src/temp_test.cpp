/**
 * @brief 利用VTK vtkPolyLine单元绘制有限元的刚性单元
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2023-07-13
 */

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAreaPicker.h>
#include <vtkAutoInit.h>
#include <vtkCellArray.h>
#include <vtkCellCenters.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLabeledDataMapper.h>
#include <vtkLongArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>
#include <vtkInteractorStyleRubberBandPick.h>


vtkSmartPointer<vtkPoints> generatePointsSets(std::vector<double> center, std::vector<std::vector<double>> outside) {
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(center[0], center[1], center[2]);
    int num = outside.size();
    for (int i = 0; i < num; i++) {
        points->InsertNextPoint(outside[i][0], outside[i][1], outside[i][2]);
    }
    return points;
}

int main(int, char *[]) {
    // 基础设置
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("PolyLine");
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    vtkNew<vtkInteractorStyleRubberBandPick> style;
    renderWindowInteractor->SetInteractorStyle(style);
    vtkNew<vtkAreaPicker> areaPicker;
    renderWindowInteractor->SetPicker(areaPicker);

    // Create five points.
    std::vector<double> center = {0.0, 0.0, 0.0};
    std::vector<std::vector<double>> outside = {
        {1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.5, 0.5, 0},
        {-1.0, 2.0, 0},
        {-1.0, -2.0, 0},
        {-1, 0.1, 0},
        {1, -1, 0}};

    // Create a vtkPoints object and store the points in it
    auto points = generatePointsSets(center, outside);

    vtkNew<vtkPolyLine> polyLine;
    int numPoints = points->GetNumberOfPoints(); // 8 个
    polyLine->GetPointIds()->SetNumberOfIds((numPoints - 1) * 2); // 单元中节点的个数： 14 个
    for (unsigned int i = 0; i < numPoints - 1; i++) { // 循环中心点个数次： 7 次
        // polyLine->GetPointIds()->SetId(单元中节点的索引, 节点编号：即在节点数组中的下标);
        polyLine->GetPointIds()->SetId(2 * i, 0); // 放入中心节点
        polyLine->GetPointIds()->SetId(2 * i + 1, i + 1); // 放入周围节点
    }

    // Create a cell array to store the lines in and add the lines to it
    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell(polyLine);

    // Create a polydata to store everything in
    vtkNew<vtkPolyData> polyData;

    // Add the points to the dataset
    polyData->SetPoints(points);

    // Add the lines to the dataset
    polyData->SetLines(cells);

    // Setup actor and mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());

    //生成id
    {
        // 点数据集添加属性：用于label拾取的属性
        auto count = polyData->GetNumberOfPoints();
        long *arr = new long[count];
        for (int i = 0; i < count; i++) {
            arr[i] = i;
        }
        vtkNew<vtkLongArray> pointIndex;
        pointIndex->SetArray(arr, count, 1);
        pointIndex->SetName("PointsIDSet");
        polyData->GetPointData()->AddArray(pointIndex);
    }

    {
        // 单元数据集添加属性：用于label拾取的属性
        auto count = polyData->GetNumberOfCells();
        long *arr = new long[count];
        for (int i = 0; i < count; i++) {
            arr[i] = i;
        }
        vtkNew<vtkLongArray> pointIndex;
        pointIndex->SetArray(arr, count, 1);
        pointIndex->SetName("CellsIDSet");
        polyData->GetCellData()->AddArray(pointIndex);
    }

    renderer->AddActor(actor);

    {
        // 显示节点标量值
        vtkSmartPointer<vtkLabeledDataMapper> nodeLabelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
        nodeLabelMapper->SetInputData(polyData);
        nodeLabelMapper->SetFieldDataName("PointsIDSet");
        nodeLabelMapper->SetLabelModeToLabelFieldData();

        nodeLabelMapper->SetLabelFormat("%d"); // 设置格式为整数
        vtkSmartPointer<vtkActor2D> actor2D = vtkSmartPointer<vtkActor2D>::New();
        nodeLabelMapper->GetLabelTextProperty()->SetColor(1.0, 1.0, 0.0);
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

        cellLabelMapper->SetLabelFormat("%d"); // 设置格式为整数
        vtkSmartPointer<vtkActor2D> actor2D = vtkSmartPointer<vtkActor2D>::New();
        cellLabelMapper->GetLabelTextProperty()->SetColor(0.0, 1.0, 0.0);
        actor2D->SetMapper(cellLabelMapper);
        //actor2D->SetPosition2(1, 0, 0);
        renderer->AddActor(actor2D);
    }

    renderWindow->Render();
    renderWindowInteractor->Start();
    return 0;
}