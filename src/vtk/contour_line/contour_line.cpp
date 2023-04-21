/**
 * @file set_vtkContourFilter.cpp
 * @author LiuShuai
 * @brief 绘制云图的等值线图
 * @date 2023-4-17
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
#include <vtkCellCenters.h>
#include <vtkCellData.h>
#include <vtkContourFilter.h>
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
#include <vtkContourFilter.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkFloatArray.h>
#include <vtkCellDataToPointData.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkStripper.h>
#include "vtkGenericCell.h"
#include "vtkCharArray.h"

using namespace std;
const std::uint32_t num = 7;

namespace
{

    // Define interaction style
    class InteractorStyle : public vtkInteractorStyleRubberBandPick
    {
    public:
        static InteractorStyle *New();
        vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);
        // 构造函数,实例化内部的成员
        InteractorStyle()
        {
            contourFilter = vtkSmartPointer<vtkContourFilter>::New();
            contourActor = vtkSmartPointer<vtkActor>::New();
            contourMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        }

        virtual void OnLeftButtonUp() override
        {
            // Forward events
            vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

            this->GetInteractor()->GetRenderWindow()->Render();
            this->HighlightProp(NULL);
        }

        virtual void OnRightButtonUp() override
        {
            // Forward events
            vtkInteractorStyleRubberBandPick::OnRightButtonUp();

            // 等值线(开始)--lius
            auto originData = this->DatasetMapper->GetInput();
            double rangeMin = originData->GetScalarRange()[0];
            double rangeMax = originData->GetScalarRange()[1];
            contourFilter->SetInputData(this->DatasetMapper->GetInput());
            this->contourFilter->GenerateValues(num, rangeMin, rangeMax); // todo：极值的bug
            contourFilter->Update();                                    // 要想打印日志必须调用Update或者render来触发update
            contourFilter->GetOutput()->PrintSelf(std::cout, (vtkIndent)2);

            // Mapper
            contourMapper->SetInputConnection(contourFilter->GetOutputPort());
            contourMapper->ScalarVisibilityOn();
            contourMapper->SetLookupTable(DatasetMapper->GetLookupTable());
            contourMapper->SetScalarModeToUsePointData();
            contourMapper->UseLookupTableScalarRangeOn(); // 代替SetScalarRange()方法，与lookupTable使用相同的ScalarRange;

            // actor
            contourActor->SetMapper(contourMapper);
            contourActor->GetProperty()->SetLineWidth(3);

            this->CurrentRenderer->AddActor(contourActor);
            // 等值线(结束)--lius
            this->GetInteractor()->GetRenderWindow()->Render();
            this->HighlightProp(NULL);

            // 给等值线图上的点显示编号
            {
                vtkSmartPointer<vtkLabeledDataMapper> nodeLabelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
                nodeLabelMapper->SetInputData(contourFilter->GetOutput());
                nodeLabelMapper->SetLabelFormat("%d");  // 设置格式为整数
                vtkSmartPointer<vtkActor2D> actor2D = vtkSmartPointer<vtkActor2D>::New();
                nodeLabelMapper->GetLabelTextProperty()->SetColor(0.0, 1.0, 0.0);
                actor2D->SetMapper(nodeLabelMapper);
                // this->CurrentRenderer->AddActor(actor2D);
            }

            labelContourLine();
        }

        void labelContourLine() {
            // 等值线图上的标签
            {
            // 方案一：所有点都显示数值标签
            // vtkSmartPointer<vtkLabeledDataMapper> labelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
            // labelMapper->SetInputData(contourFilter->GetOutput());
            // labelMapper->SetFieldDataName("zzy");
            // labelMapper->SetLabelModeToLabelFieldData();
            // labelMapper->SetLabelFormat("%0.3e");
            // labelMapper->GetLabelTextProperty()->SetFontSize(10);
            // labelMapper->GetLabelTextProperty()->ItalicOff();
            // labelMapper->GetLabelTextProperty()->SetColor(1.0, 0.0, 0.0);
            // vtkSmartPointer<vtkActor2D> actor2D = vtkSmartPointer<vtkActor2D>::New();
            // actor2D->SetMapper(labelMapper);
            // this->CurrentRenderer->AddActor(actor2D);
            }


            // 方案二：间隔指定点显示标签
            auto data = contourFilter->GetOutput();
            vtkNew<vtkStripper> theStripper;
            theStripper->SetInputData(data);
            theStripper->Update();
            auto stripperResult = theStripper->GetOutput();
            vtkNew<vtkPolyData> labelPolyData;
            vtkNew<vtkPoints> labelPoints;
            vtkNew<vtkCharArray> labelScalars;
            labelScalars->SetNumberOfComponents(1);
            labelScalars->SetName("value_zzy");
            vtkDataArray* scalars = data->GetPointData()->GetScalars();
            vtkPoints* points = data->GetPoints();

            vtkNew<vtkGenericCell> cell;
            for (vtkIdType cellId = 0; cellId < stripperResult->GetNumberOfCells(); ++cellId) { // 遍历stripper里的单元
                stripperResult->GetCell(cellId, cell); // 拿到每一个单元
                // 获取该单元所有节点对应的label字符
                vtkIdType firstPointIdOfCell = cell->GetPointId(0); // 单元里的第一个节点id：该id编号需要与等值线图编号一致
                double firstPointContourValue = scalars->GetTuple1(firstPointIdOfCell); // 第一个节点对应的等值
                char firstPointLabel = getCorrespondLabelOfContourValue(firstPointContourValue); // 获取值对应的标注字符
                // 每隔一定个数写一个label字符
                double pointCoord[3];
                for (vtkIdType pointIdIndex = 0; pointIdIndex < cell->GetNumberOfPoints(); ++pointIdIndex) {
                    vtkIdType pointId = cell->GetPointId(pointIdIndex);
                    if (pointId % 2 != 0) { // 隔5个点标一个数值
                        continue;
                    }
                    points->GetPoint(pointId, pointCoord);
                    labelPoints->InsertNextPoint(pointCoord);
                    labelScalars->InsertNextTuple1(firstPointLabel);
                }
            }
            labelPolyData->SetPoints(labelPoints);
            labelPolyData->GetPointData()->SetScalars(labelScalars);
            vtkSmartPointer<vtkLabeledDataMapper> labelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
            labelMapper->SetInputData(labelPolyData);
            labelMapper->SetFieldDataName("value_zzy");
            labelMapper->SetLabelModeToLabelFieldData();

            vtkSmartPointer<vtkActor2D> actor2D = vtkSmartPointer<vtkActor2D>::New();
            actor2D->SetMapper(labelMapper);
            this->CurrentRenderer->AddActor(actor2D);
        }


        char getCorrespondLabelOfContourValue(double contourValue) {
            auto originData = this->DatasetMapper->GetInput();
            double rangeMin = originData->GetScalarRange()[0];
            double rangeMax = originData->GetScalarRange()[1];
            double each = (rangeMax - rangeMin) / (num - 1); // 一个增长量的大小
            std::uint8_t num_id = std::round((contourValue - rangeMin) / each); // 四舍五入得到当前等值参数对应的下标
            return num_id + 'A';
        }

        void OnChar() override
        {
            vtkRenderWindowInteractor *rwi = this->Interactor;

            switch (rwi->GetKeyCode())
            {
            case 'w':
            case 'W':
                this->ModelActor->VisibilityOff(); // 隐藏之前模型
                break;
            case 's':
            case 'S':
                this->ModelActor->VisibilityOn(); // 显示之前模型
                break;
            case 'r':
            case 'R':
                this->contourFilter->SetValue(1, 0);
                this->contourFilter->SetValue(2, -0.5);
                break;

            case 'X':
            case 'x':
            {
                this->contourFilter->GenerateValues(5, -1, 1);
                break;
            }

            default:
                this->Superclass::OnChar();
                break;
            }
            this->GetInteractor()->GetRenderWindow()->Render();
            this->HighlightProp(NULL);
        }

        void SetDatasetMapper(vtkSmartPointer<vtkDataSetMapper> mapper)
        {
            this->DatasetMapper = mapper;
        }
        void SetActor(vtkSmartPointer<vtkActor> actor)
        {
            this->ModelActor = actor;
        }

    private:
        // 外部传入：
        vtkSmartPointer<vtkDataSetMapper> DatasetMapper;
        vtkSmartPointer<vtkActor> ModelActor;
        // 内部自定义：
        vtkSmartPointer<vtkPolyDataMapper> contourMapper;
        vtkSmartPointer<vtkActor> contourActor;
        vtkSmartPointer<vtkContourFilter> contourFilter;
    };

    vtkStandardNewMacro(InteractorStyle);
} // namespace

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkUnstructuredGrid> grid; /* 创建一个非结构化网格对象 */
    /*=============================网格节点=============================*/
    vtkNew<vtkPoints> meshPoints; /* 用于存储网格节点 */
    meshPoints->InsertNextPoint(20, 0, 0);
    meshPoints->InsertNextPoint(17.320508109792, 10.0000000321997, 0);
    meshPoints->InsertNextPoint(10.0000000084796, 17.3205081212641, 0);
    meshPoints->InsertNextPoint(0, 20, 0);
    meshPoints->InsertNextPoint(-10.0000000245946, 17.3205081142845, 0);
    meshPoints->InsertNextPoint(-17.320508106976, 10.0000000330863, 0);
    meshPoints->InsertNextPoint(-20, 0, 0);
    meshPoints->InsertNextPoint(-17.3205081361632, -9.99999998652343, 0);
    meshPoints->InsertNextPoint(-10.0000000640139, -17.3205080893384, 0);
    meshPoints->InsertNextPoint(0, -20, 0);
    meshPoints->InsertNextPoint(9.99999997546481, -17.3205081119126, 0);
    meshPoints->InsertNextPoint(17.3205080670077, -10.000000025522, 0);
    meshPoints->InsertNextPoint(-7.15966651257561, -7.12683602128246, 0);
    meshPoints->InsertNextPoint(-2.63739358105485, -12.9978938464381, 0);
    meshPoints->InsertNextPoint(-9.10592684278358, -0.737849316405346, 0);
    meshPoints->InsertNextPoint(6.59947131139743, -9.08284206416132, 0);
    meshPoints->InsertNextPoint(-0.137734590259206, -2.71811814272003, 0);
    meshPoints->InsertNextPoint(-7.75933677195531, 5.80947235781205, 0);
    meshPoints->InsertNextPoint(9.11726442880688, -1.21602962482667, 0);
    meshPoints->InsertNextPoint(-0.484322809037511, 4.65580556604978, 0);
    meshPoints->InsertNextPoint(-3.48354780828141, 10.4142340529639, 0);
    meshPoints->InsertNextPoint(7.28614335171517, 6.31993800133739, 0);
    meshPoints->InsertNextPoint(3.36376652373578, 13.053010718073, 0);
    grid->SetPoints(meshPoints);
    /*=================================================================*/

    /*=============================设置单元=============================*/
    vtkNew<vtkIdList> pointsId;
    pointsId->SetNumberOfIds(3);
    pointsId->SetId(0, 9);
    pointsId->SetId(1, 13);
    pointsId->SetId(2, 8);
    grid->InsertNextCell(VTK_TRIANGLE, 3, pointsId->GetPointer(0));

    pointsId->Initialize();
    pointsId->SetNumberOfIds(3);
    pointsId->SetId(0, 14);
    pointsId->SetId(1, 12);
    pointsId->SetId(2, 16);
    grid->InsertNextCell(VTK_TRIANGLE, 3, pointsId->GetPointer(0));

    pointsId->Initialize();
    pointsId->SetNumberOfIds(3);
    pointsId->SetId(0, 11);
    pointsId->SetId(1, 15);
    pointsId->SetId(2, 10);
    grid->InsertNextCell(VTK_TRIANGLE, 3, pointsId->GetPointer(0));

    pointsId->Initialize();
    pointsId->SetNumberOfIds(3);
    pointsId->SetId(0, 15);
    pointsId->SetId(1, 18);
    pointsId->SetId(2, 16);
    grid->InsertNextCell(VTK_TRIANGLE, 3, pointsId->GetPointer(0));

    pointsId->Initialize();
    pointsId->SetNumberOfIds(3);
    pointsId->SetId(0, 17);
    pointsId->SetId(1, 19);
    pointsId->SetId(2, 20);
    grid->InsertNextCell(VTK_TRIANGLE, 3, pointsId->GetPointer(0));

    pointsId->Initialize();
    pointsId->SetNumberOfIds(3);
    pointsId->SetId(0, 3);
    pointsId->SetId(1, 22);
    pointsId->SetId(2, 2);
    grid->InsertNextCell(VTK_TRIANGLE, 3, pointsId->GetPointer(0));

    pointsId->Initialize();
    pointsId->SetNumberOfIds(4);
    pointsId->SetId(0, 8);
    pointsId->SetId(1, 13);
    pointsId->SetId(2, 12);
    pointsId->SetId(3, 7);
    grid->InsertNextCell(VTK_QUAD, 4, pointsId->GetPointer(0));

    pointsId->Initialize();
    pointsId->SetNumberOfIds(4);
    pointsId->SetId(0, 12);
    pointsId->SetId(1, 14);
    pointsId->SetId(2, 6);
    pointsId->SetId(3, 7);
    grid->InsertNextCell(VTK_QUAD, 4, pointsId->GetPointer(0));

    pointsId->Initialize();
    pointsId->SetNumberOfIds(4);
    pointsId->SetId(0, 13);
    pointsId->SetId(1, 9);
    pointsId->SetId(2, 10);
    pointsId->SetId(3, 15);
    grid->InsertNextCell(VTK_QUAD, 4, pointsId->GetPointer(0));

    pointsId->Initialize();
    pointsId->SetNumberOfIds(4);
    pointsId->SetId(0, 6);
    pointsId->SetId(1, 14);
    pointsId->SetId(2, 17);
    pointsId->SetId(3, 5);
    grid->InsertNextCell(VTK_QUAD, 4, pointsId->GetPointer(0));

    pointsId->Initialize();
    pointsId->SetNumberOfIds(4);
    pointsId->SetId(0, 16);
    pointsId->SetId(1, 12);
    pointsId->SetId(2, 13);
    pointsId->SetId(3, 15);
    grid->InsertNextCell(VTK_QUAD, 4, pointsId->GetPointer(0));

    pointsId->Initialize();
    pointsId->SetNumberOfIds(4);
    pointsId->SetId(0, 14);
    pointsId->SetId(1, 16);
    pointsId->SetId(2, 19);
    pointsId->SetId(3, 17);
    grid->InsertNextCell(VTK_QUAD, 4, pointsId->GetPointer(0));

    pointsId->Initialize();
    pointsId->SetNumberOfIds(4);
    pointsId->SetId(0, 17);
    pointsId->SetId(1, 20);
    pointsId->SetId(2, 4);
    pointsId->SetId(3, 5);
    grid->InsertNextCell(VTK_QUAD, 4, pointsId->GetPointer(0));

    pointsId->Initialize();
    pointsId->SetNumberOfIds(4);
    pointsId->SetId(0, 16);
    pointsId->SetId(1, 18);
    pointsId->SetId(2, 21);
    pointsId->SetId(3, 19);
    grid->InsertNextCell(VTK_QUAD, 4, pointsId->GetPointer(0));

    pointsId->Initialize();
    pointsId->SetNumberOfIds(4);
    pointsId->SetId(0, 15);
    pointsId->SetId(1, 11);
    pointsId->SetId(2, 0);
    pointsId->SetId(3, 18);
    grid->InsertNextCell(VTK_QUAD, 4, pointsId->GetPointer(0));

    pointsId->Initialize();
    pointsId->SetNumberOfIds(4);
    pointsId->SetId(0, 20);
    pointsId->SetId(1, 19);
    pointsId->SetId(2, 21);
    pointsId->SetId(3, 22);
    grid->InsertNextCell(VTK_QUAD, 4, pointsId->GetPointer(0));

    pointsId->Initialize();
    pointsId->SetNumberOfIds(4);
    pointsId->SetId(0, 22);
    pointsId->SetId(1, 3);
    pointsId->SetId(2, 4);
    pointsId->SetId(3, 20);
    grid->InsertNextCell(VTK_QUAD, 4, pointsId->GetPointer(0));

    pointsId->Initialize();
    pointsId->SetNumberOfIds(4);
    pointsId->SetId(0, 21);
    pointsId->SetId(1, 18);
    pointsId->SetId(2, 0);
    pointsId->SetId(3, 1);
    grid->InsertNextCell(VTK_QUAD, 4, pointsId->GetPointer(0));

    pointsId->Initialize();
    pointsId->SetNumberOfIds(4);
    pointsId->SetId(0, 2);
    pointsId->SetId(1, 22);
    pointsId->SetId(2, 21);
    pointsId->SetId(3, 1);
    grid->InsertNextCell(VTK_QUAD, 4, pointsId->GetPointer(0));
    /*=================================================================*/

    /*==============================设置节点颜色========================*/
    vtkNew<vtkFloatArray> color; /* 用于设置每个节点的随机颜色 */
    for (int i = 0; i < grid->GetNumberOfPoints(); ++i)
    {
        color->InsertNextValue(std::rand());
    }
    color->SetName("zzy");
    grid->GetPointData()->AddArray(color);
    grid->GetPointData()->SetScalars(grid->GetPointData()->GetArray("zzy"));
    /*=================================================================*/

    // vtkDataSetMapper
    vtkNew<vtkDataSetMapper> dataSetMapper;
    dataSetMapper->SetInputData(grid);
    dataSetMapper->ScalarVisibilityOff();
    dataSetMapper->UseLookupTableScalarRangeOn(); // 代替SetScalarRange()方法，与lookupTable使用相同的ScalarRange;
    dataSetMapper->SetScalarModeToUsePointData(); // 使用结点数据

    // 颜色映射表
    vtkSmartPointer<vtkLookupTable> pColorTableModel = vtkSmartPointer<vtkLookupTable>::New();
    pColorTableModel->SetNumberOfColors(num);
    pColorTableModel->SetHueRange(0.67, 0); // 色调从蓝色到绿色到红色，【0，0.67】则表示从红色到蓝红色
    pColorTableModel->SetValueRange(0.7529, 0.7059);
    pColorTableModel->SetSaturationRange(0.69, 0.97);
    double rangeMin = grid->GetScalarRange()[0];
    double rangeMax = grid->GetScalarRange()[1];
    pColorTableModel->SetTableRange(rangeMin, rangeMax); // 最大值最小值
    pColorTableModel->Build();

    dataSetMapper->SetLookupTable(pColorTableModel);
    dataSetMapper->SetInterpolateScalarsBeforeMapping(1); // 显示有颜色界限的云图

    // 渲染网格的actor
    vtkSmartPointer<vtkActor> actorGrid = vtkSmartPointer<vtkActor>::New();
    actorGrid->SetMapper(dataSetMapper);
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
    scalarBarActor->SetNumberOfLabels(num);
    // scalarBarActor->SetLabelFormat("%!f(string=2023)");//%-#6.3g
    scalarBarActor->SetLabelFormat("%.3e"); //

    // 显示points标量值
    vtkSmartPointer<vtkLabeledDataMapper> nodeLabelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
    nodeLabelMapper->SetInputData(grid);
    nodeLabelMapper->SetFieldDataName("zzy");
    nodeLabelMapper->SetLabelModeToLabelFieldData();
    vtkSmartPointer<vtkActor2D> actor2D2 = vtkSmartPointer<vtkActor2D>::New();
    actor2D2->SetMapper(nodeLabelMapper);

    // Visualize
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("set_vtkContourFilter");
    renderWindow->SetSize(900, 600);
    renderer->AddActor(scalarBarActor);
    renderer->AddActor(actorGrid);
    // renderer->AddActor(actor2D2);

    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
    // 交互
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkNew<InteractorStyle> style;
    // style->StartSelect(); // 开始选择按键的设置
    style->SetDatasetMapper(dataSetMapper);
    style->SetActor(actorGrid);
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}