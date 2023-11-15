/**
 * @brief 旋转整个VTK场景得方案
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2023-10-20
 */

#include "vtkInteractorStyleRubberBandPick.h"
#include <vtkActor.h>
#include <vtkAutoInit.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkCylinderSource.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkWorldPointPicker.h>
#include "basic_interactor_style.h"


int main(int, char*[]) {
    vtkNew<vtkNamedColors> colors;
    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("WorldPointPicker");
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkNew<vtkWorldPointPicker> worldPointPicker;
    renderWindowInteractor->SetPicker(worldPointPicker);
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    vtkNew<vtkCylinderSource> source;
    source->SetResolution(4);
    source->Update();

    // Create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(source->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());
    renderer->AddActor(actor);
    vtkNew<vtkAxesActor> axes;
    // Add the actor to the scene
    renderer->AddActor(axes);

    vtkNew<BasicInteractorStyle> style;
    renderWindowInteractor->SetInteractorStyle(style);
    style->SetDefaultRenderer(renderer);
    auto data = source->GetOutput();
    style->setDataSource(data);

    // Render and interact
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}