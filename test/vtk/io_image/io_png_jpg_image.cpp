/**
 * @brief VTK windows导出图片
 * @author zhangzhiyu
 * @date 2022-12-14
 */

#include "tools/tassk_class.h"
#include "tools/msg_service.h"
#include "tools/table_driven.h"

#include <utility>
#include <iostream>
#include <signal.h>
#include <iostream>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <string>
#include <vector>
#include <vtkAxesActor.h>
#include <vtkActor.h>
#include <vtkAutoInit.h>
#include <vtkCylinderSource.h>
#include <vtkDataSetMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGWriter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkVersion.h>
#include <vtkWindowToImageFilter.h>

VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkRenderingFreeType);

int main(int, char*[]) {
    vtkNew<vtkNamedColors> colors;

    vtkSmartPointer<vtkCylinderSource> cylinder = vtkSmartPointer<vtkCylinderSource>::New();
    cylinder->Update();
    vtkSmartPointer<vtkDataSetMapper> mapper2 = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper2->SetInputConnection(cylinder->GetOutputPort());
    vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
    actor2->SetMapper(mapper2);
    actor2->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());

    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetCenter(1.0, 1.0, 0.0);
    sphereSource->SetRadius(0.5);
    sphereSource->SetPhiResolution(30);
    sphereSource->SetThetaResolution(30);
    sphereSource->Update();
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("IndianRed").GetData());

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetAlphaBitPlanes(1); // enable usage of alpha channel
    renderWindow->SetWindowName("Screenshot");
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkAxesActor> axes;
    renderer->AddActor(axes);

    renderer->AddActor(actor);
    renderer->AddActor(actor2);
    renderer->SetBackground(colors->GetColor3d("MistyRose").GetData());
    renderWindow->Render();

    // Screenshot
    vtkNew<vtkWindowToImageFilter> windowToImageFilter;
    windowToImageFilter->SetInput(renderWindow);
    windowToImageFilter->SetScale(1);                // image quality
    windowToImageFilter->SetInputBufferTypeToRGBA(); // also record the alpha
                                                     // (transparency) channel
    windowToImageFilter->ReadFrontBufferOff();        // read from the back buffer
    windowToImageFilter->Update();

    vtkNew<vtkPNGWriter> writer;
    writer->SetFileName("C:\\Users\\张智禹\\Desktop\\screenshot_off.png");
    writer->SetInputConnection(windowToImageFilter->GetOutputPort());
    writer->Write();

    vtkNew<vtkJPEGWriter> writer2;
    writer2->SetFileName("C:\\Users\\张智禹\\Desktop\\test_pic.jpg");
    writer2->SetInputConnection(filter->GetOutputPort());
    writer2->Write();

    renderWindow->Render();
    renderer->ResetCamera();
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

