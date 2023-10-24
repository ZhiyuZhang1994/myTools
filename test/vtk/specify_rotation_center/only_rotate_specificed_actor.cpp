/**
 * @brief 旋转scene中特定actor的方案
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2023-10-20
 */

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
class MouseInteractorStyle : public vtkInteractorStyleTrackballActor {
public:
    static MouseInteractorStyle *New();
    vtkTypeMacro(MouseInteractorStyle, vtkInteractorStyleTrackballActor);

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
        vtkInteractorStyleTrackballActor::OnLeftButtonDown();
    }

    virtual void Rotate() override {
        if (this->CurrentRenderer == nullptr || this->InteractionProp == nullptr) {
            return;
        }

        // if (this->CurrentRenderer == nullptr) {
        //     return;
        // }

        vtkRenderWindowInteractor *rwi = this->Interactor;
        vtkCamera *cam = this->CurrentRenderer->GetActiveCamera();

        auto bounds = this->InteractionProp->GetBounds();
        double obj_center[3];
        if (this->rotate_center[0] >= bounds[0] && this->rotate_center[0] <= bounds[1] &&
            this->rotate_center[1] >= bounds[2] && this->rotate_center[1] <= bounds[3] &&
            this->rotate_center[2] >= bounds[4] && this->rotate_center[2] <= bounds[5]) {
            for (int i = 0; i < 3; i++) {
                obj_center[i] = this->rotate_center[i];
            }
        } else {
            for (int i = 0; i < 3; i++) {
                obj_center[i] = this->InteractionProp->GetCenter()[i];
            }
        }
        // First get the origin of the assembly
        //double *obj_center = this->InteractionProp->GetCenter();

        // GetLength gets the length of the diagonal of the bounding box
        double boundRadius = this->InteractionProp->GetLength() * 0.5;

        // Get the view up and view right vectors
        double view_up[3], view_look[3], view_right[3];

        cam->OrthogonalizeViewUp();
        cam->ComputeViewPlaneNormal();
        cam->GetViewUp(view_up);
        vtkMath::Normalize(view_up);
        cam->GetViewPlaneNormal(view_look);
        vtkMath::Cross(view_up, view_look, view_right);
        vtkMath::Normalize(view_right);

        // Get the furtherest point from object position+origin
        double outsidept[3];

        outsidept[0] = obj_center[0] + view_right[0] * boundRadius;
        outsidept[1] = obj_center[1] + view_right[1] * boundRadius;
        outsidept[2] = obj_center[2] + view_right[2] * boundRadius;

        // Convert them to display coord
        double disp_obj_center[3];

        this->ComputeWorldToDisplay(obj_center[0], obj_center[1], obj_center[2], disp_obj_center);

        this->ComputeWorldToDisplay(outsidept[0], outsidept[1], outsidept[2], outsidept);

        double radius = sqrt(vtkMath::Distance2BetweenPoints(disp_obj_center, outsidept));
        double nxf = (rwi->GetEventPosition()[0] - disp_obj_center[0]) / radius;

        double nyf = (rwi->GetEventPosition()[1] - disp_obj_center[1]) / radius;

        double oxf = (rwi->GetLastEventPosition()[0] - disp_obj_center[0]) / radius;

        double oyf = (rwi->GetLastEventPosition()[1] - disp_obj_center[1]) / radius;

        if (((nxf * nxf + nyf * nyf) <= 1.0) && ((oxf * oxf + oyf * oyf) <= 1.0)) {
            double newXAngle = vtkMath::DegreesFromRadians(asin(nxf));
            double newYAngle = vtkMath::DegreesFromRadians(asin(nyf));
            double oldXAngle = vtkMath::DegreesFromRadians(asin(oxf));
            double oldYAngle = vtkMath::DegreesFromRadians(asin(oyf));

            double scale[3];
            scale[0] = scale[1] = scale[2] = 1.0;

            double **rotate = new double *[2];

            rotate[0] = new double[4];
            rotate[1] = new double[4];

            rotate[0][0] = newXAngle - oldXAngle;
            rotate[0][1] = view_up[0];
            rotate[0][2] = view_up[1];
            rotate[0][3] = view_up[2];

            rotate[1][0] = oldYAngle - newYAngle;
            rotate[1][1] = view_right[0];
            rotate[1][2] = view_right[1];
            rotate[1][3] = view_right[2];

            this->Prop3DTransform(this->InteractionProp, obj_center, 2, rotate, scale);

            delete[] rotate[0];
            delete[] rotate[1];
            delete[] rotate;

            if (this->AutoAdjustCameraClippingRange) {
                this->CurrentRenderer->ResetCameraClippingRange();
            }

            rwi->Render();
        }
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