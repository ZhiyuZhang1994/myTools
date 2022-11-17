#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAreaPicker.h>
#include <vtkCellCenters.h>
#include <vtkCylinderSource.h>
#include <vtkDataSetMapper.h>
// #include <vtkDataSetSurfaceFilter.h>
#include <vtkExtractGeometry.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkIndent.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkLabeledDataMapper.h>
#include <vtkLongArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTextProperty.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVersion.h>
#include <vtkVertex.h>
#include <vtkVertexGlyphFilter.h>

#include <vtkAOSDataArrayTemplate.h>
#include <vtkGenericDataArray.h>

#include <vtkArrowSource.h>
#include <vtkGlyph3D.h>
#include <vtkTransform.h>

#include <algorithm>
#include <array>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkPolyVertex.h>
#include <vtkVectorText.h>

#include "vtkAppendPolyData.h"
#include "vtkConeSource.h"
#include "vtkFiltersSourcesModule.h" // For export macro
#include "vtkRenderingCoreModule.h"  // For export macro

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTransformFilter.h"
#include <vtkAutoInit.h>
#include <vtkAxesActor.h>
#include <vtkCaptionActor2D.h>
#include <vtkHexahedron.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLineSource.h>
#include <vtkMatrix4x4.h>
#include <vtkNamedColors.h>
#include <vtkPointPicker.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>

using namespace std;
#include <math.h>

// 多个点单元，显示箭头
// namespace
// {
//***尝试自己写类
class vtkThreeCone : public vtkPolyDataAlgorithm {
public:
    static vtkThreeCone* New();

    // ----------通过宏控制属性
    vtkTypeMacro(vtkThreeCone, vtkPolyDataAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent) override;
    // vtk通用的外部设置参数的方法
    // get/set 锥体高度，并规定上下限
    vtkSetClampMacro(Height, double, 0.0, 1.0);
    vtkGetMacro(Height, double);
    // get/set 锥体半径，并规定上下限
    vtkSetClampMacro(Radius, double, 0.0, 10.0);
    vtkGetMacro(Radius, double);
    // get/set 锥体一圈的面的个数，并规定上下限
    vtkSetClampMacro(Resolution, int, 1, 128);
    vtkGetMacro(Resolution, int);

    // 一次控制3个参数：x y z方向的约束分别是否显示。构造函数中默认都是true
    vtkSetVector3Macro(Visibility, bool);
    vtkGetVectorMacro(Visibility, bool, 3);

    // ----------自定义方法
    // 单独控制X方向的锥体是否显示
    void SetVisibilityX(bool show);
    // 单独控制Y方向的锥体是否显示
    void SetVisibilityY(bool show);
    // 单独控制Z方向的锥体是否显示
    void SetVisibilityZ(bool show);

protected:
    vtkThreeCone();
    ~vtkThreeCone() override = default;

    //   int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
    int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

    int Resolution;
    double Height;
    double Radius;
    // 或者用vtkTypeBool来表示
    bool Visibility[3];

private:
    vtkThreeCone(const vtkThreeCone&) = delete;
    void operator=(const vtkThreeCone&) = delete;
};

vtkStandardNewMacro(vtkThreeCone);

//------------------------------------------------------------------------------
vtkThreeCone::vtkThreeCone() {
    this->Resolution = 10; // 锥体有10个面
    this->Radius = 0.08;   // 锥体底部圆的半径
    this->Height = 0.8;    // 锥体的高度
    this->Visibility[0] = true;
    this->Visibility[1] = true;
    this->Visibility[2] = true;
    this->SetNumberOfInputPorts(0);
}

//------------------------------------------------------------------------------
int vtkThreeCone::RequestData(vtkInformation* vtkNotUsed(request),
                              vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* outputVector) {
    // get the info object
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    // get the output
    vtkPolyData* output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    // 三个锥体，分别代表x y z方向
    vtkConeSource* cone[3];
    // 形状相同，直接循环初始化
    for (size_t i = 0; i < 3; i++) {
        cone[i] = vtkConeSource::New();
        cone[i]->SetResolution(this->Resolution);
        cone[i]->SetHeight(this->Height);
        cone[i]->SetRadius(this->Radius);
    }
    // 特殊的一点：X轴的锥体，反向
    cone[0]->SetDirection(-1, 0, 0);

    // 三种变换，对应三个锥体
    vtkTransform* trans[3];
    for (size_t i = 0; i < 3; i++) {
        trans[i] = vtkTransform::New();
    }
    // 对X轴的锥体的变换
    trans[0]->Translate(this->Height / 2, 0, 0.0);
    // 对Y轴的锥体的变换
    trans[1]->RotateZ(-90);
    trans[1]->Translate(-this->Height / 2, 0, 0.0);
    // 对Z轴的锥体的变换
    trans[2]->RotateY(90);
    trans[2]->Translate(-this->Height / 2, 0, 0);
    // 三个变换过滤器，对应三种变换，三个锥体
    vtkTransformFilter* tf[3];
    for (size_t i = 0; i < 3; i++) {
        tf[i] = vtkTransformFilter::New();
        tf[i]->SetTransform(trans[i]);
        tf[i]->SetInputConnection(cone[i]->GetOutputPort());
    }
    vtkAppendPolyData* append = vtkAppendPolyData::New();

    // 判断需不需要显示
    for (size_t i = 0; i < 3; i++) {
        if (Visibility[i]) {
            append->AddInputConnection(tf[i]->GetOutputPort());
        }
    }

    append->Update();
    output->ShallowCopy(append->GetOutput());

    for (size_t i = 0; i < 3; i++) {
        cone[i]->Delete();
        tf[i]->Delete();
        trans[i]->Delete();
    }
    append->Delete();

    return 1;
}

//------------------------------------------------------------------------------
void vtkThreeCone::PrintSelf(ostream& os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);

    os << indent << "Resolution: " << this->Resolution << "\n";
    os << indent << "Radius: " << this->Radius << "\n";
    os << indent << "Height: " << this->Height << "\n";
    os << indent << "ShowX: " << this->Visibility[0] << "  ShowY: " << this->Visibility[1] << "  ShowZ: " << this->Visibility[2] << "\n";
}
// 单独控制X方向的锥体是否显示
void vtkThreeCone::SetVisibilityX(bool show) {
    if (this->Visibility[0] != show) {
        this->Visibility[0] = show;
        this->Modified();
    }
}
// 单独控制Y方向的锥体是否显示
void vtkThreeCone::SetVisibilityY(bool show) {
    if (this->Visibility[1] != show) {
        this->Visibility[1] = show;
        this->Modified();
    }
}
// 单独控制Z方向的锥体是否显示
void vtkThreeCone::SetVisibilityZ(bool show) {
    if (this->Visibility[2] != show) {
        this->Visibility[2] = show;
        this->Modified();
    }
}
//***尝试结束

// Define interaction style
class InteractorStyle : public vtkInteractorStyleRubberBandPick {
public:
    static InteractorStyle* New();
    vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);

    InteractorStyle() {
        this->SelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
        this->SelectedActor = vtkSmartPointer<vtkActor>::New();
        this->SelectedActor->SetMapper(SelectedMapper);
        this->ArrowActor = vtkSmartPointer<vtkActor>::New();
        this->ArrowActor2 = vtkSmartPointer<vtkActor>::New();
    }

    /**
     * @brief 给定节点集合，x y z方向的箭头是否显示，颜色，在这些结点集合处绘制箭头
     *  参考：自定义方向+设置箭头长度： https://blog.csdn.net/qq_37366618/article/details/124037374
      控制箭头参数： https://blog.csdn.net/hit1524468/article/details/104885678
     * @param points 点集合
     * @param visibility x y z三个方向各自是否显示
     * @param rgb 箭头颜色
     */
    void DisplayConstraintAtEachPoint(vtkSmartPointer<vtkPoints> points, bool visibility[3], double rgb[3]) {
        // 结点数==方向数组normals元素个数
        vtkPolyData* polydata = vtkPolyData::New(); //创建几何实体
        polydata->SetPoints(points);                //赋予几何

        // 自己的过滤器,创建 并测试自定义的属性与方法
        vtkSmartPointer<vtkThreeCone> threeCone = vtkSmartPointer<vtkThreeCone>::New();
        threeCone->SetVisibility(visibility); //构造函数中全是true
        threeCone->SetVisibilityY(false);
        threeCone->Update();

        // 在每个点上显示自己构建的三个锥体的类
        vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
        glyph->SetInputData(polydata);
        glyph->SetSourceData(threeCone->GetOutput());
        glyph->SetScaleFactor(0.1);
        // glyph->SetVectorModeToUseNormal(); // 加了会错乱
        glyph->Update();

        vtkSmartPointer<vtkPolyDataMapper> polyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        polyDataMapper->SetInputData(glyph->GetOutput());
        polyDataMapper->Update();

        this->ArrowActor->SetMapper(polyDataMapper);
        this->ArrowActor->GetProperty()->SetColor(rgb);
        this->ArrowActor->GetProperty()->SetRepresentationToWireframe();
        // this->ArrawActor->Update();
        this->CurrentRenderer->AddActor(ArrowActor);
    }

    virtual void OnLeftButtonUp() override {
        vtkNew<vtkNamedColors> colors;

        // Forward events
        vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

        vtkPlanes* frustum = static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker())->GetFrustum();

        // 获取框选出来的点集与单元集
        vtkSmartPointer<vtkExtractGeometry> extractGeometry = vtkSmartPointer<vtkExtractGeometry>::New();
        extractGeometry->SetImplicitFunction(frustum);
        extractGeometry->SetInputData(this->Points);
        extractGeometry->Update();

        // 获取点集合
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        points->DeepCopy(extractGeometry->GetOutput()->GetPoints());

        // 组装vertex对象
        vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
        polyData->SetPoints(points);
        vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
        vertexFilter->SetInputData(polyData);
        vertexFilter->Update();

        this->SelectedMapper->SetInputData(vertexFilter->GetOutput());
        SelectedMapper->Update();

        // std::cout << "----------------cell count: " << vertexFilter->GetOutput()->GetNumberOfCells() << std::endl;

        this->SelectedActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
        this->SelectedActor->GetProperty()->SetPointSize(10);
        this->CurrentRenderer->AddActor(SelectedActor);
        // 增加约束(开始)--lius
        vtkSmartPointer<vtkPoints> points2 = vtkSmartPointer<vtkPoints>::New();
        points2->DeepCopy(extractGeometry->GetOutput()->GetPoints());
        bool visibility[3] = {true, true, true};
        // double length = 1;
        double rgb[3] = {0, 1, 0};
        // points结点集合；visibility箭头方向；rgb箭头颜色
        DisplayConstraintAtEachPoint(points2, visibility, rgb);

        // 增加约束(结束)--lius
        this->GetInteractor()->GetRenderWindow()->Render();
        this->HighlightProp(NULL);
    }

    void SetPoints(vtkSmartPointer<vtkPolyData> points) {
        this->Points = points;
    }

private:
    vtkSmartPointer<vtkPolyData> Points;
    vtkSmartPointer<vtkActor> SelectedActor;
    vtkSmartPointer<vtkActor> ArrowActor;
    vtkSmartPointer<vtkActor> ArrowActor2;
    vtkSmartPointer<vtkDataSetMapper> SelectedMapper;
    // vtkSmartPointer<vtkDataSetMapper> arrowMapper;
};

vtkStandardNewMacro(InteractorStyle);
// } // namespace

int main(int, char*[]) {
    vtkNew<vtkNamedColors> colors;
    // 定义数据源:16-point 6-cell
    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetResolution(4);
    cylinder->Update();
    vtkPolyData* polyData = cylinder->GetOutput();

    // 数据集添加属性：用于label拾取的属性
    auto count = polyData->GetNumberOfPoints(); // 16
    long* arr = new long[count];
    for (int i = 0; i < count; i++) {
        arr[i] = i;
    }
    vtkNew<vtkLongArray> pointIndex;
    pointIndex->SetArray(arr, count, 1);
    pointIndex->SetName("zzy");
    polyData->GetPointData()->AddArray(pointIndex);

    // 创建mapper：显示带label属性的actor，框选时才能知道选中了哪些label
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polyData);
    // mapper->ScalarVisibilityOn();

    // 创建actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());
    actor->GetProperty()->SetOpacity(0.5);

    // 坐标轴actor
    vtkNew<vtkAxesActor> axesActor;

    // Visualize
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("HighlightSelectedPoints");
    renderWindow->SetSize(900, 600);
    renderer->AddActor(actor);
    renderer->AddActor(axesActor);
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

    // 拾取功能开始：
    vtkNew<vtkAreaPicker> areaPicker;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetPicker(areaPicker);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    {
        // 加label
        vtkNew<vtkIdFilter> idFilter;
        idFilter->SetInputData(polyData);
        idFilter->Update();
        vtkSmartPointer<vtkLabeledDataMapper> nodeLabelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
        nodeLabelMapper->SetInputConnection(idFilter->GetOutputPort());
        vtkSmartPointer<vtkActor2D> actor2D = vtkSmartPointer<vtkActor2D>::New();
        nodeLabelMapper->GetLabelTextProperty()->SetColor(0.0, 1.0, 0.0);
        actor2D->SetMapper(nodeLabelMapper);
        renderer->AddActor(actor2D);
    }

    vtkNew<InteractorStyle> style;
    style->StartSelect(); // 开始选择按键的设置
    style->SetPoints(polyData);
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}