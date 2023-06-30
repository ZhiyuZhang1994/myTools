/**
 * @brief 通过设置renderer设置actor的图层
 * @author lius
 * @date 2022-12-19
 */
// 参考：https://blog.csdn.net/weixin_41838721/article/details/125285597
// https://blog.csdn.net/wanyongtai/article/details/106519551
// https://blog.csdn.net/lpsl1882/article/details/52107117

/*
    layer: layer数值越大，越在顶层
    PreserveColorBuffer: 1-》透明、0-》不透明(opaque)
    底层：不透明
    其他层次(如顶层)：透明(为了能看到下层)
*/

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkArrowSource.h>
#include <vtkCellArray.h>
#include <vtkCellCenters.h>
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkExtractSelection.h>
#include <vtkGlyph3D.h>
#include <vtkCylinderSource.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLabeledDataMapper.h>
#include <vtkLine.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyVertex.h>
#include <vtkProperty.h>
#include <vtkQuad.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include "vtkCamera.h"
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkTriangle.h>
#include <vtkTriangleFilter.h>
#include <vtkUnstructuredGrid.h>

int main(int, char*[]) {
    vtkNew<vtkNamedColors> colors;
    vtkCamera* camera = vtkCamera::New();
    vtkNew<vtkRenderer> renderer1;
    renderer1->SetActiveCamera(camera);
    renderer1->SetBackground(colors->GetColor3d("SlateGray").GetData());

    vtkNew<vtkRenderer> renderer2;
    renderer2->SetActiveCamera(camera);
    renderer2->SetBackground(colors->GetColor3d("SlateGray").GetData());


    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetResolution(4);
    cylinder->Update();

    vtkNew<vtkPolyDataMapper> mapper1;
    mapper1->SetInputConnection(cylinder->GetOutputPort());
    vtkNew<vtkActor> actor1;
    actor1->GetProperty()->SetColor(colors->GetColor3d("SeaGreen").GetData());
    actor1->SetMapper(mapper1);


    //mapper2,对应actor2
    vtkNew<vtkPolyDataMapper> mapper2;
    mapper2->SetInputConnection(cylinder->GetOutputPort());
    vtkNew<vtkActor> actor2;
    actor2->GetProperty()->SetColor(colors->GetColor3d("Blue").GetData());
    actor2->SetMapper(mapper2);



    vtkNew<vtkArrowSource> arrow;
    vtkNew<vtkPolyDataMapper> mapper3;
    mapper3->SetInputConnection(arrow->GetOutputPort());
    vtkNew<vtkActor> actor3;
    actor3->SetMapper(mapper3);




    // renderer1->AddActor(actor1);
    renderer1->AddActor(actor3); // actor3 图例
    renderer2->AddActor(actor2); // actor2 网格
    vtkNew<vtkRenderWindow> renderWindow;
    {
        // 将renderer1置顶
        //绘制actor1的renderer1在第1层，绘制actor2的renderer2在第0层
        // 1>0，那么renderer1就会覆盖显示下一层的renderer2
        renderer1->SetLayer(1); //负数的layer会报错
        renderer2->SetLayer(0);
        renderWindow->SetNumberOfLayers(2); // 得加，否则只绘制一个图层
    }
    // renderer1->SetPreserveColorBuffer(1); // 1 : 透明

    renderWindow->AddRenderer(renderer1);
    renderWindow->AddRenderer(renderer2);
    renderWindow->SetWindowName("set_actor_layer_by_renderer");
    renderWindow->SetSize(400, 400);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->Initialize();

    renderer1->ResetCamera();
    renderer2->ResetCamera();
    renderWindow->Render();
    renderWindowInteractor->Start();
    return EXIT_SUCCESS;
}
