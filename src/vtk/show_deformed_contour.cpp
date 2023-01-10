/**
 * @file show_deformed_contour.cpp
 * @author LiuShuai (you@domain.com)
 * @brief 显示变形后的云图，变形系数可调节
 * @date 2023-1-2
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAreaPicker.h>
#include <vtkArrowSource.h>
#include <vtkAxesActor.h>
#include <vtkCellCenters.h>
#include <vtkCellData.h>
#include <vtkCutter.h>
#include <vtkCylinderSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGlyph3D.h>
#include <vtkHexahedron.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkIndent.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkLabeledDataMapper.h>
#include <vtkLongArray.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPlane.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyVertex.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkScalarBarActor.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTextProperty.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVersion.h>
#include <vtkVertex.h>
#include <vtkVertexGlyphFilter.h>

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

namespace
{
// 定义位移,有三个分量，分别代表xyz三个分量上的位移
struct U {
    double Ux;
    double Uy;
    double Uz;
};
// Define interaction style
class InteractorStyle : public vtkInteractorStyleRubberBandPick {
public:
    static InteractorStyle *New();
    vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);
    //构造函数
    InteractorStyle() {
        // this->ArrawActor = vtkSmartPointer<vtkActor>::New();
        newPoints = vtkSmartPointer<vtkPoints>::New();
    }
    /**
     * @brief 传入网格的数据源、结点位移与变形系数，将网格的节点坐标改为变形之后的
     * 
     * @param grid 网格的数据源
     * @param u 结点位移
     * @param deformedScaleFactor 变形系数
     */
    void AddDeformedUAndFactor(vtkSmartPointer<vtkUnstructuredGrid> grid, std::vector<U> &u, double deformedScaleFactor = 1) {
        newPoints->DeepCopy(grid->GetPoints());
        for (size_t i = 0; i < u.size(); i++) {
            double newCo[3];
            newPoints->GetPoint(i, newCo);
            newCo[0] += u[i].Ux * deformedScaleFactor; //相关分量加上实际变形*变形缩放系数
            newCo[1] += u[i].Uy * deformedScaleFactor;
            newCo[2] += u[i].Uz * deformedScaleFactor;
            newPoints->SetPoint(i, newCo);
        }
        grid->SetPoints(newPoints);
    }

    virtual void OnLeftButtonUp() override {
        vtkNew<vtkNamedColors> colors;

        // Forward events
        vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

        // 变形(开始)--lius
        // 结点真正的坐标为：变形前x坐标+Ux*DeformedScaleFactor,y z同理
        //初始化27个结点的位移数据，全部置为0
        U initialU = {0, 0, 0};
        std::vector<U> UOfPoints(27, initialU);
        // 20号结点对应ABAQUS测试文件的结点，将ABAQUS测试算例的结点的位移结果赋给20号结点
        UOfPoints[20].Ux = 0.127;
        UOfPoints[20].Uy = 0.127;
        UOfPoints[20].Uz = -0.4641;
        double deformedScaleFactor = 100; //10,21.5472,100
        //打印20号结点的 1.变形前的坐标、2.变形缩放系数、3.变形后坐标（不考虑变形系数）、4.变形后坐标（考虑变形系数）
        cout << "Start: " << endl;
        cout << "Base coordinates: " << UnstructuredGrid->GetPoint(20)[0] << " "
             << UnstructuredGrid->GetPoint(20)[1] << " " << UnstructuredGrid->GetPoint(20)[2] << " " << endl;
        cout << "Deformation Scale Factor: " << deformedScaleFactor << endl;
        // 更改网格的节点坐标
        AddDeformedUAndFactor(UnstructuredGrid, UOfPoints, deformedScaleFactor);
        cout << "Deformed coordinates (scaled): " << UnstructuredGrid->GetPoint(20)[0] << " "
             << UnstructuredGrid->GetPoint(20)[1] << " " << UnstructuredGrid->GetPoint(20)[2] << " " << endl;
        cout << "End" << endl;
        string newName = "show_deformed_contour  ";
        newName.append("scale=");
        newName.append(to_string(deformedScaleFactor));
        const char *newName2 = newName.c_str();
        this->GetInteractor()->GetRenderWindow()->SetWindowName(newName2); //
        // 变形(结束)--lius
        this->GetInteractor()->GetRenderWindow()->Render();
        this->HighlightProp(NULL);
    }

    void SetGrid(vtkSmartPointer<vtkUnstructuredGrid> grid) {
        this->UnstructuredGrid = grid;
    }

private:
    vtkSmartPointer<vtkUnstructuredGrid> UnstructuredGrid;
    vtkSmartPointer<vtkPoints> newPoints;
};

vtkStandardNewMacro(InteractorStyle);
} // namespace

int main(int, char *[]) {
    vtkNew<vtkNamedColors> colors;

    // 刘帅自定义数据源 vtkUnstructuredGrid 27-point 8-cell
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

    for (int y = -1; y <= 1; y++) //循环y,-1到1
    {
        for (int z = 10; z >= 8; z--) //循环z,1到-1
        {
            for (int x = -1; x <= 1; x++) //循环x,-1到1
            {
                points->InsertNextPoint(x * 10, y * 10, z * 10);
            }
        }
    }
    // cout << "points num: " << points->GetNumberOfPoints() << endl;
    //模拟标量数据，暂定为结点y坐标
    vtkSmartPointer<vtkDoubleArray> doubleArray = vtkSmartPointer<vtkDoubleArray>::New();
    for (size_t i = 0; i < points->GetNumberOfPoints(); i++) {
        doubleArray->InsertNextTuple1(points->GetPoint(i)[1]); //每个结点的y坐标
    }

    vtkSmartPointer<vtkUnstructuredGrid> grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    {                             //27个结点组成八个单元
        int num[] = {0, 1, 4, 3}; // 1 2 5 4,减一
        int add[] = {0, 1, 3, 4, 9, 10, 12, 13};
        vector<vector<int>> cells; //存8个序列
        const int dif = 9;
        for (size_t i = 0; i < 8; i++) {
            vector<int> elementNumber;
            vtkSmartPointer<vtkHexahedron> hex = vtkSmartPointer<vtkHexahedron>::New();
            // 形成每个单元的结点编号
            for (size_t j = 0; j < 8; j++) {
                if (j < 4) {
                    elementNumber.push_back(num[j] + add[i]);
                } else {
                    elementNumber.push_back(num[j - 4] + dif + add[i]);
                }
            }
            // 利用这些结点编号，插入单元中
            for (size_t j = 0; j < 8; j++) {
                hex->GetPointIds()->SetId(j, elementNumber[j]);
            }

            cells.push_back(elementNumber);
            grid->InsertNextCell(hex->GetCellType(), hex->GetPointIds()); //插入单元
        }
    }

    grid->SetPoints(points);
    grid->GetPointData()->SetScalars(doubleArray);
    // 颜色映射表
    vtkNew<vtkLookupTable> pColorTableModel;
    pColorTableModel->SetNumberOfColors(12);
    pColorTableModel->SetHueRange(0.67, 0); //色调从红色到蓝色，【0，0.67】则表示从蓝色到红色
    pColorTableModel->SetAlphaRange(1, 1);
    pColorTableModel->SetValueRange(1, 1);
    pColorTableModel->SetSaturationRange(1, 1);
    pColorTableModel->SetTableRange(-10, 10); //最大值最小值
    pColorTableModel->Build();
    // vtkDataSetMapper
    vtkNew<vtkDataSetMapper> dataSetMapper;
    dataSetMapper->SetInputData(grid);
    dataSetMapper->SetLookupTable(pColorTableModel);
    dataSetMapper->SetInterpolateScalarsBeforeMapping(1);
    dataSetMapper->SetScalarRange(-10, 10); //最大值最小值

    //渲染网格的色卡
    vtkSmartPointer<vtkActor> actorGrid = vtkSmartPointer<vtkActor>::New();
    actorGrid->SetMapper(dataSetMapper);
    actorGrid->GetProperty()->SetColor(1, 0, 0);
    // actorGrid->GetProperty()->SetOpacity(0.5);
    actorGrid->GetProperty()->SetLineWidth(1);
    actorGrid->GetProperty()->SetEdgeVisibility(1);

    // 设置scalarBar（色卡）
    vtkNew<vtkScalarBarActor> scalarBarActor;
    scalarBarActor->SetLookupTable(pColorTableModel);
    scalarBarActor->SetTitle("Height");
    scalarBarActor->GetTitleTextProperty()->ShadowOff(); //“标题”关闭阴影
    scalarBarActor->GetLabelTextProperty()->ShadowOff(); //“标签”关闭阴影
    scalarBarActor->GetTitleTextProperty()->ItalicOff(); //“标题”关闭斜体
    scalarBarActor->GetLabelTextProperty()->ItalicOff(); //“标签”关闭斜体
    scalarBarActor->GetTitleTextProperty()->BoldOff();   //“标题”不加粗
    scalarBarActor->GetLabelTextProperty()->BoldOff();   //“标签”不加粗
    scalarBarActor->SetNumberOfLabels(13);

    // Visualize
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("show_deformed_contour");
    renderWindow->SetSize(900, 600);
    renderer->AddActor(scalarBarActor);
    renderer->AddActor(actorGrid);

    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
    // 交互
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkNew<InteractorStyle> style;
    // style->StartSelect(); // 开始选择按键的设置
    style->SetGrid(grid);
    style->SetDatasetMapper(dataSetMapper);
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}