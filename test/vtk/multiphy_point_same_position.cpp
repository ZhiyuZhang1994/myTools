/**
 * @brief 同一个位置添加多个点，在不同单元显示不同颜色
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @version 0.1
 * @date 2024-01-04
 */
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAreaPicker.h>
#include <vtkArrowSource.h>
#include <vtkAxesActor.h>
#include <vtkCellCenters.h>
#include <vtkCellData.h>
#include <vtkContourFilter.h>
#include <vtkCutter.h>
#include <vtkCylinderSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGlyph3D.h>
#include <vtkHexahedron.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkIndent.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkLabeledDataMapper.h>
#include <vtkLongArray.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPlane.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyVertex.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkScalarBarActor.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTextProperty.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVersion.h>
#include <vtkVertex.h>
#include <vtkContourFilter.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkFloatArray.h>
#include <vtkCellDataToPointData.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkStripper.h>
#include "vtkGenericCell.h"
#include "vtkCharArray.h"
using namespace std;

int main(int, char*[]) {
    vtkNew<vtkNamedColors> colors;
    vtkSmartPointer<vtkUnstructuredGrid> grid = vtkSmartPointer<vtkUnstructuredGrid>::New();

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(0, 0, 0); // 0
    points->InsertNextPoint(1, 0, 0); // 1
    points->InsertNextPoint(1, 1, 0); // 2
    points->InsertNextPoint(0, 1, 0); // 3 *

    points->InsertNextPoint(1, 2, 0); // 4
    points->InsertNextPoint(0, 2, 0); // 5
    points->InsertNextPoint(0, 1, 0); // 6 *
    // {0 1 2 3}
    // {6 2 4 5}
    cout << "points num: " << points->GetNumberOfPoints() << endl;
    grid->SetPoints(points);


    /*=============================设置单元=============================*/
    vtkNew<vtkIdList> pointsId;
    pointsId->Initialize();
    pointsId->SetNumberOfIds(4);
    pointsId->SetId(0, 0);
    pointsId->SetId(1, 1);
    pointsId->SetId(2, 2);
    pointsId->SetId(3, 3);
    grid->InsertNextCell(VTK_QUAD, 4, pointsId->GetPointer(0));
    pointsId->Initialize();
    pointsId->SetNumberOfIds(4);
    pointsId->SetId(0, 6);
    pointsId->SetId(1, 2);
    pointsId->SetId(2, 4);
    pointsId->SetId(3, 5);
    grid->InsertNextCell(VTK_QUAD, 4, pointsId->GetPointer(0));


    /*==============================设置节点颜色========================*/
    vtkNew<vtkFloatArray> color; /* 用于设置每个节点的随机颜色 */
    for (int i = 0; i < grid->GetNumberOfPoints(); ++i)
    {
        color->InsertNextValue(i);
    }
    color->SetName("zzy");
    grid->GetPointData()->AddArray(color);
    grid->GetPointData()->SetScalars(grid->GetPointData()->GetArray("zzy"));
    /*=================================================================*/

    // vtkDataSetMapper
    vtkNew<vtkDataSetMapper> dataSetMapper;
    dataSetMapper->SetInputData(grid);
    dataSetMapper->ScalarVisibilityOn();
    dataSetMapper->UseLookupTableScalarRangeOn(); // 代替SetScalarRange()方法，与lookupTable使用相同的ScalarRange;
    dataSetMapper->SetScalarModeToUsePointData(); // 使用结点数据

    // 颜色映射表
    vtkSmartPointer<vtkLookupTable> pColorTableModel = vtkSmartPointer<vtkLookupTable>::New();
    pColorTableModel->SetNumberOfColors(8);
    pColorTableModel->SetHueRange(0.67, 0); // 色调从蓝色到绿色到红色，【0，0.67】则表示从红色到蓝红色
    pColorTableModel->SetValueRange(0.7529, 0.7059);
    pColorTableModel->SetSaturationRange(0.69, 0.97);
    double rangeMin = grid->GetScalarRange()[0];
    double rangeMax = grid->GetScalarRange()[1];
    pColorTableModel->SetTableRange(rangeMin, rangeMax); // 最大值最小值
    pColorTableModel->Build();

    dataSetMapper->SetLookupTable(pColorTableModel);
    dataSetMapper->SetInterpolateScalarsBeforeMapping(1); // 显示有颜色界限的云图

    // 渲染网格的actor
    vtkSmartPointer<vtkActor> actorGrid = vtkSmartPointer<vtkActor>::New();
    actorGrid->SetMapper(dataSetMapper);
    actorGrid->GetProperty()->SetLineWidth(1);
    actorGrid->GetProperty()->SetEdgeVisibility(1);

    // 设置scalarBar（色卡）
    vtkSmartPointer<vtkScalarBarActor> scalarBarActor = vtkSmartPointer<vtkScalarBarActor>::New();
    scalarBarActor->SetLookupTable(pColorTableModel);

    scalarBarActor->SetTitle("Height");
    scalarBarActor->GetTitleTextProperty()->ShadowOff(); // “标题”关闭阴影
    scalarBarActor->GetLabelTextProperty()->ShadowOff(); // “标签”关闭阴影
    scalarBarActor->GetTitleTextProperty()->ItalicOff(); // “标题”关闭斜体
    scalarBarActor->GetLabelTextProperty()->ItalicOff(); // “标签”关闭斜体
    scalarBarActor->GetTitleTextProperty()->BoldOff();   // “标题”不加粗
    scalarBarActor->GetLabelTextProperty()->BoldOff();   // “标签”不加粗
    scalarBarActor->SetNumberOfLabels(9);
    // scalarBarActor->SetLabelFormat("%!f(string=2023)");//%-#6.3g
    scalarBarActor->SetLabelFormat("%.3ecm"); // 设置统一后缀

    // 显示points标量值
    vtkSmartPointer<vtkLabeledDataMapper> nodeLabelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
    nodeLabelMapper->SetInputData(grid);
    nodeLabelMapper->SetFieldDataName("zzy");
    nodeLabelMapper->SetLabelModeToLabelFieldData();
    vtkSmartPointer<vtkActor2D> actor2D2 = vtkSmartPointer<vtkActor2D>::New();
    actor2D2->SetMapper(nodeLabelMapper);

    // Visualize
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("set_vtkContourFilter");
    renderWindow->SetSize(900, 600);
    renderer->AddActor(scalarBarActor);
    renderer->AddActor(actorGrid);
    // renderer->AddActor(actor2D2);

    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
    // 交互
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
