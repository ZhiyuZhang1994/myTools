/**
 * @brief 用于放置固定位置axes的widget
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2023-08-22
 */

#include "vtkAxesWidget.h"

#include "vtkCallbackCommand.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include <iostream>
#include <vtkObjectFactory.h>

vtkStandardNewMacro(vtkAxesWidget);

vtkAxesWidget::vtkAxesWidget() {
}

vtkAxesWidget::~vtkAxesWidget() {
    if (this->Interactor) {
        auto renWin = this->Interactor->GetRenderWindow();
        renWin->RemoveObserver(repositionObserverId_);
    }
}

void vtkAxesWidget::setAutomaticAdjustPosition() {
    if (!this->Interactor) {
        return;
    }
    auto renWin = this->Interactor->GetRenderWindow();
    repositionObserverId_ = renWin->AddObserver(vtkCommand::WindowResizeEvent, this, &vtkAxesWidget::adjustAxesWidgetPosition);
    adjustAxesWidgetPosition(renWin, 0, nullptr);
}

void vtkAxesWidget::adjustAxesWidgetPosition(vtkObject* caller, unsigned long, void*) {
    vtkRenderWindow* renWin = reinterpret_cast<vtkRenderWindow*>(caller);
    int* size = renWin->GetSize();
    SetViewport(0, 0, fixSize_ / size[0], fixSize_ / size[1]);

}