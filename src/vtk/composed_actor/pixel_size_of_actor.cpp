// /**
//  * @brief 图像大小显示为像素大小
//  * @author zhiyu.zhang@cqbdri.pku.edu.cn
//  * @date 2022-11-28
//  */

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkDistanceToCamera.h>
#include <vtkGlyph3D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    //---------------------------------------------------------------------------
    // Draw some arrows that maintain a fixed size during zooming.

    // Create a set of points.
    vtkNew<vtkPointSource> fixedPointSource;
    fixedPointSource->SetNumberOfPoints(2);
    // Calculate the distance to the camera of each point.
    vtkNew<vtkDistanceToCamera> distanceToCamera;
    distanceToCamera->SetInputConnection(fixedPointSource->GetOutputPort());
    distanceToCamera->SetScreenSize(200.0); // 图例在屏幕的像素大小
    // Glyph each point with an arrow.
    vtkNew<vtkArrowSource> arrow;
    vtkNew<vtkGlyph3D> fixedGlyph;
    fixedGlyph->SetInputConnection(distanceToCamera->GetOutputPort());
    fixedGlyph->SetSourceConnection(arrow->GetOutputPort());
    // Scale each point.
    fixedGlyph->SetScaleModeToScaleByScalar();
    fixedGlyph->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "DistanceToCamera");
    fixedGlyph->SetScaleFactor(0.5);


    // Create a mapper.
    vtkNew<vtkPolyDataMapper> fixedMapper;
    fixedMapper->SetInputConnection(fixedGlyph->GetOutputPort());
    // fixedMapper->SetInputData(fixedGlyph->GetOutput()); // 必须用SetInputConnection形式，将管道连接，而不是只取数据
    fixedMapper->SetScalarVisibility(false);
    // Create an actor.
    vtkNew<vtkActor> fixedActor;
    fixedActor->SetMapper(fixedMapper);
    fixedActor->GetProperty()->SetColor(colors->GetColor3d("Cyan").GetData());
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("DistanceToCamera");
    renderWindow->SetSize(200, 600);

    // Give DistanceToCamera a pointer to the renderer.
    distanceToCamera->SetRenderer(renderer);

    renderer->AddActor(fixedActor);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}