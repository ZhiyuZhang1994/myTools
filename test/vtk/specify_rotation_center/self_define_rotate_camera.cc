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

namespace
{

// Define interaction style
class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera {
public:
    static MouseInteractorStyle *New();
    vtkTypeMacro(MouseInteractorStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonDown() override {
        if (this->Interactor->GetAltKey()) {
            std::cout << "Picking pixel: " << this->Interactor->GetEventPosition()[0]
                      << " " << this->Interactor->GetEventPosition()[1] << std::endl;
            this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
                                                this->Interactor->GetEventPosition()[1],
                                                0, // always zero.
                                                this->Interactor->GetRenderWindow()
                                                    ->GetRenderers()
                                                    ->GetFirstRenderer());
            double picked[3];
            this->Interactor->GetPicker()->GetPickPosition(picked);
            for (int i = 0; i < 3; i++) {
                this->rotate_center[i] = picked[i];
            }
            this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera()->SetFocalPoint(picked);
            vtkNew<vtkSphereSource> point;
            point->SetCenter(picked);
            point->SetRadius(0.02);
            point->Update();
            vtkNew<vtkPolyDataMapper> mapper;
            mapper->SetInputConnection(point->GetOutputPort());
            vtkNew<vtkActor> actor;
            actor->SetMapper(mapper);
            actor->GetProperty()->SetColor(1, 0, 0);
            this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);
            std::cout << "Picked value: " << picked[0] << " " << picked[1] << " " << picked[2] << std::endl;
        }
        // Forward events
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }

    double rotate_center[3];
};
vtkStandardNewMacro(MouseInteractorStyle);

} // namespace

VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkRenderingFreeType);

int main(int, char *[]) {
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkCylinderSource> source;
    source->SetResolution(4);
    source->Update();

    vtkNew<vtkWorldPointPicker> worldPointPicker;

    // Create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(source->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());
    vtkNew<vtkAxesActor> axes;

    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("WorldPointPicker");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetPicker(worldPointPicker);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    //vtkNew<vtkInteractorStyleTrackballActor> style;
    vtkNew<MouseInteractorStyle> style;
    renderWindowInteractor->SetInteractorStyle(style);

    // Add the actor to the scene
    renderer->AddActor(actor);
    renderer->AddActor(axes);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    // // vtk Element /////////////////////////////////////////////////////////
    // vtkCamera *camera = renderer->GetActiveCamera();

    // // Camera Parameters ///////////////////////////////////////////////////
    // double *focalPoint = camera->GetFocalPoint();
    // double *viewUp = camera->GetViewUp();
    // double *position = camera->GetPosition();
    // double axis[3];
    // axis[0] = -camera->GetViewTransformMatrix()->GetElement(0, 0);
    // axis[1] = -camera->GetViewTransformMatrix()->GetElement(0, 1);
    // axis[2] = -camera->GetViewTransformMatrix()->GetElement(0, 2);

    // // Build The transformatio /////////////////////////////////////////////////
    // vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    // transform->Identity();

    // transform->Translate(d->center[0], d->center[1], d->center[2]);
    // transform->RotateWXYZ(rxf, viewUp); // Azimuth
    // transform->RotateWXYZ(ryf, axis);   // Elevation
    // transform->Translate(-d->center[0], -d->center[1], -d->center[2]);

    // double newPosition[3];
    // transform->TransformPoint(position, newPosition);     // Transform Position
    // double newFocalPoint[3];
    // transform->TransformPoint(focalPoint, newFocalPoint); // Transform Focal Point

    // camera->SetPosition(newPosition);
    // camera->SetFocalPoint(newFocalPoint);

    // // Orhthogonalize View Up //////////////////////////////////////////////////
    // camera->OrthogonalizeViewUp();
    // renderer->ResetCameraClippingRange();

    // rwi->Render();

    // Render and interact
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}