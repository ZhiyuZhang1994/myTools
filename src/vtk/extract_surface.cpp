/**
 * @brief vtkDataSetSurfaceFilter测试
 * @author zhangzhiyu
 * @date 2023-01-05
 */

/**
 * 总结：
 * vtkDataSetSurfaceFilter将三维网格cell的每个面单独抽取为一个二位网格cell
 * 如以下算例：两个六面体单元(一个单元8节点6个面)抽取后形成12个平面单元(一个单元4个节点1个面)
 * tip: 两单元相挨着的两个面不会合并，会抽取为两个平面单元
 */

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCutter.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkHexahedron.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlane.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnstructuredGrid.h>

int main()
{
    vtkNew<vtkNamedColors> colors;

    // Setup the coordinates of eight points
    // (the two faces must be in counter clockwise order as viewed from the
    // outside)
    // 单元1的节点
    std::array<std::array<double, 3>, 8> pointCoords{{{{0.0, 0.0, 0.0}},
                                                      {{1.0, 0.0, 0.0}},
                                                      {{1.0, 1.0, 0.0}},
                                                      {{0.0, 1.0, 0.0}},
                                                      {{0.0, 0.0, 1.0}},
                                                      {{1.0, 0.0, 1.0}},
                                                      {{1.0, 1.0, 1.0}},
                                                      {{0.0, 1.0, 1.0}}}};

    // Create the points and a hexahedron from the points.
    vtkNew<vtkPoints> points;
    vtkNew<vtkHexahedron> hexa;
    for (auto i = 0; i < pointCoords.size(); ++i)
    {
        points->InsertNextPoint(pointCoords[i].data());
        hexa->GetPointIds()->SetId(i, i);
    }

    // 单元2的节点
    std::array<std::array<double, 3>, 8> pointCoords2{{{{1.0, 0.0, 0.0}},
                                                       {{2.0, 0.0, 0.0}},
                                                       {{2.0, 1.0, 0.0}},
                                                       {{1.0, 1.0, 0.0}},
                                                       {{1.0, 0.0, 1.0}},
                                                       {{2.0, 0.0, 1.0}},
                                                       {{2.0, 1.0, 1.0}},
                                                       {{1.0, 1.0, 1.0}}}};

    // Create the points and a hexahedron from the points.
    vtkNew<vtkHexahedron> hexa2;
    for (auto i = 0; i < pointCoords2.size(); ++i)
    {
        points->InsertNextPoint(pointCoords2[i].data());
        hexa2->GetPointIds()->SetId(i, i + 8);
    }

    // 将两个单元放在网格对象中
    // Add the points and hexahedron to an unstructured grid.
    vtkNew<vtkUnstructuredGrid> uGrid;
    uGrid->SetPoints(points);
    uGrid->InsertNextCell(hexa->GetCellType(), hexa->GetPointIds());
    uGrid->InsertNextCell(hexa2->GetCellType(), hexa2->GetPointIds());

    // Extract the outer (polygonal) surface.
    vtkNew<vtkDataSetSurfaceFilter> surface;
    surface->SetPieceInvariant(1);
    surface->SetInputData(uGrid);
    surface->Update();

    vtkNew<vtkDataSetMapper> aBeamMapper;
    aBeamMapper->SetInputConnection(surface->GetOutputPort());

    surface->GetOutput()->PrintSelf(std::cout, vtkIndent(4));

    vtkNew<vtkActor> aBeamActor;
    aBeamActor->SetMapper(aBeamMapper);
    aBeamActor->AddPosition(0, 0, 0);
    aBeamActor->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());
    aBeamActor->GetProperty()->SetOpacity(0.60);
    aBeamActor->GetProperty()->EdgeVisibilityOn();
    aBeamActor->GetProperty()->SetEdgeColor(
        colors->GetColor3d("Black").GetData());
    aBeamActor->GetProperty()->SetLineWidth(1.5);

    // Create a plane to cut, here it cuts in the XZ direction
    // (xz normal=(1,0,0); XY =(0,0,1), YZ =(0,1,0)
    vtkNew<vtkPlane> plane;
    plane->SetOrigin(0.5, 0, 0);
    plane->SetNormal(1, 0, 0);

    // Create cutter
    vtkNew<vtkCutter> cutter;
    cutter->SetCutFunction(plane);
    cutter->SetInputData(aBeamActor->GetMapper()->GetInput());
    cutter->Update();
    vtkNew<vtkPolyDataMapper> cutterMapper;
    cutterMapper->SetInputConnection(cutter->GetOutputPort());

    // Create plane actor
    vtkNew<vtkActor> planeActor;
    planeActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
    planeActor->GetProperty()->SetLineWidth(2);
    planeActor->SetMapper(cutterMapper);

    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetWindowName("DatasetSurface");
    renderWindow->AddRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Add the actors to the scene
    renderer->AddActor(aBeamActor);
    renderer->AddActor(planeActor);
    renderer->SetBackground(colors->GetColor3d("Seashell").GetData());

    renderer->ResetCamera();
    renderer->GetActiveCamera()->Azimuth(-25);
    renderer->GetActiveCamera()->Elevation(30);

    // Render and interact
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}