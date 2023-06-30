#include "tool/point_picker.h"

//vtkStandardNewMacro只用放在源文件中实现，不用放在头文件中
vtkStandardNewMacro(PointPickerInteractorStyle);
PointPickerInteractorStyle::PointPickerInteractorStyle() {
    // this->mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    //this->actor = vtkSmartPointer<vtkActor>::New();
    // this->actor->GetProperty()->SetColor(0.0, 1.0, 0.0);
    // this->actor->SetMapper(mapper);
}

//覆盖了vtkInteractorStyleTrackballCamera类下的OnLeftButtonDown()函数
void PointPickerInteractorStyle::OnLeftButtonDown() {
    auto picker = this->Interactor->GetPicker();
    picker->Pick(this->Interactor->GetEventPosition()[0],
                 this->Interactor->GetEventPosition()[1],
                 0, // always zero.
                 this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
    double picked[3];
    picker->GetPickPosition(picked);

    auto point_picker = vtkPointPicker::SafeDownCast(picker);
    std::cout << point_picker->GetPointId() << std::endl;

    //std::cout << "Picked value: " << picked[0] << " " << picked[1] << " " << picked[2] << std::endl;
    if (point_picker->GetPointId() != -1) {
        auto point_pos = points->GetPoint(point_picker->GetPointId());
        vtkSmartPointer<vtkSphereSource> sphereSource =
            vtkSmartPointer<vtkSphereSource>::New();
        sphereSource->SetRadius(0.5);
        sphereSource->Update();
        auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(sphereSource->GetOutputPort());
        auto actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        actor->SetPosition(picked);
        actor->SetScale(0.03);
        actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
        this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);

        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    } else {
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
}

void PointPickerInteractorStyle::set_points(vtkSmartPointer<vtkPolyData> points) {
    this->points = points;
}
