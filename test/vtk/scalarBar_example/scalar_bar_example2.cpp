

/**
 * @brief 在zhangzy代码“给已存在的actor添加体数据用于颜色显示”基础上，测试颜色映射表的显示
 * @author lius
 * @date 2023-04-13
 */

/*
    验证流程：
    1、main函数设置初始参数
    2、左键生成cell的体数据，并作为cell的scalar数据存储，
    3、右键打印原始结构体信息，查看初始与当前信息变化。
*/

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAreaPicker.h>
#include <vtkCellCenters.h>
#include <vtkCellData.h>
#include <vtkColorTransferFunction.h>
#include <vtkCylinderSource.h>
#include <vtkDataSetMapper.h>
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
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVertex.h>

namespace
{
// Define interaction style
class InteractorStyle : public vtkInteractorStyleRubberBandPick {
public:
    static InteractorStyle *New();
    vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);

    InteractorStyle() {
        this->SelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
        this->SelectedActor = vtkSmartPointer<vtkActor>::New();
        this->SelectedActor->SetMapper(SelectedMapper);
    }

    /**
 * @brief 通过vtkColorTransferFunction类的SetColorSpaceToDiverging()函数实现两种饱和颜色之间插入白色，更改颜色查找表
 * 
 * @param bottomColor 色谱底端颜色
 * @param topColor 色谱顶端颜色
 * @param tableSize 色谱颜色份数
 */
    void SetColoredLookupTable(vtkSmartPointer<vtkLookupTable> lut, double bottomColor[3], double topColor[3], int tableSize = 12) {
        //定义颜色转换函数
        vtkSmartPointer<vtkColorTransferFunction> ctf = vtkSmartPointer<vtkColorTransferFunction>::New();
        ctf->SetColorSpaceToDiverging();                                       //Diverging颜色空间
        ctf->AddRGBPoint(0.0, bottomColor[0], bottomColor[1], bottomColor[2]); //色谱底端颜色
        ctf->AddRGBPoint(1.0, topColor[0], topColor[1], topColor[2]);          //色谱顶端颜色
        //更新颜色查找表
        lut->SetNumberOfTableValues(tableSize);
        //根据颜色转换函数设置颜色查找表
        for (auto i = 0; i < lut->GetNumberOfColors(); ++i) {
            std::array<double, 3> rgb; //定义rgb分量数组
            ctf->GetColor(static_cast<double>(i) / lut->GetNumberOfColors(),
                          rgb.data());                                   //在颜色转换函数的[0,1]范围内，得到插值后的颜色分量，传到rgb分量数组中
            std::array<double, 4> rgba{0.0, 0.0, 0.0, 1.0};              //定义rgba分量数组
            std::copy(std::begin(rgb), std::end(rgb), std::begin(rgba)); //rgb→rgba
            lut->SetTableValue(static_cast<vtkIdType>(i), rgba.data());  //设置每一个tableValue
        }
    }

    /**
     * @brief 更新自定义的颜色查找表的份数，仅限于cool to warm。不能将数组设置为函数的默认参数，才新建一个的。对吗？
     * 
     * @param lut 颜色查找表
     * @param tableSize 份数
     */
    void SetNumberOfLookupTableValues(vtkSmartPointer<vtkLookupTable> lut, int tableSize) {
        //定义颜色转换函数,设为Diverging颜色空间，定义为cool to warm
        vtkSmartPointer<vtkColorTransferFunction> ctf = vtkSmartPointer<vtkColorTransferFunction>::New();
        ctf->SetColorSpaceToDiverging();            //Diverging颜色空间
        ctf->AddRGBPoint(0.0, 0.230, 0.299, 0.754); //cool
        ctf->AddRGBPoint(1.0, 0.706, 0.016, 0.150); //warm

        //更新颜色查找表。不用提前判断要更新的份数是否是之前的份数，SetNumberOfTableValues()中已经写好了
        lut->SetNumberOfTableValues(tableSize);
        //根据颜色转换函数设置颜色查找表
        for (auto i = 0; i < lut->GetNumberOfColors(); ++i) {
            std::array<double, 3> rgb; //定义rgb分量数组
            ctf->GetColor(static_cast<double>(i) / lut->GetNumberOfColors(),
                          rgb.data());                                   //在颜色转换函数的[0,1]范围内，得到插值后的颜色分量，传到rgb分量数组中
            std::array<double, 4> rgba{0.0, 0.0, 0.0, 1.0};              //定义rgba分量数组
            std::copy(std::begin(rgb), std::end(rgb), std::begin(rgba)); //rgb→rgba
            lut->SetTableValue(static_cast<vtkIdType>(i), rgba.data());  //设置每一个tableValue
        }
    }

    virtual void OnRightButtonUp() override {
        vtkInteractorStyleRubberBandPick::OnRightButtonUp();
        actor->GetMapper()->PrintSelf(std::cout, vtkIndent(4));
        // 左键对原始结构添加体数据后，右键打印出actor里的信息会同步变化
        // actor->GetMapper()->GetInput()->PrintSelf(std::cout, vtkIndent(2));
        actor->GetMapper()->SetScalarVisibility(0);
        // actor->GetMapper()->SetScalarModeToUseCellData();
        this->GetInteractor()->GetRenderWindow()->Render();
    }

    virtual void OnLeftButtonUp() override {
        // Forward events
        vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
        {
            // 单元数据集添加属性：用于label拾取的属性
            auto count = polyData->GetNumberOfCells(); // 6
            long *arr = new long[count];
            for (int i = 0; i < count; i++) {
                arr[i] = i + 1;
            }
            vtkNew<vtkLongArray> pointIndex;
            pointIndex->SetArray(arr, count, 1);
            pointIndex->SetName("CellsIDSet");
            polyData->GetCellData()->AddArray(pointIndex);
        }
        {
            // 显示cell标量值
            vtkSmartPointer<vtkCellCenters> cellCenters = vtkSmartPointer<vtkCellCenters>::New();
            cellCenters->SetInputData(polyData);
            vtkSmartPointer<vtkLabeledDataMapper> cellLabelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
            cellLabelMapper->SetInputConnection(cellCenters->GetOutputPort());
            cellLabelMapper->SetFieldDataName("CellsIDSet");
            cellLabelMapper->SetLabelModeToLabelFieldData();

            cellLabelMapper->SetLabelFormat("%d"); // 设置格式为整数
            vtkSmartPointer<vtkActor2D> actor2D = vtkSmartPointer<vtkActor2D>::New();
            cellLabelMapper->GetLabelTextProperty()->SetColor(0.0, 1.0, 0.0);
            actor2D->SetMapper(cellLabelMapper);
            this->CurrentRenderer->AddActor(actor2D);
        }

        polyData->GetCellData()->SetScalars(polyData->GetCellData()->GetArray("CellsIDSet"));
        actor->GetMapper()->SetScalarVisibility(1);
        this->GetInteractor()->GetRenderWindow()->Render();
    }

    void SetPoints(vtkSmartPointer<vtkPolyData> polyData) {
        this->polyData = polyData;
    }

    void SetActor(vtkSmartPointer<vtkActor> actor) {
        this->actor = actor;
    }

    void SetColorBarActor(vtkSmartPointer<vtkScalarBarActor> scalarBarActor) {
        this->scalarBarActor_ = scalarBarActor;
    }
    void OnChar() override {
        vtkRenderWindowInteractor *rwi = this->Interactor;
        auto lut = vtkLookupTable::SafeDownCast(scalarBarActor_->GetLookupTable());
        switch (rwi->GetKeyCode()) {
        case 'w':
        case 'W':
            lut->SetHueRange(0, 0);
            lut->SetSaturationRange(0, 0);
            lut->SetValueRange(0, 1);
            break;
        case 's':
        case 'S':
            lut->SetHueRange(0.67, 0);
            lut->SetSaturationRange(1, 1);
            lut->SetValueRange(1, 1);
            break;

        case 'r':
        case 'R':
            // lut->SetNumberOfColors(20); // 指定色卡条目的个数
            SetNumberOfLookupTableValues(lut, 20);
            break;

        case 'X':
        case 'x': {                                        //括号必须要有，否则报错。why 152行可以没有？
            double bottomColor[3] = {0.230, 0.299, 0.754}; //0.230, 0.299, 0.754,cool蓝
            double topColor[3] = {0.706, 0.016, 0.150};    //0.706, 0.016, 0.150，warm红
            //通过vtkColorTransferFunction类的SetColorSpaceToDiverging()函数实现两种饱和颜色之间插入白色，更改颜色查找表
            SetColoredLookupTable(lut, bottomColor, topColor, 12);
            break;
        }

        default:
            this->Superclass::OnChar();
            break;
        }
    }

private:
    vtkSmartPointer<vtkPolyData> polyData;
    vtkSmartPointer<vtkActor> actor;
    vtkSmartPointer<vtkScalarBarActor> scalarBarActor_;
    vtkSmartPointer<vtkActor> SelectedActor;
    vtkSmartPointer<vtkDataSetMapper> SelectedMapper;
};

vtkStandardNewMacro(InteractorStyle);
} // namespace

int main(int, char *[]) {
    vtkNew<vtkNamedColors> colors;
    // 定义数据源:16-point 6-cell
    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetResolution(4);
    cylinder->Update();
    vtkPolyData *polyData = cylinder->GetOutput();

    // 创建mapper：显示带label属性的actor，框选时才能知道选中了哪些label
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polyData);
    mapper->ScalarVisibilityOn();
    mapper->SetScalarModeToUseCellData();

    vtkSmartPointer<vtkLookupTable> loopUpTable = vtkSmartPointer<vtkLookupTable>::New();
    loopUpTable->SetHueRange(0, 0.0);  // 设置色调范围，从蓝色到红色
    loopUpTable->SetSaturationRange(0, 0); // 设置饱和度范围
    loopUpTable->SetValueRange(0.3, 0.5);      // 设置亮度范围

    loopUpTable->SetTableRange(2, 6); // 指定标量对色卡的映射范围
    loopUpTable->SetBelowRangeColor(0.2, 0.2, 0.2, 1.0);
    loopUpTable->SetAboveRangeColor(0.9, 0.9, 0.9, 1.0); // 设置最大越界值颜色
    loopUpTable->SetUseAboveRangeColor(true);            // 设置使用最大越界值
    loopUpTable->SetUseBelowRangeColor(true);
    loopUpTable->SetNumberOfColors(6); // 指定色卡条目的个数
    loopUpTable->Build();
    mapper->SetLookupTable(loopUpTable);

    // Create a scalar bar
    vtkNew<vtkScalarBarActor> scalarBar;
    scalarBar->SetLookupTable(mapper->GetLookupTable());
    scalarBar->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
    scalarBar->UnconstrainedFontSizeOn();
    scalarBar->SetNumberOfLabels(6);
    scalarBar->SetDrawAboveRangeSwatch(true);                        // 设置图例的最大越界值色块(样块)显示
    scalarBar->SetDrawBelowRangeSwatch(true);                        // 设置图例的最小越界值色块显示
    scalarBar->SetBelowRangeAnnotation(std::to_string(1.0).c_str()); // 设置小于最小值(最小越界值)的注释
    scalarBar->SetLabelFormat("%0.3e");
    scalarBar->SetAboveRangeAnnotation("8");  // 设置大于最大值(最大越界值)的注释
    scalarBar->SetAnnotationLeaderPadding(0); // 注释的文本与颜色框间的引线长度
    scalarBar->GetAnnotationTextProperty()->SetJustificationToRight();
    scalarBar->GetLabelTextProperty()->SetJustificationToRight();
    scalarBar->SetTextPositionToSucceedScalarBar();
    scalarBar->SetPosition(0.85, 0.1);

    // TODO: 设置显示梯度参数
    mapper->SetInterpolateScalarsBeforeMapping(1);
    mapper->SetUseLookupTableScalarRange(true); // 使用LookupTable的最大最小值范围，不使用mapper的

    //  创建actor
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

    // Visualize
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("HighlightSelectedPoints");
    renderer->AddActor(actor);
    renderer->AddActor(scalarBar);
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

    // 拾取功能开始：
    vtkNew<vtkAreaPicker> areaPicker;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetPicker(areaPicker);

    vtkNew<InteractorStyle> style;
    style->SetPoints(polyData);
    style->SetActor(actor);
    style->SetColorBarActor(scalarBar);
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
