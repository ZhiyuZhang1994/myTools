/**
 * @brief 通过mapper设置actor置顶显示
 * @author lius
 * @date 2022-12-26
 */
// 参考：https://blog.csdn.net/weixin_41838721/article/details/125285597
// https://blog.csdn.net/wanyongtai/article/details/106519551
// https://blog.csdn.net/lpsl1882/article/details/52107117

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkArrowSource.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellCenters.h>
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkExtractSelection.h>
#include <vtkGlyph3D.h>
#include <vtkIdFilter.h>
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
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkTriangle.h>
#include <vtkTriangleFilter.h>
#include <vtkUnstructuredGrid.h>

int main(int, char*[]) {
    vtkNew<vtkNamedColors> colors;
    // 自定义5个节点，2个单元
    double coordinate[5][3] = {{0, 0, 0}, {4, 0, 0}, {4, 8, 0}, {0, 8, 0}, {8, 0, 0}}; // 5个结点
    int pointNum = 5;

    vtkPolyData* polyData = vtkPolyData::New();

    vtkPoints* points = vtkPoints::New();
    vtkCellArray* cellArray = vtkCellArray::New();
    //设置结点
    points->SetNumberOfPoints(pointNum);
    for (int i = 0; i < pointNum; ++i) {
        points->SetPoint(i, coordinate[i]);
    }

    // 四边形单元
    vtkQuad* quad = vtkQuad::New();
    for (size_t i = 0; i < 4; i++) {
        quad->GetPointIds()->SetId(i, i);
    }
    cellArray->InsertNextCell(quad);
    // 三角形单元
    vtkTriangle* triangle = vtkTriangle::New();
    triangle->GetPointIds()->SetId(0, 1);
    triangle->GetPointIds()->SetId(1, 2);
    triangle->GetPointIds()->SetId(2, 4);
    cellArray->InsertNextCell(triangle);
    // 设置polyData
    polyData->SetPolys(cellArray);
    polyData->SetPoints(points);

    vtkNew<vtkPolyDataMapper> mapper1;
    mapper1->SetInputData(polyData);
    {
        // 设置mapper1
        //mapper1对应的actor1置顶
        const double units0 = -1000; //相对偏移量，默认是0，负值对应的actor前移.正值对应的actor后移。
        // 但units取小的负数 如：-1/-0.1/-10时，一开始ok，转一下就不ok.取-20某些角度ok，有渐变效果。最好绝对值>=100的负数
        //factor在vtk8.1版本之后就不起作用了
        mapper1->SetResolveCoincidentTopologyToPolygonOffset();
        mapper1->SetRelativeCoincidentTopologyPolygonOffsetParameters(0, units0); //设置面的相对重合拓扑参数，主要是偏移量offset,factor设为0即可
        mapper1->SetRelativeCoincidentTopologyLineOffsetParameters(0, units0);    //设置线的相对重合拓扑参数，主要是偏移量offset,factor设为0即可
        mapper1->SetRelativeCoincidentTopologyPointOffsetParameter(units0);       //设置点的相对重合拓扑参数，只需要设置偏移量offset
        mapper1->Update();
        mapper1->PrintSelf(std::cout, (vtkIndent)2);
    }
    //mapper2,对应actor2
    vtkNew<vtkPolyDataMapper> mapper2;
    mapper2->SetInputData(polyData);

    {
        // // 设置mapper2
        // //mapper2对应的actor2
        // const double units0 = -200; //原数是-66000，0/正数不ok,-1/-0.1/-10一开始ok，转一下就不ok.-20某些角度ok
        // mapper2->SetResolveCoincidentTopologyToPolygonOffset();
        // mapper2->SetRelativeCoincidentTopologyPolygonOffsetParameters(-0.1, units0); //本来是0
        // mapper2->SetRelativeCoincidentTopologyLineOffsetParameters(0, units0);
        // mapper2->SetRelativeCoincidentTopologyPointOffsetParameter(units0);
        // mapper2->Update();
        // std::cout << std::endl;
        // std::cout << "mapper2:" << std::endl;
        // mapper2->PrintSelf(std::cout, (vtkIndent)2);
    }
    vtkNew<vtkActor> actor1;
    actor1->GetProperty()->SetColor(1, 0, 0);
    actor1->SetMapper(mapper1);
    // actor1->GetProperty()->SetRepresentationToWireframe();
    // actor1->GetProperty()->SetPointSize(10);

    vtkNew<vtkActor> actor2;
    actor2->GetProperty()->SetColor(0, 1, 0);
    actor2->SetMapper(mapper2);
    // actor2->GetProperty()->SetRepresentationToWireframe();
    actor2->GetProperty()->SetPointSize(10);

    vtkNew<vtkRenderer> renderer;

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("set_actor_layer_by_mapper");
    renderWindow->SetSize(400, 400);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->Initialize();

    renderer->AddActor(actor1); //红
    renderer->AddActor(actor2); //绿

    // renderer->ResetCamera();

    renderer->SetBackground(colors->GetColor3d("PaleTurquoise").GetData());

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
