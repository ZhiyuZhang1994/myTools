/**
 * @brief 利用vtkAnimationCue与vtkAnimationScene结合实现高级动画
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2024-05-21
 */

#include "AnimateActors.h"

#include <vtkAnimationCue.h>
#include <vtkAnimationScene.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkConeSource.h>
#include <vtkLogger.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int argc, char* argv[])
{
  vtkLogger::Init(argc, argv);

  // Colors
  vtkNew<vtkNamedColors> colors;
  vtkColor3d coneColor = colors->GetColor3d("Tomato");
  vtkColor3d sphereColor = colors->GetColor3d("Banana");
  vtkColor3d backgroundColor = colors->GetColor3d("Peacock");

  // Create the graphics structure. The renderer renders into the
  // render window.
  vtkNew<vtkRenderWindowInteractor> iren;
  vtkNew<vtkRenderer> ren1;
  ren1->SetBackground(backgroundColor.GetData());

  vtkNew<vtkRenderWindow> renWin;
  iren->SetRenderWindow(renWin);
  renWin->AddRenderer(ren1);

  // Generate a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetPhiResolution(31);
  sphereSource->SetThetaResolution(31);

  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> sphere;
  sphere->SetMapper(sphereMapper);
  sphere->GetProperty()->SetDiffuseColor(sphereColor.GetData());
  sphere->GetProperty()->SetDiffuse(.7);
  sphere->GetProperty()->SetSpecular(.3);
  sphere->GetProperty()->SetSpecularPower(30.0);

  ren1->AddActor(sphere);

  // Generate a cone
  vtkNew<vtkConeSource> coneSource;
  coneSource->SetResolution(31);

  vtkNew<vtkPolyDataMapper> coneMapper;
  coneMapper->SetInputConnection(coneSource->GetOutputPort());
  // auto cone = vtkSmartPointer<vtkActor>::New();
  vtkNew<vtkActor> cone;
  cone->SetMapper(coneMapper);
  cone->GetProperty()->SetDiffuseColor(coneColor.GetData());

  ren1->AddActor(cone);

  // Create an Animation Scene
  vtkNew<vtkAnimationScene> scene;
  if (argc >= 2 && strcmp(argv[1], "-real") == 0)
  {
    vtkLogF(INFO, "real-time mode");
    scene->SetModeToRealTime();
  }
  else
  {
    vtkLogF(INFO, "sequence mode");
    scene->SetModeToSequence();
  }
  scene->SetLoop(0);
  scene->SetFrameRate(30);
  scene->SetStartTime(20);
  scene->SetEndTime(200);
  scene->AddObserver(vtkCommand::AnimationCueTickEvent, renWin.GetPointer(),&vtkWindow::Render);
                   //为什么额外需要scene观察

  // Create an Animation Cue for each actor
  vtkNew<vtkAnimationCue> cue1;
  cue1 ->SetTimeModeToNormalized();
  cue1->SetStartTime(0);
  cue1->SetEndTime(1.0);
  //cue1 ->SetTimeModeToNormalized();
  std::cout << cue1->GetTimeMode() << std::endl;
  scene->AddCue(cue1);

  vtkNew<vtkAnimationCue> cue2;
  cue2->SetStartTime(100);
  cue2->SetEndTime(300);
  scene->AddCue(cue2);

  // Create an ActorAnimator for each actor;
  ActorAnimator animateSphere;
  animateSphere.SetActor(sphere);
  animateSphere.AddObserversToCue(cue1);

  ActorAnimator animateCone;
  animateCone.SetEndPosition(vtkVector3d(-1, -1, -1));//s
  animateCone.SetActor(cone);
  animateCone.AddObserversToCue(cue2);

  renWin->SetWindowName("AnimateActors");

  renWin->Render();
  ren1->ResetCamera();
  ren1->GetActiveCamera()->Dolly(.5);
  ren1->ResetCameraClippingRange();

  // Create Cue observer.
  scene->Play();
  scene->Stop();

  iren->Start();
  return EXIT_SUCCESS;
}
