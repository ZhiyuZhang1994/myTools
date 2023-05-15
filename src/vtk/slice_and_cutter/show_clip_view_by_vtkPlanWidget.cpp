/**
 * @brief 采用vtkPlane做为要获取的切面，
 * 将vtkPlane作为vtkCutter的参数来获取被切actor上指定位置的切面
 * 
 * @author zhangzhiyu
 * @date 2023-01-05
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
#include <vtkClipDataSet.h>
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
#include <vtkLight.h>
#include <vtkLongArray.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPlane.h>
#include <vtkPlaneWidget.h>
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

    // Define interaction style
    class InteractorStyle : public vtkInteractorStyleRubberBandPick
    {
    public:
        static InteractorStyle *New();
        vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);
        // 构造函数
        InteractorStyle()
        {
            // this->ArrawActor = vtkSmartPointer<vtkActor>::New();
            newPoints = vtkSmartPointer<vtkPoints>::New();
        }
        void UpdatePlaneAndWidget()
        {
            PlaneWidget->GetPlane(Plane);
            // PlaneWidget->SetCenter(Plane->get()); //
            // PlaneWidget->PlaceWidget(UnstructuredGrid->GetBounds()); //
        }

        virtual void OnLeftButtonUp() override
        {
            vtkNew<vtkNamedColors> colors;

            // Forward events
            vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

            // 切割(开始)--lius
            UpdatePlaneAndWidget();
            // 切割(结束)--lius
            this->GetInteractor()->GetRenderWindow()->Render();
            this->HighlightProp(NULL);
        }
        virtual void OnRightButtonUp() override
        {
            // 原始actor的隐藏/显示
            vtkInteractorStyleRubberBandPick::OnRightButtonUp();
            this->Actor->GetVisibility() ? (Actor->VisibilityOff()) : Actor->VisibilityOn();

            this->GetInteractor()->GetRenderWindow()->Render();
        }

        void SetGrid(vtkSmartPointer<vtkUnstructuredGrid> grid)
        {
            this->UnstructuredGrid = grid;
            this->ConstUnstructuredGrid->DeepCopy(grid);
        }
        void SetActor(vtkSmartPointer<vtkActor> actor)
        {
            this->Actor = actor;
        }
        void SetPlane(vtkSmartPointer<vtkPlane> plane)
        {
            this->Plane = plane;
        }
        void SetWidget(vtkSmartPointer<vtkPlaneWidget> planeWidget)
        {
            this->PlaneWidget = planeWidget;
        }

    private:
        vtkSmartPointer<vtkUnstructuredGrid> UnstructuredGrid;
        vtkSmartPointer<vtkUnstructuredGrid> ConstUnstructuredGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
        vtkSmartPointer<vtkActor> Actor;
        vtkSmartPointer<vtkPoints> newPoints;
        vtkSmartPointer<vtkPlane> Plane;
        vtkSmartPointer<vtkPlaneWidget> PlaneWidget;
    };

    vtkStandardNewMacro(InteractorStyle);
} // namespace

class vtkRealTimeSliceCallback : public vtkCommand
{
public:
    static vtkRealTimeSliceCallback *New() { return new vtkRealTimeSliceCallback; }
    void Execute(vtkObject *caller, unsigned long eventId, void *) override
    {
        if (eventId != vtkCommand::InteractionEvent)
        {
            return;
        }
        vtkPlaneWidget *planeWidget = reinterpret_cast<vtkPlaneWidget *>(caller);
        // 提前创建好平面
        planeWidget->GetPlane(plane);
        cutter_->SetCutFunction(plane);
        planeWidget->GetInteractor()->GetRenderWindow()->Render();
    }
    vtkRealTimeSliceCallback() {}

    void setCutter(vtkSmartPointer<vtkCutter> cutter) {
        cutter_ = cutter;
    }

private:
    vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
    vtkSmartPointer<vtkCutter> cutter_ = nullptr;
};

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    // 刘帅自定义数据源 vtkUnstructuredGrid 27-point 8-cell
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

    for (int y = -1; y <= 1; y++) // 循环y,-1到1
    {
        for (int z = 1; z >= -1; z--) // 循环z,1到-1
        {
            for (int x = -1; x <= 1; x++) // 循环x,-1到1
            {
                points->InsertNextPoint(x, y, z);
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
    // 颜色映射表
    vtkNew<vtkLookupTable> pColorTableModel;
    pColorTableModel->SetNumberOfColors(12);
    pColorTableModel->SetHueRange(0.67, 0); // 色调从红色到蓝色，【0，0.67】则表示从蓝色到红色
    pColorTableModel->SetAlphaRange(1, 1);
    pColorTableModel->SetValueRange(1, 1);
    pColorTableModel->SetSaturationRange(1, 1);
    pColorTableModel->SetTableRange(0, 1); // 最大值最小值
    pColorTableModel->Build();
    // vtkDataSetMapper
    vtkNew<vtkDataSetMapper> dataSetMapper;
    dataSetMapper->SetInputData(grid);
    dataSetMapper->SetLookupTable(pColorTableModel);
    dataSetMapper->SetInterpolateScalarsBeforeMapping(1);
    dataSetMapper->SetScalarRange(0, 1); // 最大值最小值

    // 渲染网格的色卡
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
    scalarBarActor->GetTitleTextProperty()->ShadowOff(); // “标题”关闭阴影
    scalarBarActor->GetLabelTextProperty()->ShadowOff(); // “标签”关闭阴影
    scalarBarActor->GetTitleTextProperty()->ItalicOff(); // “标题”关闭斜体
    scalarBarActor->GetLabelTextProperty()->ItalicOff(); // “标签”关闭斜体
    scalarBarActor->GetTitleTextProperty()->BoldOff();   // “标题”不加粗
    scalarBarActor->GetLabelTextProperty()->BoldOff();   // “标签”不加粗
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
    renderer->ResetCamera(actorGrid->GetBounds()); // 重置
    // 创建交互器
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // widget
    //  创建交互器和切面小部件

    vtkSmartPointer<vtkPlaneWidget> planeWidget = vtkSmartPointer<vtkPlaneWidget>::New();
    planeWidget->SetInteractor(renderWindowInteractor);
    planeWidget->SetResolution(20);
    planeWidget->GetPlaneProperty()->SetColor(1, 1, 1);
    planeWidget->SetRepresentationToOutline();   // 边框
    planeWidget->SetCenter(grid->GetCenter());   // 放在中间位置
    planeWidget->NormalToYAxisOn();              // 指向Y轴
    planeWidget->PlaceWidget(grid->GetBounds()); // 放置widget
    planeWidget->SetEnabled(1);
    planeWidget->On();


    // 提前创建好平面
    vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
    planeWidget->GetPlane(plane);

    // 创建切割对象
    vtkSmartPointer<vtkCutter> cutter = vtkSmartPointer<vtkCutter>::New();
    cutter->SetInputData(grid);
    cutter->SetCutFunction(plane);


    vtkRealTimeSliceCallback *cwCallback2 = vtkRealTimeSliceCallback::New();
    planeWidget->AddObserver(vtkCommand::InteractionEvent, cwCallback2);
    cwCallback2->setCutter(cutter);

    // cutter->GetValues
    // 创建切面演员
    vtkSmartPointer<vtkPolyDataMapper> cutterMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    cutterMapper->SetLookupTable(pColorTableModel);
    cutterMapper->SetInputConnection(cutter->GetOutputPort());
    cutterMapper->SetInterpolateScalarsBeforeMapping(1);
    cutterMapper->SetScalarRange(0, 1); // 最大值最小值
    vtkSmartPointer<vtkActor> cutterActor = vtkSmartPointer<vtkActor>::New();
    cutterActor->SetMapper(cutterMapper);

    // 将切面演员添加到渲染器中
    renderer->AddActor(cutterActor);

    // 自定义交互方式+传参
    vtkNew<InteractorStyle> style;
    // style->StartSelect(); // 开始选择按键的设置
    style->SetGrid(grid);
    style->SetActor(actorGrid);
    style->SetPlane(plane);
    style->SetWidget(planeWidget);
    renderWindowInteractor->SetInteractorStyle(style);

    vtkSmartPointer<vtkLight> light = vtkSmartPointer<vtkLight>::New();
    light->SetLightTypeToCameraLight();

    // 开始交互
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}