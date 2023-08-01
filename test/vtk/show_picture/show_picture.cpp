/**
 * @brief 在固定位置显示固定大小的图片,不允许旋转缩放
 * @author zhangzhiyu
 * @date 2023-01-05
 */

#include <vtkActor2D.h>
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
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTexture.h>
#include <vtkTextureMapToPlane.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

 
int main() {
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkNew<vtkNamedColors> colors;
    std::string inputFilename = "D:\\work_station\\befem_work_station\\befem-studio\\share\\befem-studio1.png";

    // Read the image
    vtkNew<vtkPNGReader> reader;
    reader->SetFileName(inputFilename.c_str());

    // 方案一：不可行————vtkImageViewer2接管了所有渲染流程，不兼容其他actor显示
    //   vtkNew<vtkImageViewer2> imageViewer;
    //   imageViewer->SetInputConnection(reader->GetOutputPort());
    //   vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    //   imageViewer->SetupInteractor(renderWindowInteractor);
    //   imageViewer->Render();
    //   imageViewer->GetRenderer()->ResetCamera();
    //   imageViewer->GetRenderer()->SetBackground(
    //       colors->GetColor3d("DarkSlateGray").GetData());
    //   imageViewer->GetRenderWindow()->SetWindowName("PNGReader");
    //   imageViewer->Render();

    // 方案二：不可行vtkImageActor显示图片-----图片大小可以缩放旋转
    // // Create an actor
    // vtkNew<vtkImageActor> actor;
    // actor->GetMapper()->SetInputConnection(reader->GetOutputPort());

    // 方案三：采用vtkFollower解决旋转问题,采用vtkOrientationMarkerWidget解决缩放问题
    // // Create a plane
    // vtkSmartPointer<vtkPlaneSource> plane = vtkSmartPointer<vtkPlaneSource>::New();
    // plane->SetCenter(0.0, 0.0, 0.0);
    // plane->SetNormal(0.0, 0.0, 1.0);
    // // Apply the texture
    // vtkSmartPointer<vtkTexture> texture = vtkSmartPointer<vtkTexture>::New();
    // texture->SetInputConnection(reader->GetOutputPort());
    // vtkSmartPointer<vtkTextureMapToPlane> texturePlane = vtkSmartPointer<vtkTextureMapToPlane>::New();
    // texturePlane->SetInputConnection(plane->GetOutputPort());
    // vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    // planeMapper->SetInputConnection(texturePlane->GetOutputPort());
    // vtkSmartPointer<vtkFollower> texturedPlane = vtkSmartPointer<vtkFollower>::New();
    // texturedPlane->SetMapper(planeMapper);
    // texturedPlane->SetTexture(texture);
    // renderer->AddActor(texturedPlane); // 作为对比
    // texturedPlane->SetCamera(renderer->GetActiveCamera()); // 解决旋转问题

    // 方案四：采用vtkImageMapper显示二维图片，显示像素大小
    vtkSmartPointer<vtkImageMapper> mapper = vtkSmartPointer<vtkImageMapper>::New();
    mapper->SetInputConnection(reader->GetOutputPort());
    mapper->SetColorWindow(255.0); // 解决VTK bug：图片颜是灰色的问题
    mapper->SetColorLevel(127.5); // 解决VTK bug：图片颜是灰色的问题

    vtkSmartPointer<vtkActor2D> actor = vtkSmartPointer<vtkActor2D>::New();
    actor->SetMapper(mapper);
    // 位置设置方法一：设置全局坐标系绝对位置：像素距离
    actor->SetPosition(30, 750);
    // actor->SetPosition2(0.001, 0.002);
    // 位置设置方法二：设置屏幕坐标系相对位置：相对左下角比例
    // actor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
    // // actor->SetPosition(0, 0.8);
    renderer->AddActor(actor);

    
    // 可视化流程___________________________________________________________________________________start
    renderer->ResetCamera();
    renderer->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());
    // Setup render window
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("ImageGridSource");
    // Setup render window interactor
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    // Render and start interaction
    renderWindowInteractor->SetRenderWindow(renderWindow);
    // 可视化流程___________________________________________________________________________________end

    // 方案三：采用vtkFollower解决旋转问题,采用vtkOrientationMarkerWidget解决缩放问题
    // auto fixSizeAndPosPicture = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    // fixSizeAndPosPicture->SetOrientationMarker(actor);
    // fixSizeAndPosPicture->SetInteractor(renderWindowInteractor);
    // fixSizeAndPosPicture->SetEnabled(true);
    // fixSizeAndPosPicture->SetInteractive(false);
    // fixSizeAndPosPicture->SetViewport(0.2, 0.2, 0.4 , 0.4); // 图片位置和大小设置

    renderWindow->Render();
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();
    return 0;
}
