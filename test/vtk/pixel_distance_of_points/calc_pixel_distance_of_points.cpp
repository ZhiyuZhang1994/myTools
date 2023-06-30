/**
 * @file show_deformed_contour.cpp
 * @author LiuShuai (you@domain.com)
 * @brief 显示变形后的云图，变形系数自适应
 * @date 2023-1-2
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAreaPicker.h>
#include <vtkArrowSource.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
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
#include <vtkProperty2D.h>
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

#include <math.h>
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
    void UpdatePointToDeformed(vtkSmartPointer<vtkUnstructuredGrid> grid, std::vector<U> &u, double deformedScaleFactor = 1) {
        newPoints->DeepCopy(ConstUnstructuredGrid->GetPoints());
        for (size_t i = 0; i < u.size(); i++) {
            double newCo[3];
            newPoints->GetPoint(i, newCo);
            newCo[0] += u[i].Ux * deformedScaleFactor; //相关分量加上实际变形*变形缩放系数
            newCo[1] += u[i].Uy * deformedScaleFactor;
            newCo[2] += u[i].Uz * deformedScaleFactor;
            newPoints->SetPoint(i, newCo);
        }
        grid->SetPoints(newPoints);
        // DatasetMapper->Update();
    }
    double GetMaxU(const std::vector<U> &u, int &nodeIndex) {
        //求最大位移以及所在的节点索引
        double maxU = INT32_MIN;
        nodeIndex = -1;
        for (size_t i = 0; i < u.size(); i++) {
            double tempU = sqrt(pow(u[i].Ux, 2) + pow(u[i].Uy, 2) + pow(u[i].Uz, 2));
            if (tempU > maxU) {
                nodeIndex = i;
                maxU = tempU;
            }
        }
        return maxU;
    }
    /**
     * @brief 根据最大变形量表现的实际像素距离 求变形缩放系数
     * 
     * @param realDisplayDistance 最大变形量表现的实际像素距离
     * @param ruleDisplayDistance 规定最大变形量要显示为多少像素距离
     * @return double 变形缩放系数
     */
    double CalculateDeformedScaleFactorByGivenDisplayDistance(int realDisplayDistance, int ruleDisplayDistance = 100) {
        // 世界坐标系上两个点的实际像素距离realDisplayDistance,规定像素距离ruleDisplayDistance，变形系数Factor,
        // ruleDisplayDistance=realDisplayDistance*Factor,得到：Factor=ruleDisplayDistance/realDisplayDistance

        //真实像素距离为0的话，说明没有变形，变形系数置为0。需要转化为double类型，否则返回整数
        return realDisplayDistance == 0 ? 0 : (double)ruleDisplayDistance / realDisplayDistance;
    }

    /**
     * @brief 求世界坐标系两点在屏幕坐标系上的距离
     * 
     * @param p1 世界坐标系的第一个点
     * @param p2 世界坐标系的第二个点
     * @return double 世界坐标系两点在屏幕坐标系上的距离。经测试，返回值是int
     */
    double GetDisplayDistanceBetweenTwoPoint(double *p1, double *p2) {
        //得到当前的Renderer，转化为父类viewport
        vtkViewport *viewPort = this->CurrentRenderer;
        //p1
        //建立坐标系统
        vtkSmartPointer<vtkCoordinate> pCoord1 = vtkSmartPointer<vtkCoordinate>::New();
        //坐标系统设置为World坐标系统
        pCoord1->SetCoordinateSystemToWorld();
        //Set,传入World坐标系统中的节点位置
        pCoord1->SetValue(p1);
        //Get,坐标系统之间的转换：将World坐标系转化为Display坐标系，得到计算后的坐标Display坐标系中的坐标（像素值，用整数表示）。注意是二维的
        auto dispCoord1 = pCoord1->GetComputedDisplayValue(viewPort);

        //p2，与P1操作相同
        vtkSmartPointer<vtkCoordinate> pCoord2 = vtkSmartPointer<vtkCoordinate>::New();
        pCoord2->SetCoordinateSystemToWorld();
        pCoord2->SetValue(p2);
        auto dispCoord2 = pCoord2->GetComputedDisplayValue(viewPort);
        //返回P1与P2在屏幕坐标系中的距离
        return sqrt(pow(dispCoord1[0] - dispCoord2[0], 2) + pow(dispCoord1[1] - dispCoord2[1], 2));
    }
    virtual void OnLeftButtonUp() override {
        vtkNew<vtkNamedColors> colors;

        // Forward events
        vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

        // 变形(开始)--lius
        // 结点真正的坐标为：变形前x坐标+Ux*DeformedScaleFactor,y z同理
        //初始化27个结点的位移数据，先全部置为0，再赋特殊值
        U initialU = {0, 0, 0};
        std::vector<U> UOfPoints(27, initialU);
        // 20号结点(右上角)赋予位移值
        UOfPoints[20].Ux = 0;
        UOfPoints[20].Uy = 1;
        UOfPoints[20].Uz = 0;
        //2号结点（右下角）赋予位移值
        UOfPoints[2].Ux = 0;
        UOfPoints[2].Uy = -0.5;
        UOfPoints[2].Uz = 0;
        cout << "Start: " << endl;
        //求最大变形量以及节点索引
        int UmaxNodeIndex = -1;
        double Umax = GetMaxU(UOfPoints, UmaxNodeIndex);
        //最大变形点变形前的坐标
        double *p1 = ConstUnstructuredGrid->GetPoint(UmaxNodeIndex);
        //变形前坐标+增量 得到变形后的坐标
        double p2[3] = {p1[0] + UOfPoints[UmaxNodeIndex].Ux, p1[1] + UOfPoints[UmaxNodeIndex].Uy, p1[2] + UOfPoints[UmaxNodeIndex].Uz};

        double realDisplayDistance = GetDisplayDistanceBetweenTwoPoint(p1, p2);
        cout << "Umax number: " << UmaxNodeIndex << "  Base coordinates: " << p1[0] << " " << p1[1] << " " << p1[2] << " " << endl;
        cout << "DisplayDistance: " << realDisplayDistance << endl;
        //反求变形缩放系数
        double deformedScaleFactor = CalculateDeformedScaleFactorByGivenDisplayDistance(realDisplayDistance);
        cout << "DeformationScaleFactor:  " << deformedScaleFactor << endl;
        //用新的变形缩放系数绘制变形图
        UpdatePointToDeformed(UnstructuredGrid, UOfPoints, deformedScaleFactor);
        cout << "Deformed coordinates (scaled): " << p1[0] << " "
             << p1[1] << " " << p1[2] << " " << endl;
        cout << "End" << endl
             << endl;
        string newName = "show_deformed_contour  ";
        newName.append("scale=");
        newName.append(to_string(deformedScaleFactor));
        const char *newName2 = newName.c_str();
        this->GetInteractor()->GetRenderWindow()->SetWindowName(newName2); //
        // 变形(结束)--lius
        this->GetInteractor()->GetRenderWindow()->Render();
        this->HighlightProp(NULL);
    }
    virtual void OnRightButtonUp() override {
        // 重置相机
        vtkInteractorStyleRubberBandPick::OnRightButtonUp();
        this->CurrentRenderer->GetActiveCamera()->SetFocalPoint(0, 0, 0); //焦点
        this->CurrentRenderer->GetActiveCamera()->SetPosition(0, 0, 1);   //相机位置
        this->CurrentRenderer->GetActiveCamera()->SetViewUp(0, 1, 0);     //朝上方向
        this->CurrentRenderer->ResetCamera();
        this->GetInteractor()->GetRenderWindow()->Render();
    }

    void SetGrid(vtkSmartPointer<vtkUnstructuredGrid> grid) {
        this->UnstructuredGrid = grid;
        this->ConstUnstructuredGrid->DeepCopy(grid);
    }
    void SetDatasetMapper(vtkSmartPointer<vtkDataSetMapper> mapper) {
        this->DatasetMapper = mapper;
    }

private:
    vtkSmartPointer<vtkUnstructuredGrid> UnstructuredGrid;
    vtkSmartPointer<vtkUnstructuredGrid> ConstUnstructuredGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    vtkSmartPointer<vtkDataSetMapper> DatasetMapper;
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
        for (int z = 1; z >= -1; z--) //循环z,1到-1
        {
            for (int x = -1; x <= 1; x++) //循环x,-1到1
            {
                points->InsertNextPoint(x, y, z);
            }
        }
    }
    // cout << "points num: " << points->GetNumberOfPoints() << endl;
    //模拟标量数据，暂定为结点y坐标
    vtkSmartPointer<vtkDoubleArray> doubleArray = vtkSmartPointer<vtkDoubleArray>::New();
    for (size_t i = 0; i < points->GetNumberOfPoints(); i++) {
        doubleArray->InsertNextTuple1(points->GetPoint(i)[1] / 3); //每个结点的y坐标
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
    pColorTableModel->SetTableRange(-1, 1); //最大值最小值
    pColorTableModel->Build();
    // vtkDataSetMapper
    vtkNew<vtkDataSetMapper> dataSetMapper;
    dataSetMapper->SetInputData(grid);
    dataSetMapper->SetLookupTable(pColorTableModel);
    dataSetMapper->SetInterpolateScalarsBeforeMapping(1);
    dataSetMapper->SetScalarRange(-0.5, 0.5); //最大值最小值

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
    renderer->ResetCamera(actorGrid->GetBounds()); //重置
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