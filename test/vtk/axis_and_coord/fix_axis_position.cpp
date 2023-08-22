/**
 * @brief 用于放置固定位置axes的widget
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2023-08-22
 */

#include "vtkAxesWidget.h"
#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkCaptionActor2D.h>
#include <vtkFollower.h>
#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageGridSource.h>
#include <vtkImageMapper.h>
#include <vtkImageMapper3D.h>
#include <vtkImageViewer2.h>
#include <vtkInteractorStyleImage.h>
#include <vtkJPEGReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPNGReader.h>
#include <vtkPlaneSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTexture.h>
#include <vtkTextureMapToPlane.h>
#include <vtkTransform.h>


int main(int, char*[]) {
    // ————————————————————————————————初始设置操作
    vtkNew<vtkNamedColors> colors;
    // a renderer and render window
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetWindowName("Axes");
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(300, 300);
    // an interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // ————————————————————————————————图显开始
    // add the actors to the scene
    vtkSmartPointer<vtkAxesActor> axis = vtkSmartPointer<vtkAxesActor>::New();
    axis->GetXAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
    axis->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(10);
    axis->GetYAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
    axis->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(10);
    axis->GetZAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
    axis->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(10);
    axis->SetShaftTypeToCylinder();
    axis->SetCylinderRadius(0.5 * axis->GetCylinderRadius());
    renderer->AddActor(axis);

    vtkSmartPointer<vtkAxesWidget> axisWidget = vtkSmartPointer<vtkAxesWidget>::New();
    axisWidget->SetOrientationMarker(axis);
    axisWidget->SetInteractor(renderWindow->GetInteractor());
    axisWidget->SetEnabled(true);            // 显示与否
    axisWidget->SetInteractive(false);       // 不允许拖动
    axisWidget->setAutomaticAdjustPosition();

    // ————————————————————————————————渲染开始
    renderer->ResetCamera();
    renderWindow->Render();
    renderWindowInteractor->Start();
    return EXIT_SUCCESS;
}
