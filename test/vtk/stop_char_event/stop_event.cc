#include "tool/StopEvent.h"

#include <vtkActor.h>
#include <vtkAutoInit.h>
#include <vtkCallbackCommand.h>
#include <vtkCylinderSource.h>
#include <vtkDataSetMapper.h>
#include <vtkEvent.h>
#include <vtkEventForwarderCommand.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkWidgetEventTranslator.h>

VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkRenderingFreeType);

int main() {
    vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();
    //renderer1用来观察没有改变焦点位置时的旋转

    //显示一个圆柱体用来作为设置旋转中心点的对象
    vtkSmartPointer<vtkCylinderSource> cylinder = vtkSmartPointer<vtkCylinderSource>::New();
    cylinder->Update();
    auto polydata = cylinder->GetOutputPort();
    vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper->SetInputConnection(polydata);
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("Salmon").GetData());
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
    renderer->AddActor(actor);
    vtkSmartPointer<vtkRenderWindow> render_window = vtkSmartPointer<vtkRenderWindow>::New();
    render_window->AddRenderer(renderer);
    render_window->SetWindowName("StopCenter");
    vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    //vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    vtkSmartPointer<StopEvent> style = vtkSmartPointer<StopEvent>::New();
    interactor->SetInteractorStyle(style);
    // vtkSmartPointer<vtkCallbackCommand> command = vtkSmartPointer<vtkCallbackCommand>::New();
    // //command->AbortFlagOnExecuteOn(); // callback默认空指针，因此不加这句话也行
    // style->AddObserver(vtkCommand::CharEvent, command);
    interactor->SetRenderWindow(render_window);

    render_window->Render();
    interactor->Start();

    return 0;
}