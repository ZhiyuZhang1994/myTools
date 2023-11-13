/**
 * @brief 在平面两侧都可以显示四边形单元的边线
 * @version 0.1
 * @date 2023-11-13
 */

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkCubeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> namedColors;
  // Visualization
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderer->SetBackground(namedColors->GetColor3d("SlateGray").GetData());

  // Create a cube.
  vtkNew<vtkCubeSource> cubeSource1;
  cubeSource1->SetCenter(5.0, 0.0, 0.0);
  cubeSource1->SetZLength(0.0);
  cubeSource1->SetXLength(5.0);
  cubeSource1->SetYLength(5.0);
  cubeSource1->Update();
  vtkNew<vtkPolyDataMapper> cubeMapper1;
  cubeMapper1->SetInputConnection(cubeSource1->GetOutputPort());
  vtkNew<vtkActor> cubeActor1;
  cubeActor1->SetMapper(cubeMapper1);
  cubeActor1->GetProperty()->SetColor(
      namedColors->GetColor3d("SteelBlue").GetData());
  renderer->AddActor(cubeActor1);

  // Create a cube.
  vtkNew<vtkCubeSource> cubeSource;
  cubeSource->SetCenter(5.0, 0.0, 0);
  cubeSource->SetZLength(0.00001); // 为了让平面两面都可以看到线框
  cubeSource->Update();
  vtkNew<vtkPolyDataMapper> cubeMapper;
  cubeMapper->SetInputConnection(cubeSource->GetOutputPort());
  vtkNew<vtkActor> cubeActor;
  cubeActor->SetMapper(cubeMapper);
  cubeActor->GetProperty()->SetColor(
      namedColors->GetColor3d("Tomato").GetData());
    cubeActor->GetProperty()->SetRepresentationToWireframe();
  renderer->AddActor(cubeActor);



  renderer->ResetCamera();
  renderWindow->SetWindowName("Assembly");
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}