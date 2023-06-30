/**
 * @brief 给已存在的actor添加体数据用于颜色显示
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2022-12-07
 */

/*
    验证流程：
    1、main函数设置初始参数
    2、左键生成cell的体数据，并作为cell的scalar数据存储，
    3、右键打印原始结构体信息，查看初始与当前信息变化。
*/

#include <vtkLongArray.h>
#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkDataSetMapper.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleRubberBandPick.h>
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
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkLabeledDataMapper.h>
#include <vtkActor2D.h>
#include <vtkIndent.h>
#include <vtkCellCenters.h>
#include <vtkTextProperty.h>
#include <vtkCylinderSource.h>
#include <vtkVertex.h>
#include <vtkUnstructuredGrid.h>
#include <vtkCellData.h>
#include <vtkLookupTable.h>

namespace
{
// Define interaction style
class InteractorStyle : public vtkInteractorStyleRubberBandPick
{
public:
    static InteractorStyle *New();
    vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);

    InteractorStyle()
    {
        this->SelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
        this->SelectedActor = vtkSmartPointer<vtkActor>::New();
        this->SelectedActor->SetMapper(SelectedMapper);
    }
    virtual void OnRightButtonUp() override
    {
        vtkInteractorStyleRubberBandPick::OnRightButtonUp();
        actor->GetMapper()->PrintSelf(std::cout, vtkIndent(4));
        // 左键对原始结构添加体数据后，右键打印出actor里的信息会同步变化
        // actor->GetMapper()->GetInput()->PrintSelf(std::cout, vtkIndent(2));
        actor->GetMapper()->SetScalarVisibility(0);
        // actor->GetMapper()->SetScalarModeToUseCellData();
        this->GetInteractor()->GetRenderWindow()->Render();
    }
    virtual void OnLeftButtonUp() override
    {
        // Forward events
        vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
        {
            // 单元数据集添加属性：用于label拾取的属性
            auto count = polyData->GetNumberOfCells(); // 6
            long *arr = new long[count];
            for (int i = 0; i < count; i++)
            {
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

    void SetPoints(vtkSmartPointer<vtkPolyData> points)
    {
        this->polyData = points;
    }

    void SetActor(vtkSmartPointer<vtkActor> actor)
    {
        this->actor = actor;
    }

private:
    vtkSmartPointer<vtkPolyData> polyData;
    vtkSmartPointer<vtkActor> actor;
    vtkSmartPointer<vtkActor> SelectedActor;
    vtkSmartPointer<vtkDataSetMapper> SelectedMapper;
};

vtkStandardNewMacro(InteractorStyle);
} // namespace

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;
    // 定义数据源:16-point 6-cell
    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetResolution(4);
    cylinder->Update();
    vtkPolyData *polyData = cylinder->GetOutput();

    {
        // 点数据集添加属性：用于label拾取的属性
        auto count = polyData->GetNumberOfPoints(); // 16
        long *arr = new long[count];
        for (int i = 0; i < count; i++)
        {
            arr[i] = 33;
        }
        vtkNew<vtkLongArray> pointIndex;
        pointIndex->SetArray(arr, count, 1);
        pointIndex->SetName("PointsIDSet");
        polyData->GetPointData()->AddArray(pointIndex);
    }

    polyData->PrintSelf(std::cout, vtkIndent(2));

    // 创建mapper：显示带label属性的actor，框选时才能知道选中了哪些label
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polyData);
    mapper->ScalarVisibilityOn();
    mapper->SetScalarModeToUseCellData();

    vtkSmartPointer<vtkLookupTable> loopUpTable = vtkSmartPointer<vtkLookupTable>::New();
    // loopUpTable->SetNumberOfColors(11);
    loopUpTable->SetHueRange(0.67, 0);
    loopUpTable->SetTableRange(1, 6);
    loopUpTable->Build();
    mapper->SetLookupTable(loopUpTable);
    // TODO: 设置显示梯度参数
    mapper->SetInterpolateScalarsBeforeMapping(0);
    mapper->SetScalarRange(1, 6);

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
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

    // 拾取功能开始：
    vtkNew<vtkAreaPicker> areaPicker;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetPicker(areaPicker);

    vtkNew<InteractorStyle> style;
    style->SetPoints(polyData);
    style->SetActor(actor);
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    {
        // 显示节点标量值
        vtkSmartPointer<vtkLabeledDataMapper> nodeLabelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
        nodeLabelMapper->SetInputData(polyData);
        nodeLabelMapper->SetFieldDataName("PointsIDSet");
        nodeLabelMapper->SetLabelModeToLabelFieldData();

        nodeLabelMapper->SetLabelFormat("%d"); // 设置格式为整数
        vtkSmartPointer<vtkActor2D> actor2D = vtkSmartPointer<vtkActor2D>::New();
        nodeLabelMapper->GetLabelTextProperty()->SetColor(0.0, 1.0, 0.0);
        actor2D->SetMapper(nodeLabelMapper);
        renderer->AddActor(actor2D);
    }

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}