/**
 * @brief 最新版本切块实现方案，采用vtkImplicitPlaneWidget2实现
 * 用vtkClipDataSet实现数据的切块
 * 用id的mapper中添加切平面实现节点编号的切割
 * 用特征边对象的mapper中添加切平面实现特征边(轮廓)的切割
 * @author zhangzhiyu
 * @date 2023-06-19
 */

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <math.h>
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
#include <vtkClipPolyData.h>
#include <vtkCommand.h>
#include <vtkCutter.h>
#include <vtkCylinderSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGlyph3D.h>
#include <vtkHexahedron.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkImplicitPlaneRepresentation.h>
#include <vtkImplicitPlaneWidget2.h>
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
#include <vtkSelectVisiblePoints.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTextProperty.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVersion.h>
#include <vtkVertex.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkWidgetCallbackMapper.h>
#include <vtkWidgetEvent.h>
#include <vtkXMLPolyDataReader.h>

namespace
{

/**
 * @brief 重写widget类，用于在旋转交互时发出信号
 */
class myVtkImplicitPlaneWidget : public vtkImplicitPlaneWidget2 {
public:
    static myVtkImplicitPlaneWidget* New();
    vtkTypeMacro(myVtkImplicitPlaneWidget, vtkImplicitPlaneWidget2);
    // 构造函数
    myVtkImplicitPlaneWidget() {
        // 会覆盖掉基类中对vtkCommand::MouseMoveEvent设置的
        this->CallbackMapper->SetCallbackMethod(
            vtkCommand::MouseMoveEvent, vtkWidgetEvent::Move, this, myVtkImplicitPlaneWidget::MoveAction);
    }
    static void MoveAction(vtkAbstractWidget* w) {
        // std::cout << "zzy" << std::endl;
        vtkImplicitPlaneWidget2::MoveAction(w);
    }
};
vtkStandardNewMacro(myVtkImplicitPlaneWidget);

// Define interaction style
class InteractorStyle : public vtkInteractorStyleRubberBandPick {
public:
    static InteractorStyle* New();
    vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);
    // 构造函数
    InteractorStyle() {
    }

    virtual void OnLeftButtonUp() override {
        // Forward events
        vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
    }
    virtual void OnRightButtonUp() override {
        // Forward events
        vtkInteractorStyleRubberBandPick::OnRightButtonUp();
    }

    void SetplaneWidget(vtkSmartPointer<myVtkImplicitPlaneWidget> planeWidget) {
        this->planeWidget_ = planeWidget;
    }

    void SetVtkPlane(vtkSmartPointer<vtkPlane> plane) {
        this->plane_ = plane;
    }

    void SetCliper(vtkSmartPointer<vtkClipPolyData> clipper) {
        this->clipper_ = clipper;
    }

    void SetLabelPane(vtkSmartPointer<vtkPlane> plane) {
        this->labelPlane_ = plane;
    }

    void Set2DActorMapper(vtkSmartPointer<vtkLabeledDataMapper> nodeLabelMapper) {
        this->nodeLabelMapper_ = nodeLabelMapper;
    }

    /**
     * @brief 用于模拟切割完显示左边还是右边的按钮
     */
    void OnChar() override {
        vtkRenderWindowInteractor* rwi = this->Interactor;

        switch (rwi->GetKeyCode()) {
            case 'w':
            case 'W': // 左侧
                clipper_->SetInsideOut(false);
                nodeLabelMapper_->RemoveAllClippingPlanes();
                nodeLabelMapper_->AddClippingPlane(plane_);
                break;
            case 's':
            case 'S': // 右侧
                clipper_->SetInsideOut(true);
                nodeLabelMapper_->RemoveAllClippingPlanes();
                nodeLabelMapper_->AddClippingPlane(labelPlane_);
                break;

            default:
                this->Superclass::OnChar();
                break;
        }
        this->GetInteractor()->GetRenderWindow()->Render();
        this->HighlightProp(NULL);
    }

private:
    vtkSmartPointer<myVtkImplicitPlaneWidget> planeWidget_ = vtkSmartPointer<myVtkImplicitPlaneWidget>::New();
    vtkSmartPointer<vtkClipPolyData> clipper_ = vtkSmartPointer<vtkClipPolyData>::New();
    vtkSmartPointer<vtkPlane> plane_ = vtkSmartPointer<vtkPlane>::New();
    vtkSmartPointer<vtkPlane> labelPlane_ = vtkSmartPointer<vtkPlane>::New();
    vtkSmartPointer<vtkLabeledDataMapper> nodeLabelMapper_ = vtkSmartPointer<vtkLabeledDataMapper>::New();
};
vtkStandardNewMacro(InteractorStyle);
} // namespace

// Callback for the interaction
// This does the actual work: updates the vtkPlane implicit function.
// This in turn causes the pipeline to update and clip the object.
class vtkIPWCallback : public vtkCommand {
public:
    static vtkIPWCallback* New() {
        return new vtkIPWCallback;
    }

    virtual void Execute(vtkObject* caller, unsigned long, void*) {
        myVtkImplicitPlaneWidget* planeWidget =
            reinterpret_cast<myVtkImplicitPlaneWidget*>(caller);
        vtkImplicitPlaneRepresentation* rep = reinterpret_cast<vtkImplicitPlaneRepresentation*>(planeWidget->GetRepresentation());
        rep->GetPlane(this->plane_);

        if (1) { // 左侧法向量相反，右侧不动
            rep->GetPlane(this->labelPlane_);
            std::array<double, 3> normalXyz;
            rep->GetNormal(normalXyz.data());
            std::transform(normalXyz.begin(), normalXyz.end(), normalXyz.begin(), [](double each) { return -each; });
            labelPlane_->SetNormal(normalXyz.data());
        }
    }

    vtkIPWCallback() = default;

    void SetVtkPlane(vtkSmartPointer<vtkPlane> plane) {
        this->plane_ = plane;
    }

    void SetLabelPane(vtkSmartPointer<vtkPlane> plane) {
        this->labelPlane_ = plane;
    }

    void Set2DActorMapper(vtkSmartPointer<vtkLabeledDataMapper> nodeLabelMapper) {
        this->nodeLabelMapper_ = nodeLabelMapper;
    }

private:
    vtkSmartPointer<vtkPlane> plane_ = vtkSmartPointer<vtkPlane>::New();
    vtkSmartPointer<vtkPlane> labelPlane_ = vtkSmartPointer<vtkPlane>::New();
    vtkSmartPointer<vtkLabeledDataMapper> nodeLabelMapper_ = vtkSmartPointer<vtkLabeledDataMapper>::New();
};

int main(int argc, char* argv[]) {
    // 基础设置
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("ImplicitPlaneWidget2");
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetRadius(10.0);
    sphereSource->Update();
    vtkPolyData* polyData = sphereSource->GetOutput();
    {
        // 点数据集添加属性：用于label拾取的属性
        auto count = polyData->GetNumberOfPoints(); // 16
        long* arr = new long[count];
        for (int i = 0; i < count; i++) {
            arr[i] = i;
        }
        vtkNew<vtkLongArray> pointIndex;
        pointIndex->SetArray(arr, count, 1);
        pointIndex->SetName("PointsIDSet");
        polyData->GetPointData()->AddArray(pointIndex);
    }

    // Setup a visualization pipeline.
    vtkNew<vtkPlane> plane;
    vtkSmartPointer<vtkClipPolyData> clipper = vtkSmartPointer<vtkClipPolyData>::New();
    clipper->SetClipFunction(plane);
    clipper->SetInsideOut(false); // 左侧
    clipper->SetInputConnection(sphereSource->GetOutputPort());
    // Create a mapper and actor.
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(clipper->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetEdgeVisibility(true);
    actor->GetProperty()->SetEdgeColor(0, 0, 0);
    renderer->AddActor(actor);

    // 显示节点标量值
    vtkSmartPointer<vtkSelectVisiblePoints> visPts = vtkSmartPointer<vtkSelectVisiblePoints>::New();
    visPts->SetInputConnection(sphereSource->GetOutputPort());
    visPts->SetRenderer(renderer);
    vtkSmartPointer<vtkLabeledDataMapper> nodeLabelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
    nodeLabelMapper->SetInputConnection(visPts->GetOutputPort());
    vtkSmartPointer<vtkPlane> labelPlane = vtkSmartPointer<vtkPlane>::New();
    nodeLabelMapper->AddClippingPlane(labelPlane);
    // nodeLabelMapper->SetInputData(polyData);
    nodeLabelMapper->SetFieldDataName("PointsIDSet");
    nodeLabelMapper->SetLabelModeToLabelFieldData();
    nodeLabelMapper->SetLabelFormat("%d"); // 设置格式为整数
    vtkSmartPointer<vtkActor2D> actor2D = vtkSmartPointer<vtkActor2D>::New();
    nodeLabelMapper->GetLabelTextProperty()->SetColor(0.0, 1.0, 0.0);
    actor2D->SetMapper(nodeLabelMapper);
    renderer->AddActor(actor2D);


    // The callback will do the work.
    vtkNew<vtkIPWCallback> myCallback;
    myCallback->SetVtkPlane(plane);       // 切平面
    myCallback->SetLabelPane(labelPlane); // 切节点的平面

    vtkNew<vtkImplicitPlaneRepresentation> rep;
    rep->SetPlaceFactor(1.25); // This must be set prior to placing the widget.
    rep->PlaceWidget(polyData->GetBounds());
    rep->SetNormal(plane->GetNormal());
    rep->SetOutlineTranslation(false); // 设置整个隐函数模型是否可以左键选中平移
    rep->SetScaleEnabled(false);       // 设置是否可以右键缩放整个隐函数模型
    rep->SetDrawPlane(true);           // 是否绘制中间的切面
    rep->SetOutlineTranslation(false);
    rep->SetScaleEnabled(false);
    rep->SetVisibility(true); // 设置widget是否可见，操作不受影响
    vtkNew<myVtkImplicitPlaneWidget> planeWidget;
    planeWidget->SetInteractor(renderWindowInteractor);
    planeWidget->SetRepresentation(rep);
    planeWidget->AddObserver(vtkCommand::InteractionEvent, myCallback);

    // Render and interact.
    vtkNew<InteractorStyle> style;
    style->SetplaneWidget(planeWidget);
    style->SetCliper(clipper);
    style->Set2DActorMapper(nodeLabelMapper);
    style->SetLabelPane(labelPlane); // 切节点编号的平面
    style->SetVtkPlane(plane);       // 切平面
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->Initialize();
    renderWindow->Render();
    planeWidget->On();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}