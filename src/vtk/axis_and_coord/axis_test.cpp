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
  vtkNew<vtkAxesActor> axes;
  vtkNew<vtkAxesActor> axes1;

//   vtkNew<vtkTransform> transform;
//   transform->Translate(1.0, 0.0, 0.0);
//   transform->RotateX(40);
//   axes->SetUserTransform(transform);
  axes->SetScale(2);
  axes->SetTotalLength(0.1,0.1,0.1);
  renderer->AddActor(axes);
  renderer->AddActor(axes1);

  renderer->ResetCamera();
  renderWindow->SetWindowName("Axes");
  renderWindow->Render();
  
  std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!origin!!!!!!!!!!!!!!!!!!!" << std::endl;
  axes1->PrintSelf(std::cout,vtkIndent(2));

  std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!moved!!!!!!!!!!!!!!!!!!!" << std::endl;
  axes->PrintSelf(std::cout,vtkIndent(2));

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}