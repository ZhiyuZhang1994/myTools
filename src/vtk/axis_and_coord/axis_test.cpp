#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkCaptionActor2D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>


namespace {
// Define interaction style
class myVtkAxesActor : public vtkAxesActor
{
public:
  static myVtkAxesActor* New();
  vtkTypeMacro(myVtkAxesActor, vtkAxesActor);
  int RenderOpaqueGeometry(vtkViewport* viewport) override;

};

int myVtkAxesActor::RenderOpaqueGeometry(vtkViewport* vp) {
    int renderedSomething = vtkAxesActor::RenderOpaqueGeometry(vp);
    this->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(1);
    this->XAxisLabel->RenderOpaqueGeometry(vp);
    captionActor->GetCaptionTextProperty()
    std::cout << "zzz111111111" << std::endl;
    return renderedSomething;
}

vtkStandardNewMacro(myVtkAxesActor);
} // namespace


int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetCenter(0.0, 0.0, 0.0);
    sphereSource->SetRadius(1);

    // create a mapper
    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphereSource->GetOutputPort());

    // create an actor
    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper);

    // a renderer and render window
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetWindowName("Axes");
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(300, 300);

    // an interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // add the actors to the scene
    //   renderer->AddActor(sphereActor);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());


    vtkNew<myVtkAxesActor> axes;
    vtkNew<vtkAxesActor> axes1;

    vtkNew<vtkTransform> transform;
    transform->Translate(1.0, 0.0, 0.0);
    axes->SetUserTransform(transform);
    axes->SetTotalLength(0.1,0.1,0.1);

    vtkTextProperty* tprop = axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty();
    tprop->SetFontSize(5);

    axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->ShallowCopy(tprop);
    axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->ShallowCopy(tprop);

    renderer->AddActor(axes);
    renderer->AddActor(axes1);

    renderer->ResetCamera();
    renderWindow->SetWindowName("Axes");
    renderWindow->Render();

    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!origin!!!!!!!!!!!!!!!!!!!" << std::endl;
    // axes1->PrintSelf(std::cout,vtkIndent(2));

    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!moved!!!!!!!!!!!!!!!!!!!" << std::endl;
    axes->PrintSelf(std::cout,vtkIndent(2));

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}