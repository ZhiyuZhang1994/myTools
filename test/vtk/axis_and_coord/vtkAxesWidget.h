/**
 * @brief 用于放置固定位置axes的widget
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2023-08-22
 */

#ifndef GRAPHIC_VTK_WIDGET_VTK_AXES_WIDGET_H
#define GRAPHIC_VTK_WIDGET_VTK_AXES_WIDGET_H

#include "vtkInteractionWidgetsModule.h"
#include "vtkInteractorObserver.h"
#include <vtkOrientationMarkerWidget.h>

#include <limits>
#include <utility>

class vtkAxesWidget : public vtkOrientationMarkerWidget {
public:
    vtkTypeMacro(vtkAxesWidget, vtkOrientationMarkerWidget);
    static vtkAxesWidget* New();

    void setAutomaticAdjustPosition();

protected:
    vtkAxesWidget();
    ~vtkAxesWidget() override;

private:
    void adjustAxesWidgetPosition(vtkObject*, unsigned long, void* calldata);

    vtkAxesWidget(const vtkAxesWidget&) = delete;
    void operator=(const vtkAxesWidget&) = delete;
    std::uint32_t repositionObserverId_ = std::numeric_limits<std::uint32_t>::max();
    float fixSize_ = 60; // 固定60px
};

#endif
