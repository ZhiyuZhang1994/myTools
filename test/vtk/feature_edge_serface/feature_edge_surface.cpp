/**
 * @brief vtkDataSetSurfaceFilter与vtkFeatureEdges
 * @author zhangzhiyu
 * @date 2023-01-05
 */

/**
 * @brief 此例子展示了从三维单元模型中提取外轮廓面，
 * 及从外表面中提取不同类特征边（轮廓边，特征边）。
 */

#include <vtkCellData.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkHexahedron.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkFeatureEdges.h>
#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPointPicker.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSphereSource.h>
#include <vtkCamera.h>
#include <vtkGeometryFilter.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellType.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkTriangle.h>
#include <vtkUnstructuredGrid.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkTetra.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

int main()
{
    // 刘帅自定义数据源 vtkUnstructuredGrid 27-point 8-cell
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

    for (int y = -1; y <= 1; y++) // 循环y,-1到1
    {
        for (int z = 10; z >= 8; z--) // 循环z,1到-1
        {
            for (int x = -1; x <= 1; x++) // 循环x,-1到1
            {
                points->InsertNextPoint(x * 10, y * 10, z * 10);
            }
        }
    }
    // cout << "points num: " << points->GetNumberOfPoints() << endl;
    // 模拟标量数据，暂定为结点y坐标
    vtkSmartPointer<vtkDoubleArray> doubleArray = vtkSmartPointer<vtkDoubleArray>::New();
    for (size_t i = 0; i < points->GetNumberOfPoints(); i++)
    {
        doubleArray->InsertNextTuple1(points->GetPoint(i)[1]); // 每个结点的y坐标
    }

    vtkSmartPointer<vtkUnstructuredGrid> grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    {                             // 27个结点组成八个单元
        int num[] = {0, 1, 4, 3}; // 1 2 5 4,减一
        int add[] = {0, 1, 3, 4, 9, 10, 12, 13};
        vector<vector<int>> cells; // 存8个序列
        const int dif = 9;
        for (size_t i = 0; i < 8; i++)
        {
            vector<int> elementNumber;
            vtkSmartPointer<vtkHexahedron> hex = vtkSmartPointer<vtkHexahedron>::New();
            // 形成每个单元的结点编号
            for (size_t j = 0; j < 8; j++)
            {
                if (j < 4)
                {
                    elementNumber.push_back(num[j] + add[i]);
                }
                else
                {
                    elementNumber.push_back(num[j - 4] + dif + add[i]);
                }
            }
            // 利用这些结点编号，插入单元中
            for (size_t j = 0; j < 8; j++)
            {
                hex->GetPointIds()->SetId(j, elementNumber[j]);
            }

            cells.push_back(elementNumber);
            grid->InsertNextCell(hex->GetCellType(), hex->GetPointIds()); // 插入单元
        }
    }

    grid->SetPoints(points);
    grid->GetPointData()->SetScalars(doubleArray);

    // 原始网格actor样式
    vtkSmartPointer<vtkDataSetMapper> mapper1 = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper1->SetInputData(grid);
    vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
    actor2->SetMapper(mapper1);
    actor2->GetProperty()->SetColor(1, 0, 0);
    actor2->GetProperty()->SetEdgeVisibility(true);
    actor2->GetProperty()->SetRepresentationToWireframe();

    // 输出24个面(外轮廓的单元面)
    vtkSmartPointer<vtkDataSetSurfaceFilter> surfaceFilter = vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
    surfaceFilter->SetInputData(grid);
    surfaceFilter->Update();
    // surfaceFilter->GetOutput()->PrintSelf(std::cout, vtkIndent(4));

    // 使用vtkFeatureEdges过滤器提取非结构化网格的内部边线
    vtkSmartPointer<vtkFeatureEdges> featureEdges = vtkSmartPointer<vtkFeatureEdges>::New();
    featureEdges->SetInputData(surfaceFilter->GetOutput());
    featureEdges->BoundaryEdgesOn();
    featureEdges->FeatureEdgesOn();
    featureEdges->ManifoldEdgesOn();
    featureEdges->NonManifoldEdgesOn();
    featureEdges->SetFeatureAngle(30); // 用于特征边(单元与单元接触边)的拾取与否
    featureEdges->Update();
    featureEdges->GetOutput()->PrintSelf(std::cout, vtkIndent(4));

    // 创建vtkPolyDataMapper和vtkActor显示结果
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(featureEdges->GetOutput());
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0, 0);

    // 创建vtkRenderer、vtkRenderWindow和vtkRenderWindowInteractor
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(renderWindow);

    // 将vtkActor添加到vtkRenderer并启动vtkRenderWindowInteractor
    renderer->AddActor(actor);

    // renderer->AddActor(actor2);
    renderer->SetBackground(1, 1, 1);
    renderWindow->SetSize(640, 480);
    renderWindow->Render();
    interactor->Start();

    return 0;
}