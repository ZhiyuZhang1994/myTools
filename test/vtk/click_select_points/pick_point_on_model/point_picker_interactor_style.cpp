#include "point_picker_interactor_style.h"

//vtkStandardNewMacro只用放在源文件中实现，不用放在头文件中
vtkStandardNewMacro(PointPickerInteractorStyle);
PointPickerInteractorStyle::PointPickerInteractorStyle() {
    this->SelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    this->SelectedActor = vtkSmartPointer<vtkActor>::New();
    this->SelectedActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    this->SelectedActor->SetMapper(mapper);
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

    auto point_picker = vtkCellPicker::SafeDownCast(picker);
    std::cout << point_picker->GetPointId() << std::endl;

    std::cout << "Picked value: " << picked[0] << " " << picked[1] << " " << picked[2] << std::endl;
    if (point_picker->GetPointId() != -1) {
        auto point_pos = points->GetPoint(point_picker->GetPointId());
        vtkSmartPointer<vtkSphereSource> sphereSource =
            vtkSmartPointer<vtkSphereSource>::New();
        sphereSource->SetRadius(0.5);
        sphereSource->Update();
        SelectedMapper->SetInputConnection(sphereSource->GetOutputPort());
        SelectedActor->SetPosition(point_pos);
        SelectedActor->SetScale(0.03);
        this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);

        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    } else {
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
}

void PointPickerInteractorStyle::set_points(vtkSmartPointer<vtkPolyData> points) {
    this->points = points;
}
