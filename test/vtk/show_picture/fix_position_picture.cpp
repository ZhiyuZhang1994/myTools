/**
 * @file 固定vtk中2DActor的位置不随窗口缩放变化
 * @brief 参考资料：https://github.com/marcomusy/vedo/issues/474
 * @date 2023-08-21
 */

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
    // renderer->AddActor(axis);

    vtkSmartPointer<vtkOrientationMarkerWidget> axisWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    axisWidget->SetOrientationMarker(axis);
    axisWidget->SetInteractor(renderWindow->GetInteractor());
    axisWidget->SetEnabled(true);            // 显示与否
    axisWidget->SetInteractive(false);       // 不允许拖动
    axisWidget->SetViewport(0, 0, 0.2, 0.2); // 相对位置
                                             // 问题：绝对位置的接口如何设置？？？

    // 显示图片
    std::string inputFilename = "D:\\work_station\\github_project\\func_test\\src\\befem-studio.png";

    // Read the image
    vtkNew<vtkPNGReader> reader;
    reader->SetFileName(inputFilename.c_str());
    vtkSmartPointer<vtkImageMapper> mapper = vtkSmartPointer<vtkImageMapper>::New();
    mapper->SetInputConnection(reader->GetOutputPort());
    mapper->SetColorWindow(255.0); // 解决VTK bug：图片颜是灰色的问题
    mapper->SetColorLevel(127.5);  // 解决VTK bug：图片颜是灰色的问题

    vtkSmartPointer<vtkActor2D> actor = vtkSmartPointer<vtkActor2D>::New();
    actor->SetMapper(mapper);

    vtkSmartPointer<vtkCoordinate> pCoord1 = vtkSmartPointer<vtkCoordinate>::New();
    pCoord1->SetCoordinateSystemToNormalizedViewport();
    pCoord1->SetValue(1, 1);
    actor->GetPositionCoordinate()->SetCoordinateSystemToViewport();
    actor->GetPositionCoordinate()->SetReferenceCoordinate(pCoord1);

    actor->SetPosition(-60, -60); // 图片像素为60*60px
    renderer->AddActor(actor);
    // ————————————————————————————————渲染开始
    renderer->ResetCamera();
    renderWindow->Render();
    renderWindowInteractor->Start();
    return EXIT_SUCCESS;
}