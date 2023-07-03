/**
 * @brief 云图：显示单元上的标量数据
 * @brief 线图：显示单元上的标量数据
 * @date 2023-07-03
 */

#include "vtkCharArray.h"
#include "vtkGenericCell.h"
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
#include <vtkCellDataToPointData.h>
#include <vtkContourFilter.h>
#include <vtkCutter.h>
#include <vtkCylinderSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
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
#include <vtkStripper.h>
#include <vtkTextProperty.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVersion.h>
#include <vtkVertex.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkTypeUInt32Array.h>
#include <vtkSelectVisiblePoints.h>

using namespace std;
const std::uint32_t num = 7;

namespace
{

// Define interaction style
class InteractorStyle : public vtkInteractorStyleRubberBandPick {
public:
    static InteractorStyle* New();
    vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);
    // 构造函数,实例化内部的成员
    InteractorStyle() {
        contourFilter = vtkSmartPointer<vtkContourFilter>::New();
        contourActor = vtkSmartPointer<vtkActor>::New();
        contourMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    }

    virtual void OnLeftButtonUp() override {
        // Forward events
        vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
    }

    virtual void OnRightButtonUp() override {
        // Forward events
        vtkInteractorStyleRubberBandPick::OnRightButtonUp();

        // 等值线(开始)--lius
        auto originData = this->datasetMapper_->GetInput();
        double rangeMin = originData->GetScalarRange()[0];
        double rangeMax = originData->GetScalarRange()[1];
        contourFilter->SetInputData(this->datasetMapper_->GetInput());
        this->contourFilter->GenerateValues(num + 2, rangeMin, rangeMax);
        contourFilter->Update(); // 要想打印日志必须调用Update或者render来触发update
        contourFilter->GetOutput()->PrintSelf(std::cout, (vtkIndent)2);

        // Mapper
        contourMapper->SetInputConnection(contourFilter->GetOutputPort());
        contourMapper->ScalarVisibilityOn();
        contourMapper->SetLookupTable(datasetMapper_->GetLookupTable());
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
            nodeLabelMapper->SetLabelFormat("%d"); // 设置格式为整数
            vtkSmartPointer<vtkActor2D> actor2D = vtkSmartPointer<vtkActor2D>::New();
            nodeLabelMapper->GetLabelTextProperty()->SetColor(0.0, 1.0, 0.0);
            actor2D->SetMapper(nodeLabelMapper);
            // this->CurrentRenderer->AddActor(actor2D);
        }

        labelContourLine();
        setScalarBarAnnotation(); // 设置图例的左边注释
    }

    void labelContourLine() {
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
            stripperResult->GetCell(cellId, cell);                                          // 拿到每一个单元
            // 获取该单元所有节点对应的label字符
            vtkIdType firstPointIdOfCell = cell->GetPointId(0);                              // 单元里的第一个节点id：该id编号需要与等值线图编号一致
            double firstPointContourValue = scalars->GetTuple1(firstPointIdOfCell);          // 第一个节点对应的等值
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
        auto originData = this->datasetMapper_->GetInput();
        double rangeMin = originData->GetScalarRange()[0];
        double rangeMax = originData->GetScalarRange()[1];
        double each = (rangeMax - rangeMin) / (num - 1);                    // 一个增长量的大小
        std::uint8_t num_id = std::round((contourValue - rangeMin) / each); // 四舍五入得到当前等值参数对应的下标
        return num_id + 'A';
    }

    void setScalarBarAnnotation() {
        auto lut = datasetMapper_->GetLookupTable();
        double temp = 0;
        std::string label;
        for (std::uint8_t index = 0; index < 5; index++) {
            temp += 3;
            label = static_cast<char>('A' + index);
            label += "=";
            lut->SetAnnotation(vtkVariant(temp), label);
        }
    }

    void OnChar() override {
        vtkRenderWindowInteractor* rwi = this->Interactor;
        auto lut = vtkLookupTable::SafeDownCast(datasetMapper_->GetLookupTable());
        switch (rwi->GetKeyCode()) {
            case 'w':
            case 'W':
                // 显示节点的云图
                datasetMapper_->SetScalarModeToUsePointData(); // 使用结点数据
                lut->SetTableRange(0, 26); // 最大值最小值 节点共26个
                break;
            case 's':
            case 'S':
                // 显示单元的云图
                datasetMapper_->SetScalarModeToUseCellData(); // 使用单元数据
                lut->SetTableRange(0, 7); // 最大值最小值 单元共26个
                break;
            case 'r':
            case 'R':
                this->contourFilter->SetValue(1, 0);
                this->contourFilter->SetValue(2, -0.5);
                break;

            case 'X':
            case 'x': {
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

    void SetDatasetMapper(vtkSmartPointer<vtkDataSetMapper> mapper) {
        this->datasetMapper_ = mapper;
    }
    void SetActor(vtkSmartPointer<vtkActor> actor) {
        this->ModelActor = actor;
    }

private:
    // 外部传入：
    vtkSmartPointer<vtkDataSetMapper> datasetMapper_;
    vtkSmartPointer<vtkActor> ModelActor;
    // 内部自定义：
    vtkSmartPointer<vtkPolyDataMapper> contourMapper;
    vtkSmartPointer<vtkActor> contourActor;
    vtkSmartPointer<vtkContourFilter> contourFilter;
};

vtkStandardNewMacro(InteractorStyle);
} // namespace

int main(int, char*[]) {
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetWindowName("set_vtkContourFilter");
    renderWindow->SetSize(900, 600);
    vtkNew<vtkRenderer> renderer;
    renderWindow->AddRenderer(renderer);
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (int y = -1; y <= 1; y++)         //循环y,-1到1
    {
        for (int z = 10; z >= 8; z--)     //循环z,1到-1
        {
            for (int x = -1; x <= 1; x++) //循环x,-1到1
            {
                points->InsertNextPoint(x * 10, y * 10, z * 10);
            }
        }
    }

    vtkSmartPointer<vtkUnstructuredGrid> grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    {                              //27个结点组成八个单元
        int num[] = {0, 1, 4, 3};  // 1 2 5 4,减一
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
    /*=================================================================*/

    /*==============================设置节点颜色========================*/
    vtkNew<vtkTypeUInt32Array> color_node; /* 用于设置每个节点的随机颜色 */
    for (int i = 0; i < grid->GetNumberOfPoints(); ++i) {
        color_node->InsertNextValue(i);
    }
    color_node->SetName("node_scalar");
    grid->GetPointData()->AddArray(color_node);
    grid->GetPointData()->SetScalars(grid->GetPointData()->GetArray("node_scalar"));
    /*=================================================================*/

    /*==============================设置单元颜色========================*/
    vtkNew<vtkTypeUInt32Array> color_element; /* 用于设置每个节点的随机颜色 */
    for (int i = 0; i < grid->GetNumberOfCells(); ++i) {
        color_element->InsertNextValue(i);
    }
    color_element->SetName("element_scalar");
    grid->GetCellData()->AddArray(color_element);
    grid->GetCellData()->SetScalars(grid->GetCellData()->GetArray("element_scalar"));
    /*=================================================================*/

    // vtkDataSetMapper
    vtkNew<vtkDataSetMapper> dataSetMapper;
    dataSetMapper->SetInputData(grid);
    dataSetMapper->ScalarVisibilityOn();
    dataSetMapper->UseLookupTableScalarRangeOn(); // 代替SetScalarRange()方法，与lookupTable使用相同的ScalarRange;
    dataSetMapper->SetScalarModeToUsePointData(); // 使用结点数据

    // 颜色映射表
    vtkSmartPointer<vtkLookupTable> pColorTableModel = vtkSmartPointer<vtkLookupTable>::New();
    pColorTableModel->SetNumberOfColors(num + 1);
    pColorTableModel->SetHueRange(0.67, 0); // 色调从蓝色到绿色到红色，【0，0.67】则表示从红色到蓝红色
    pColorTableModel->SetValueRange(0.7529, 0.7059);
    pColorTableModel->SetSaturationRange(0.69, 0.97);
    // double rangeMin = grid->GetScalarRange()[0]; // 获取的最大最小值包括当前节点标量数据与单元标量数据，因此不好用
    // double rangeMax = grid->GetScalarRange()[1];
    pColorTableModel->SetTableRange(0, 26); // 最大值最小值：默认使用节点的
    pColorTableModel->Build();

    dataSetMapper->SetLookupTable(pColorTableModel);
    dataSetMapper->SetInterpolateScalarsBeforeMapping(1); // 显示有颜色界限的云图

    // 渲染网格的actor
    vtkSmartPointer<vtkActor> actorGrid = vtkSmartPointer<vtkActor>::New();
    actorGrid->SetMapper(dataSetMapper);
    actorGrid->GetProperty()->SetLineWidth(1);
    actorGrid->GetProperty()->SetEdgeVisibility(1);

    // 设置scalarBar（色卡）
    vtkSmartPointer<vtkScalarBarActor> scalarBarActor = vtkSmartPointer<vtkScalarBarActor>::New();
    scalarBarActor->SetLookupTable(pColorTableModel);

    scalarBarActor->SetTitle("Height");
    scalarBarActor->GetTitleTextProperty()->ShadowOff(); // “标题”关闭阴影
    scalarBarActor->GetLabelTextProperty()->ShadowOff(); // “标签”关闭阴影
    scalarBarActor->GetTitleTextProperty()->ItalicOff(); // “标题”关闭斜体
    scalarBarActor->GetLabelTextProperty()->ItalicOff(); // “标签”关闭斜体
    scalarBarActor->GetTitleTextProperty()->BoldOff();   // “标题”不加粗
    scalarBarActor->GetLabelTextProperty()->BoldOff();   // “标签”不加粗
    scalarBarActor->SetNumberOfLabels(num + 2);
    // scalarBarActor->SetLabelFormat("%!f(string=2023)");//%-#6.3g
    scalarBarActor->SetLabelFormat("%.3ecm"); // 设置统一后缀

    // 显示points标量值
    vtkSmartPointer<vtkSelectVisiblePoints> visPtsNode = vtkSmartPointer<vtkSelectVisiblePoints>::New();
    visPtsNode->SetInputData(grid);
    visPtsNode->SetRenderer(renderer);
    vtkSmartPointer<vtkLabeledDataMapper> nodeLabelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
    nodeLabelMapper->SetInputConnection(visPtsNode->GetOutputPort());
    nodeLabelMapper->SetFieldDataName("node_scalar");
    nodeLabelMapper->SetLabelModeToLabelFieldData();
    nodeLabelMapper->SetLabelFormat("%d"); // 设置格式为整数
    vtkSmartPointer<vtkActor2D> actor2D_node = vtkSmartPointer<vtkActor2D>::New();
    actor2D_node->SetMapper(nodeLabelMapper);

    // 显示cells标量值
    vtkSmartPointer<vtkCellCenters> cellCenters = vtkSmartPointer<vtkCellCenters>::New();
    cellCenters->SetInputData(grid);
    vtkSmartPointer<vtkSelectVisiblePoints> visPtsElement = vtkSmartPointer<vtkSelectVisiblePoints>::New();
    visPtsElement->SetInputConnection(cellCenters->GetOutputPort());
    visPtsElement->SetRenderer(renderer);
    vtkSmartPointer<vtkLabeledDataMapper> cellLabelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
    cellLabelMapper->SetInputConnection(visPtsElement->GetOutputPort());
    cellLabelMapper->SetFieldDataName("element_scalar");
    cellLabelMapper->SetLabelModeToLabelFieldData();
    cellLabelMapper->SetLabelFormat("%ld"); // 设置格式为整数
    vtkSmartPointer<vtkActor2D> actor2D_element = vtkSmartPointer<vtkActor2D>::New();
    cellLabelMapper->GetLabelTextProperty()->SetColor(0.0, 1.0, 0.0);
    actor2D_element->SetMapper(cellLabelMapper);

    // Visualize
    renderer->AddActor(scalarBarActor);
    renderer->AddActor(actorGrid);
    renderer->AddActor(actor2D_element);
    renderer->AddActor(actor2D_node);


    vtkNew<InteractorStyle> style;
    style->SetDatasetMapper(dataSetMapper);
    style->SetActor(actorGrid);
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
