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


class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera {
public:
    static MouseInteractorStyle* New();
    vtkTypeMacro(MouseInteractorStyle, vtkInteractorStyleTrackballCamera);

    MouseInteractorStyle() {
        point = vtkSmartPointer<vtkSphereSource>::New();
        point->SetRadius(0.02);
        mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(point->GetOutputPort());
        actor = vtkSmartPointer<vtkActor>::New();
        actor->GetProperty()->SetColor(1, 0, 0);
        actor->SetMapper(mapper);
    }

    virtual void OnLeftButtonDown() override {
        if (this->Interactor->GetAltKey()) {
            int* pos = this->GetInteractor()->GetEventPosition();
            std::cout << "Picking pixel: " << pos[0] << " " << pos[1] << std::endl;
            auto picker = this->Interactor->GetPicker();
            picker->Pick(pos[0], pos[1], 0, this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

            this->Interactor->GetPicker()->GetPickPosition(picked);
            actor->SetPosition(picked);
            this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);
            std::cout << "Picked value: " << picked[0] << " " << picked[1] << " " << picked[2] << std::endl;
        }
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }

    virtual void Rotate() override {
        if (this->CurrentRenderer == nullptr) {
            return;
        }
        vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
        double position[3];
        double focalPoint[3];
        double viewUp[3];
        camera->GetPosition(position);
        camera->GetFocalPoint(focalPoint);
        camera->GetViewUp(viewUp);
        double axis[3];
        axis[0] = -camera->GetViewTransformMatrix()->GetElement(0, 0);
        axis[1] = -camera->GetViewTransformMatrix()->GetElement(0, 1);
        axis[2] = -camera->GetViewTransformMatrix()->GetElement(0, 2);
        vtkRenderWindowInteractor* rwi = this->Interactor;
        int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
        int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
        const int* size = this->CurrentRenderer->GetRenderWindow()->GetSize();
        double delta_elevation = -20.0 / size[1];
        double delta_azimuth = -20.0 / size[0];
        double rxf = dx * delta_azimuth * this->MotionFactor;
        double ryf = dy * delta_elevation * this->MotionFactor;
    
        vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
        transform->Identity();
        transform->Translate(this->picked[0], this->picked[1], this->picked[2]);
        transform->RotateWXYZ(rxf, viewUp); // Azimuth
        transform->RotateWXYZ(ryf, axis);   // Elevation
        transform->Translate(-this->picked[0], -this->picked[1], -this->picked[2]);
        double newPosition[3];
        transform->TransformPoint(position, newPosition); // Transform Position
        double newFocalPoint[3];
        transform->TransformPoint(focalPoint, newFocalPoint); // Transform Focal Point

        camera->SetPosition(newPosition);
        camera->SetFocalPoint(newFocalPoint);
        camera->OrthogonalizeViewUp();

        if (this->AutoAdjustCameraClippingRange) {
            this->CurrentRenderer->ResetCameraClippingRange();
        }
        if (rwi->GetLightFollowCamera()) {
            this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
        }
        rwi->Render();
    }

private:
    vtkSmartPointer<vtkSphereSource> point = nullptr;
    vtkSmartPointer<vtkPolyDataMapper>mapper = nullptr;
    vtkSmartPointer<vtkActor> actor = nullptr;
    double picked[3];
};

vtkStandardNewMacro(MouseInteractorStyle);

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

    vtkNew<MouseInteractorStyle> style;
    renderWindowInteractor->SetInteractorStyle(style);
    std::cout << style->GetState() << std::endl;

    // Render and interact
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}