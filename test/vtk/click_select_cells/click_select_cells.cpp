/**
 * @brief 点选单元拾取，支持多次增选
 */
#include <iostream>
#include <set>
#include <vtkAbstractPicker.h>
#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkAutoInit.h>
#include <vtkCellPicker.h>
#include <vtkCylinderSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkExtractGeometry.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkHexahedron.h>
#include <vtkExtractSelection.h>
#include <vtkIdFilter.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLabeledDataMapper.h>
#include <vtkLongArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataWriter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTextProperty.h>
#include <vtkTriangle.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVertexGlyphFilter.h>
#include <string>
#include <vector>
using namespace std;

class InteractorStyle : public vtkInteractorStyleTrackballCamera {
public:
    static InteractorStyle* New();
    vtkTypeMacro(InteractorStyle, vtkInteractorStyleTrackballCamera);

    InteractorStyle();

    std::vector<uint32_t> pick_element();
    virtual void OnLeftButtonDown() override;
    void set_points(vtkSmartPointer<vtkUnstructuredGrid> points);

private:
    vtkSmartPointer<vtkUnstructuredGrid> points;
    std::vector<vtkSmartPointer<vtkActor>> selected_actors;
    std::vector<vtkSmartPointer<vtkDataSetMapper>> selected_mappers;
    std::set<int> selected_ids;
};

vtkStandardNewMacro(InteractorStyle);

InteractorStyle::InteractorStyle() {
}

void InteractorStyle::OnLeftButtonDown() {
    //事件转发
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();

    //鼠标点击位置(x,y)
    int* pos = this->GetInteractor()->GetEventPosition();

    auto picker = this->Interactor->GetPicker();
    //Pick(x, y, z, vtkRenderer)
    picker->Pick(pos[0], pos[1], 0,
                 this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

    //点击点的世界坐标
    double world_pos[3];
    picker->GetPickPosition(world_pos);

    auto cell_picker = vtkCellPicker::SafeDownCast(picker);
    this->selected_ids.insert(cell_picker->GetCellId());
    // std::cout << "Pick Position: [" << worldPos[0] << ", " << worldPos[1] << ", " << worldPos[2] << "]" << std::endl
    //           << "Cell Id: " << cellpicker->GetCellId() << std::endl
    //           << "Point Id: " << cellpicker->GetPointId() << std::endl;
    if (cell_picker->GetCellId() != -1) {
        auto cell = points->GetCell(cell_picker->GetCellId());
        // std::cout << "Cell Class Name: " << cell->GetClassName() << std::endl;
        vtkNew<vtkUnstructuredGrid> selected_cells;
        vtkNew<vtkDataSetMapper> mapper;
        vtkNew<vtkActor> actor;
        vtkNew<vtkCellArray> cell_array;
        vtkNew<vtkPoints> point_list;
        vtkNew<vtkIdList> id_list;
        auto points = cell->GetPoints();
        auto num = points->GetNumberOfPoints();
        double p[3];
        for (int i = 0; i < num; i++) {
            points->GetPoint(i, p);
            id_list->InsertNextId(point_list->InsertNextPoint(p)); // 组装point同时组装对应的cel
        }
        selected_cells->SetPoints(point_list);
        selected_cells->InsertNextCell(cell->GetCellType(), id_list);

        mapper->SetInputData(selected_cells);
        actor->SetMapper(mapper);
        actor->GetProperty()->EdgeVisibilityOn();

        actor->GetProperty()->SetEdgeVisibility(false); // 修复显示条形颜色

        actor->GetProperty()->SetColor(1.0, 1.0, 0.0);
        Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);
        this->selected_actors.push_back(actor);
        this->selected_mappers.push_back(mapper);
        for (auto id : selected_ids) {
            std::cout << id << std::endl;
        }
        std::cout << std::endl;
    }
}

void InteractorStyle::set_points(vtkSmartPointer<vtkUnstructuredGrid> points) {
    this->points = points;
}

int main(int, char*[]) {
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

    // 数据集添加属性：用于label拾取的属性
    auto count = grid->GetNumberOfPoints(); // 16
    long* arr = new long[count];
    for (int i = 0; i < count; i++) {
        arr[i] = i;
    }
    vtkNew<vtkLongArray> point_index;
    point_index->SetArray(arr, count, 1);
    point_index->SetName("zzy");
    grid->GetPointData()->AddArray(point_index);

    // 创建mapper：显示带label属性的actor，框选时才能知道选中了哪些label
    vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper->SetInputData(grid);
    // mapper->ScalarVisibilityOn();

    //  创建actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1.0, 1.0, 1.0);
    actor->GetProperty()->SetOpacity(1);
    actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

    // Visualize
    vtkNew<vtkRenderer> renderer;
    //如果该标志为On并且渲染引擎支持，则线框模式下几何图形的隐藏先会被移除
    //renderer->UseHiddenLineRemovalOn();
    vtkNew<vtkRenderWindow> render_window;
    render_window->AddRenderer(renderer);
    render_window->SetWindowName("HighlightSelectedCells");
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

    // 拾取功能开始：
    vtkNew<vtkCellPicker> picker;
    vtkNew<vtkRenderWindowInteractor> render_window_interactor;
    render_window_interactor->SetPicker(picker);
    render_window_interactor->SetRenderWindow(render_window);

    vtkNew<InteractorStyle> style;
    style->set_points(grid);
    style->SetDefaultRenderer(renderer);
    render_window_interactor->SetInteractorStyle(style);
    render_window->Render();
    render_window_interactor->Start();

    return EXIT_SUCCESS;
}
