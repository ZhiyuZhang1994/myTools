#ifndef CAE_INCLUDE_TOOL_STOP_EVENT_H
#define CAE_INCLUDE_TOOL_STOP_EVENT_H

#include <vtkAbstractPicker.h>
#include <vtkAbstractPropPicker.h>
#include <vtkAssemblyPath.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

class StopEvent : public vtkInteractorStyleTrackballCamera {
public:
    static StopEvent* New();
    vtkTypeMacro(StopEvent, vtkInteractorStyleTrackballCamera);

    StopEvent();

    void CallbackFunc();
};

#endif //CAE_INCLUDE_TOOL_STOP_EVENT_H