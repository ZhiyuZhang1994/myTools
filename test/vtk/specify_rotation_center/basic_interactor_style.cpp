/**
 * @brief 旋转scene的交互类型
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2023-11-15
 */

#include "basic_interactor_style.h"
#include <vtkCallbackCommand.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTypeUInt32Array.h>
#include <vtkViewport.h>
#include <vtkProperty.h>
#include <vtkCoordinate.h>
#include <vtkCellPicker.h>
#include <vtkRendererCollection.h>
#include <vtkTransform.h>
#include <vtkCamera.h>

vtkStandardNewMacro(BasicInteractorStyle);

BasicInteractorStyle::BasicInteractorStyle() : vtkInteractorStyleRubberBandPick() {
    point_ = vtkSmartPointer<vtkSphereSource>::New();
    point_->SetRadius(0.02);
    mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper_->SetInputConnection(point_->GetOutputPort());
    actor_ = vtkSmartPointer<vtkActor>::New();
    actor_->GetProperty()->SetColor(1, 0, 0);
    actor_->SetMapper(mapper_);
}

///ctrl+左键：旋转
void BasicInteractorStyle::OnLeftButtonDown() {
    int* pos = this->Interactor->GetEventPosition();
    leftButtonPressedPos_[0] = pos[0];
    leftButtonPressedPos_[1] = pos[1];
    buttonDown(ROTATE);
}

void BasicInteractorStyle::OnLeftButtonUp() {
    vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
    setUserDefinedRotateCenter();
}

///ctrl+右键：平移
void BasicInteractorStyle::OnRightButtonDown() {
    buttonDown(PAN);
}

void BasicInteractorStyle::OnRightButtonUp() {
    vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
}

///ctrl+中键：没响应
void BasicInteractorStyle::OnMiddleButtonDown() {
    return;
}

void BasicInteractorStyle::OnMiddleButtonUp() {
    return;
}

void BasicInteractorStyle::OnMouseMove() {
    this->Superclass::OnMouseMove();
    vtkRenderWindowInteractor* rwi = this->Interactor;
    switch (this->State) {
        case VTKIS_ROTATE:
            if (picked) {
                rwi->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor_);
            }
            break;

        case VTKIS_PAN:
            rwi->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->RemoveActor(actor_);
            break;
    }
    rwi->Render();
}

void BasicInteractorStyle::OnMouseWheelForward() {
    vtkRenderWindowInteractor* rwi = this->Interactor;
    rwi->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->RemoveActor(actor_);
    /// 滚轮上滑缩小
    if (this->Interactor->GetControlKey()) {
        Superclass::OnMouseWheelBackward();
    }
}

void BasicInteractorStyle::OnMouseWheelBackward() {
    vtkRenderWindowInteractor* rwi = this->Interactor;
    rwi->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->RemoveActor(actor_);
    /// 滚轮下滑放大
    if (this->Interactor->GetControlKey()) {
        Superclass::OnMouseWheelForward();
    }
}

void BasicInteractorStyle::OnChar() {
    vtkRenderWindowInteractor* rwi = this->Interactor;
    switch (rwi->GetKeyCode()) {
        case 'w':
        case 'W':
        case 's':
        case 'S':
        case 'r':
        case 'R':
            break;

        default:
            this->Superclass::OnChar();
            break;
    }
}

void BasicInteractorStyle::Rotate() {
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
    transform->Translate(this->pickedPos_[0], this->pickedPos_[1], this->pickedPos_[2]);
    transform->RotateWXYZ(rxf, viewUp);
    transform->RotateWXYZ(ryf, axis);
    transform->Translate(-this->pickedPos_[0], -this->pickedPos_[1], -this->pickedPos_[2]);
    double newPosition[3];
    transform->TransformPoint(position, newPosition);
    double newFocalPoint[3];
    transform->TransformPoint(focalPoint, newFocalPoint);

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

void BasicInteractorStyle::OnKeyPress() {
    std::cout << "OnKeyPress " << controlPressed_ << std::endl;
    if (this->Interactor->GetControlKey()) {
        controlPress();
    }
}

void BasicInteractorStyle::controlPress() {
    controlPressed_ = 1;
    std::cout << "controlPress "<< std::endl;
}

void BasicInteractorStyle::OnKeyRelease() {
    std::cout << "OnKeyRelease " << controlPressed_ << std::endl;
    if (controlPressed_&& !this->Interactor->GetControlKey() ) {
        controlRelease();
    }
}

void BasicInteractorStyle::controlRelease() {
    controlPressed_ = 0;
    std::cout << "controlRelease"<< std::endl;
    vtkRenderWindowInteractor* rwi = this->Interactor;
    rwi->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->RemoveActor(actor_);
    rwi->Render();
}

void BasicInteractorStyle::setDataSource(vtkSmartPointer<vtkDataSet> dataSource) {
    dataSource_ = dataSource;
    setDefaultRotationCenter();
}

void BasicInteractorStyle::setDefaultRotationCenter() {
    vtkRenderWindowInteractor* rwi = this->Interactor;
    rwi->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->RemoveActor(actor_);

    this->FindPokedRenderer(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);
    if (this->CurrentRenderer == nullptr) {
        return;
    }
    double allBounds[6];
    this->CurrentRenderer->ComputeVisiblePropBounds(allBounds);
    pickedPos_[0] = (allBounds[0] + allBounds[1]) / 2;
    pickedPos_[1] = (allBounds[2] + allBounds[3]) / 2;
    pickedPos_[2] = (allBounds[4] + allBounds[5]) / 2;
    picked = false;
}

void BasicInteractorStyle::setUserDefinedRotateCenter() {
    // 如果未导入数据则返回
    if (!dataSource_) {
        std::cout << "not load dataSource_" << std::endl;
        return;
    }

    // 是否按着ctrl键
    if (!this->Interactor->GetControlKey()) {
        std::cout << "not press control" << std::endl;
        return;
    }

    // 释放时左键位置是否移动
    vtkRenderWindowInteractor* rwi = this->Interactor;
    int* pos = rwi->GetEventPosition();

    int dx = pos[0] - leftButtonPressedPos_[0];
    int dy = pos[1] - leftButtonPressedPos_[1];
    if (dx * dx + dy * dy > clickTolerate_ * clickTolerate_) { // 像素距离
        std::cout << "not same position" << std::endl;
        return;
    }

    // 是否拾取到模型上点
    vtkNew<vtkCellPicker> objectPicker;
    objectPicker->Pick(pos[0], pos[1], 0, this->GetDefaultRenderer());
    auto vtkPointID = objectPicker->GetPointId();
    if (vtkPointID == -1) {
        // 恢复原始旋转中心
        std::cout << "out point" << std::endl;
        setDefaultRotationCenter();
        return;
    }

    // 拾取的点是否在模型节点附近
    dataSource_->GetPoint(vtkPointID, pickedPos_);
    if (!isAdjacent(pickedPos_, pos)) {
        std::cout << "not in field" << std::endl;
        return;
    }

    // 创建旋转中心图例
    std::cout << "create sphere" << std::endl;
    actor_->SetPosition(pickedPos_);
    rwi->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor_);
    rwi->Render();
    picked = true;
}

bool BasicInteractorStyle::isAdjacent(double* pointPos, const int* pos) {
    this->FindPokedRenderer(
        this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);
    vtkViewport* viewPort = this->CurrentRenderer;
    vtkSmartPointer<vtkCoordinate> pCoord = vtkSmartPointer<vtkCoordinate>::New();
    pCoord->SetCoordinateSystemToWorld();
    pCoord->SetValue(pointPos);
    auto dispCoord = pCoord->GetComputedDisplayValue(viewPort);
    double dist = (dispCoord[0] - pos[0]) * (dispCoord[0] - pos[0]) + (dispCoord[1] - pos[1]) * (dispCoord[1] - pos[1]);
    return clickTolerate_ * clickTolerate_ > dist;
}

void BasicInteractorStyle::buttonDown(ControlMode mode) {
    this->FindPokedRenderer(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);
    if (this->CurrentRenderer == nullptr) {
        return;
    }

    if (this->Interactor->GetControlKey()) {
        this->GrabFocus(this->EventCallbackCommand);
        switch (mode) {
            case ROTATE: {
                this->StartRotate();
                break;
            }
            case PAN: {
                this->StartPan();
                break;
            }
        }
    }
}
