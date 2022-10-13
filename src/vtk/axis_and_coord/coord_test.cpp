#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkLine.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnsignedCharArray.h>
#include <vtkAxesActor.h>
#include <vtkTransform.h>
#include <vtkCamera.h>
#include <iostream>
#include <cmath>
// For compatibility with new VTK generic data arrays
#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif

class Coord
{
public:
    Coord() = default;
    Coord(double x, double y, double z) : x(x), y(y), z(z) {}
    Coord(double arr[3]) : x(arr[0]), y(arr[1]), z(arr[2]) {}
    Coord(const Coord &point) = default;

public:
    double x{0};
    double y{0};
    double z{0};
};

// 计算三点成面的法向量
Coord Cal_Normal_3D(const Coord &v1, const Coord &v2, const Coord &v3)
{
    double na = (v2.y - v1.y) * (v3.z - v1.z) - (v2.z - v1.z) * (v3.y - v1.y);
    double nb = (v2.z - v1.z) * (v3.x - v1.x) - (v2.x - v1.x) * (v3.z - v1.z);
    double nc = (v2.x - v1.x) * (v3.y - v1.y) - (v2.y - v1.y) * (v3.x - v1.x);

    return Coord(na, nb, nc);
}

int main(int, char *[])
{
    // Create the polydata where we will store all the geometric data
    vtkNew<vtkPolyData> linesPolyData;

    // Create three points
    double origin[3] = {0.0, 0.0, 0.0};
    double p0[3] = {1.0, 2.0, 0.0};
    double p1[3] = {3.0, 1.0, 0.0};

    Coord p2 = Cal_Normal_3D(origin, p0, p1);
    //归一化
    double length = std::sqrt(std::pow(p2.x,2) + std::pow(p2.y,2) + std::pow(p2.z,2));
    p2.x = p2.x / length;
    p2.y = p2.y / length;
    p2.z = p2.z / length;

    std::cout << p2.x << p2.y << p2.z << std::endl;

    // Create a vtkPoints container and store the points in it
    vtkNew<vtkPoints> pts;
    pts->InsertNextPoint(origin);
    pts->InsertNextPoint(p0);
    pts->InsertNextPoint(p1);
    pts->InsertNextPoint(p2.x, p2.y, p2.z);

    // Add the points to the polydata container
    linesPolyData->SetPoints(pts);

    // Create the first line (between Origin and P0)
    vtkNew<vtkLine> line0;
    line0->GetPointIds()->SetId(
        0,
        0); // the second 0 is the index of the Origin in linesPolyData's points
    line0->GetPointIds()->SetId(
        1, 1); // the second 1 is the index of P0 in linesPolyData's points

    // Create the second line (between Origin and P1)
    vtkNew<vtkLine> line1;
    line1->GetPointIds()->SetId(
        0,
        0); // the second 0 is the index of the Origin in linesPolyData's points
    line1->GetPointIds()->SetId(
        1, 2); // 2 is the index of P1 in linesPolyData's points

    // Create the second line (between Origin and P1)
    vtkNew<vtkLine> line2;
    line2->GetPointIds()->SetId(
        0,
        0); // the second 0 is the index of the Origin in linesPolyData's points
    line2->GetPointIds()->SetId(
        1, 3); // 2 is the index of P1 in linesPolyData's points

    // Create a vtkCellArray container and store the lines in it
    vtkNew<vtkCellArray> lines;
    lines->InsertNextCell(line0);
    lines->InsertNextCell(line1);
    lines->InsertNextCell(line2);

    // Add the lines to the polydata container
    linesPolyData->SetLines(lines);

    vtkNew<vtkNamedColors> namedColors;

    // Create a vtkUnsignedCharArray container and store the colors in it
    vtkNew<vtkUnsignedCharArray> colors;
    colors->SetNumberOfComponents(3);
    colors->InsertNextTupleValue(namedColors->GetColor3ub("Tomato").GetData());
    colors->InsertNextTupleValue(namedColors->GetColor3ub("Mint").GetData());
    colors->InsertNextTupleValue(namedColors->GetColor3ub("red").GetData());

    // Color the lines.
    // SetScalars() automatically associates the values in the data array passed
    // as parameter to the elements in the same indices of the cell data array on
    // which it is called. This means the first component (red) of the colors
    // array is matched with the first component of the cell array (line 0) and
    // the second component (green) of the colors array is matched with the second
    // component of the cell array (line 1)
    linesPolyData->GetCellData()->SetScalars(colors);

    // Setup the visualization pipeline
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(linesPolyData);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetLineWidth(4);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(namedColors->GetColor3d("SlateGray").GetData());

    // zzy

    vtkNew<vtkAxesActor> axes;
    renderer->AddActor(axes);
    vtkNew<vtkTransform> transform;
    transform->Translate(-1.0, -1.0, 0.0);
    // The axes are positioned with a user transform
    axes->SetUserTransform(transform);
    vtkCamera *activeCamera = renderer->GetActiveCamera();
    activeCamera->SetPosition(0, 0, 0);
    activeCamera->SetFocalPoint(-1.0, -1.0, 0);
    // zzy

    renderer->ResetCamera();
    vtkNew<vtkRenderWindow> window;
    window->SetWindowName("Colored Lines");
    window->AddRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(window);

    // Visualize
    window->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}