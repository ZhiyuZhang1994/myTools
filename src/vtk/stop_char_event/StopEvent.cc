#include "tool/StopEvent.h"

vtkStandardNewMacro(StopEvent);

StopEvent::StopEvent() {
    vtkSmartPointer<vtkCallbackCommand> command = vtkSmartPointer<vtkCallbackCommand>::New();
    //command->AbortFlagOn();
    this->AddObserver(vtkCommand::CharEvent, this, &StopEvent::CallbackFunc);
    //command->SetCallback(CallbackFunc)
}

void StopEvent::CallbackFunc() {
    vtkRenderWindowInteractor* rwi = this->GetInteractor();

    switch (rwi->GetKeyCode()) {
    case 'm':
    case 'M':
        if (this->AnimState == VTKIS_ANIM_OFF) {
            this->StartAnimate();
        } else {
            this->StopAnimate();
        }
        break;

    case 'Q':
    case 'q':
    case 'e':
    case 'E':
        rwi->ExitCallback();
        break;

    case 'f':
    case 'F': {
        if (this->CurrentRenderer != nullptr) {
            this->AnimState = VTKIS_ANIM_ON;
            vtkAssemblyPath* path = nullptr;
            this->FindPokedRenderer(rwi->GetEventPosition()[0], rwi->GetEventPosition()[1]);
            rwi->GetPicker()->Pick(
                rwi->GetEventPosition()[0], rwi->GetEventPosition()[1], 0.0, this->CurrentRenderer);
            vtkAbstractPropPicker* picker;
            if ((picker = vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker()))) {
                path = picker->GetPath();
            }
            if (path != nullptr) {
                rwi->FlyTo(this->CurrentRenderer, picker->GetPickPosition());
            }
            this->AnimState = VTKIS_ANIM_OFF;
        } else {
            vtkWarningMacro(<< "no current renderer on the interactor style.");
        }
    } break;

    case 'u':
    case 'U':
        rwi->UserCallback();
        break;

    case 'r':
    case 'R':
        this->FindPokedRenderer(rwi->GetEventPosition()[0], rwi->GetEventPosition()[1]);
        if (this->CurrentRenderer != nullptr) {
            this->CurrentRenderer->ResetCamera();
        } else {
            vtkWarningMacro(<< "no current renderer on the interactor style.");
        }
        rwi->Render();
        break;

    case 'w':
    case 'W':
        break;

    case 's':
    case 'S':
        break;

    case '3':
        if (rwi->GetRenderWindow()->GetStereoRender()) {
            rwi->GetRenderWindow()->StereoRenderOff();
        } else {
            rwi->GetRenderWindow()->StereoRenderOn();
        }
        rwi->Render();
        break;

    case 'p':
    case 'P':
        if (this->CurrentRenderer != nullptr) {
            if (this->State == VTKIS_NONE) {
                vtkAssemblyPath* path = nullptr;
                int* eventPos = rwi->GetEventPosition();
                this->FindPokedRenderer(eventPos[0], eventPos[1]);
                rwi->StartPickCallback();
                vtkAbstractPropPicker* picker = vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker());
                if (picker != nullptr) {
                    picker->Pick(eventPos[0], eventPos[1], 0.0, this->CurrentRenderer);
                    path = picker->GetPath();
                }
                if (path == nullptr) {
                    this->HighlightProp(nullptr);
                    this->PropPicked = 0;
                } else {
                    this->HighlightProp(path->GetFirstNode()->GetViewProp());
                    this->PropPicked = 1;
                }
                rwi->EndPickCallback();
            }
        } else {
            vtkWarningMacro(<< "no current renderer on the interactor style.");
        }
        break;
    }
}
