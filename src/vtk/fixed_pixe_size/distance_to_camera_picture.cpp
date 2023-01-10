/**
 * @file texture_map_plane.cpp
 * @author lius (you@domain.com)
 * @brief 整合vtk官方案例+《图形图像开发进阶》2.2.4。官方案例：https://kitware.github.io/vtk-examples/site/Cxx/Visualization/TextureMapPlane/
 * 这个示例读取一个jpg图像文件，并使用它对平面进行纹理贴图.
 * 先读入一幅JPEG的二维纹理图;
 * 然后定义一个纹理类vtkTexture对象，接着把读入的JPEG图像输入到vtkTexture里，作为它即将“贴”到平面上的一个纹理图;
 * 再定义一个vtkPlaneSource对象，类vtkPlaneSource可以生成一个平面，也就是纹理图要“贴”图的地方。
 * 简而言之，在做纹理贴图时，先要有东西可以“贴”，也就是要准备一幅二维的纹理图;然后再确定这幅纹理图要“贴”到哪里。

 * @version 0.1
 * @date 2023-01-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <vtkDistanceToCamera.h>
#include <vtkGlyph3DMapper.h>
#include <vtkImageData.h>
#include <vtkJPEGReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTexture.h>
#include <vtkTextureMapToPlane.h>

int main(int argc, char* argv[]) {
    // Parse command line arguments
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " Filename e.g. Yinyang.jpg"
                  << std::endl;
        return EXIT_FAILURE;
    }

    vtkNew<vtkNamedColors> colors;

    std::string inputFilename = argv[1];

    // Read the image which will be the texture
    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName(inputFilename.c_str());

    // Create a plane
    vtkNew<vtkPlaneSource> plane;
    plane->SetCenter(0.0, 0.0, 0.0);
    plane->SetNormal(0.0, 0.0, 1.0);

    // Apply the texture
    vtkNew<vtkTexture> texture;
    texture->SetInputConnection(jPEGReader->GetOutputPort());
    // texture->InterpolateOn();
    // 官方例子里面的这层过滤器，对显示图片来说，不是必须的。跳过这个也可以
    vtkNew<vtkTextureMapToPlane> texturePlane;
    texturePlane->SetInputConnection(plane->GetOutputPort());

    // vtkNew<vtkPolyDataMapper> planeMapper;
    // planeMapper->SetInputConnection(texturePlane->GetOutputPort());

    // Draw some pictures that maintain a fixed size during zooming.
    vtkNew<vtkPointSource> pointSource;
    pointSource->SetNumberOfPoints(4);
    // Calculate the distance to the camera of each point.
    vtkNew<vtkDistanceToCamera> distanceToCamera;
    distanceToCamera->SetInputConnection(pointSource->GetOutputPort());
    distanceToCamera->SetScreenSize(60.0);

    //-------------------------vtkDistanceToCamera+vtkGlyph3DMapper，实现固定像素显示
    vtkNew<vtkGlyph3DMapper> glyph3DMapper;
    glyph3DMapper->SetInputConnection(distanceToCamera->GetOutputPort());
    glyph3DMapper->SetSourceConnection(plane->GetOutputPort()); //可以是vtkTextureMapToPlane，plane
    //要有SetScaleModeToScaleByMagnitude().
    // SetScaleModeToScaleNoDataScaling不起作用，SetScaleModeToScaleByVectorComponents需要DistanceToCamera have 3 components,
    glyph3DMapper->SetScaleModeToScaleByMagnitude();
    glyph3DMapper->SetScalarVisibility(false);
    glyph3DMapper->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "DistanceToCamera"); //不可缺少，name不可改变长度与大小写

    vtkNew<vtkActor> textureActor;
    textureActor->SetMapper(glyph3DMapper);
    textureActor->SetTexture(texture);
    textureActor->GetProperty()->SetColor(
        colors->GetColor3d("MistyRose").GetData());

    // Visualize the textured plane
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("TextureMapPlane");

    distanceToCamera->SetRenderer(renderer);

    renderer->AddActor(textureActor);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderWindow->Render();

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
