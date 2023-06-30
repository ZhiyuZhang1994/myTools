/**
 * @file distance_to_camera.cpp
 * @author liushuai (you@domain.com)
 * @brief 实现固定像素的2种方式:1.vtkDistanceToCamera+vtkGlyph3D+vtkPolyDataMapper;2.vtkDistanceToCamera+vtkGlyph3DMapper;.vtk参考https://kitware.github.io/vtk-examples/site/Cxx/Visualization/DistanceToCamera/
 * @version 0.1
 * @date 2023-01-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkAxesActor.h>
#include <vtkDistanceToCamera.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph3DMapper.h>
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

int main(int, char*[]) {
    vtkNew<vtkNamedColors> colors;

    //---------------------------------------------------------------------------
    // Draw some arrows that maintain a fixed size during zooming.

    // Create a set of points.
    vtkNew<vtkPointSource> fixedPointSource;
    fixedPointSource->SetNumberOfPoints(2);

    // Calculate the distance to the camera of each point.
    vtkNew<vtkDistanceToCamera> distanceToCamera;
    distanceToCamera->SetInputConnection(fixedPointSource->GetOutputPort());
    distanceToCamera->SetScreenSize(100.0);
    // Glyph each point with an arrow.
    vtkNew<vtkArrowSource> arrow;

    //-------------------------1.vtkDistanceToCamera+vtkGlyph3D+vtkPolyDataMapper，实现固定像素显示
    vtkNew<vtkGlyph3D> fixedGlyph;
    fixedGlyph->SetInputConnection(distanceToCamera->GetOutputPort());
    fixedGlyph->SetSourceConnection(arrow->GetOutputPort());

    // Scale each point.
    fixedGlyph->SetScaleModeToScaleByScalar(); //可以没有
    fixedGlyph->SetInputArrayToProcess(
        0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "DistanceToCamera"); //不可缺少，name不可改变长度与大小写

    // Create a mapper.
    vtkNew<vtkPolyDataMapper> fixedMapper;
    fixedMapper->SetInputConnection(fixedGlyph->GetOutputPort());
    fixedMapper->SetScalarVisibility(false);

    // Create an actor.
    vtkNew<vtkActor> fixedActor;
    fixedActor->SetMapper(fixedMapper);
    fixedActor->GetProperty()->SetColor(colors->GetColor3d("Cyan").GetData());

    //---------------------------------------------------------------------------
    // Draw some spheres that maintain a fixed size during zooming.
    // Create a set of points.
    vtkNew<vtkPointSource> pointSource;
    pointSource->SetNumberOfPoints(4);

    // Glyph each point with a sphere.
    double radius = .2;
    vtkNew<vtkSphereSource> sphere; //球体
    // sphere->SetRadius(radius);

    // Calculate the distance to the camera of each point.
    vtkNew<vtkDistanceToCamera> distanceToCamera2;
    distanceToCamera2->SetInputConnection(pointSource->GetOutputPort());
    distanceToCamera2->SetScreenSize(60.0);

    //-------------------------2.vtkDistanceToCamera+vtkGlyph3DMapper，实现固定像素显示
    vtkNew<vtkGlyph3DMapper> glyph3DMapper;
    glyph3DMapper->SetInputConnection(distanceToCamera2->GetOutputPort());
    glyph3DMapper->SetSourceConnection(sphere->GetOutputPort());
    //要有SetScaleModeToScaleByMagnitude().
    // SetScaleModeToScaleNoDataScaling不起作用，SetScaleModeToScaleByVectorComponents需要DistanceToCamera have 3 components,
    glyph3DMapper->SetScaleModeToScaleByMagnitude();
    glyph3DMapper->SetScalarVisibility(false);
    glyph3DMapper->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "DistanceToCamera"); //不可缺少，name不可改变长度与大小写

    // Create an actor.
    vtkNew<vtkActor> actor;
    actor->SetMapper(glyph3DMapper); //mapper或者glyph3DMapper
    actor->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());

    // Create a vtkAxesActor
    vtkNew<vtkAxesActor> axesActor;

    //---------------------------------------------------------------------------

    // A renderer and render window.
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("DistanceToCamera");

    // Give DistanceToCamera a pointer to the renderer.
    distanceToCamera->SetRenderer(renderer);
    distanceToCamera2->SetRenderer(renderer);

    // Add the actors to the scene.
    renderer->AddActor(fixedActor);
    renderer->AddActor(actor);
    renderer->AddActor(axesActor);

    // An interactor.
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Render an image (lights and cameras are created automatically).
    renderWindow->Render();

    // Begin mouse interaction.
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
