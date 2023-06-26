/**
 * @brief 最新版本切块实现方案，采用vtkImplicitPlaneWidget2实现
 * 
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
        this->CallbackMapper->SetCallbackMethod(
            vtkCommand::MouseMoveEvent, vtkWidgetEvent::Move, this, myVtkImplicitPlaneWidget::MoveaaAction);
    }
    static void MoveaaAction(vtkAbstractWidget* w) {
        // std::cout << "zzy" << std::endl;
        vtkImplicitPlaneWidget2::MoveAction(w);
    }

private:
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
        vtkImplicitPlaneRepresentation* rep = reinterpret_cast<vtkImplicitPlaneRepresentation*>(planeWidget_->GetRepresentation());
        rep->SetDrawPlane(1);
    }
    virtual void OnRightButtonUp() override {
        // 原始actor的隐藏/显示
        vtkInteractorStyleRubberBandPick::OnRightButtonUp();
        vtkImplicitPlaneRepresentation* rep = reinterpret_cast<vtkImplicitPlaneRepresentation*>(planeWidget_->GetRepresentation());
        rep->SetDrawPlane(0);
    }
    void SetplaneWidget(vtkSmartPointer<myVtkImplicitPlaneWidget> planeWidget) {
        this->planeWidget_ = planeWidget;
    }

private:
    vtkSmartPointer<myVtkImplicitPlaneWidget> planeWidget_ = vtkSmartPointer<myVtkImplicitPlaneWidget>::New();
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
        vtkImplicitPlaneRepresentation* rep =
            reinterpret_cast<vtkImplicitPlaneRepresentation*>(
                planeWidget->GetRepresentation());
        rep->GetPlane(this->plane);
    }

    vtkIPWCallback() = default;

    vtkPlane* plane{nullptr};
};

int main(int argc, char* argv[]) {
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetRadius(10.0);
    sphereSource->Update();
    vtkPolyData* polyData = sphereSource->GetOutput();

    {
        // 点数据集添加属性：用于label拾取的属性
        auto count = polyData->GetNumberOfPoints();  // 16
        long *arr=new long[count];
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
    vtkNew<vtkClipPolyData> clipper;
    clipper->SetClipFunction(plane);
    clipper->InsideOutOn();
    clipper->SetInputConnection(sphereSource->GetOutputPort());


    // Create a mapper and actor.
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(clipper->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkProperty> backFaces;
    backFaces->SetDiffuseColor(colors->GetColor3d("Gold").GetData());

    actor->SetBackfaceProperty(backFaces);
    actor->GetProperty()->SetEdgeVisibility(true);
    actor->GetProperty()->SetEdgeColor(0, 0, 0);
    // A renderer and render window.
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("ImplicitPlaneWidget2");

    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    {
        // 显示节点标量值
        vtkSmartPointer<vtkLabeledDataMapper> nodeLabelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
        nodeLabelMapper->SetInputData(polyData);
        nodeLabelMapper->SetFieldDataName("PointsIDSet");
        nodeLabelMapper->SetLabelModeToLabelFieldData();

        nodeLabelMapper->SetLabelFormat("%d");  // 设置格式为整数
        vtkSmartPointer<vtkActor2D> actor2D = vtkSmartPointer<vtkActor2D>::New();
        nodeLabelMapper->GetLabelTextProperty()->SetColor(0.0, 1.0, 0.0);
        actor2D->SetMapper(nodeLabelMapper);
        renderer->AddActor(actor2D);
    }

    // An interactor.
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // The callback will do the work.
    vtkNew<vtkIPWCallback> myCallback;
    myCallback->plane = plane;

    vtkNew<vtkImplicitPlaneRepresentation> rep;
    rep->SetPlaceFactor(1.25); // This must be set prior to placing the widget.
    rep->PlaceWidget(polyData->GetBounds());
    rep->SetNormal(plane->GetNormal());
    rep->SetOutlineTranslation(false); // 设置整个隐函数模型是否可以左键选中平移
    rep->SetScaleEnabled(false);       // 设置是否可以右键缩放整个隐函数模型
    rep->SetDrawPlane(true);           // 是否绘制中间的切面
    rep->SetOutlineTranslation(false);
    rep->SetScaleEnabled(false);
    rep->SetVisibility(true);          // 设置widget是否可见，操作不受影响
    vtkNew<myVtkImplicitPlaneWidget> planeWidget;
    planeWidget->SetInteractor(renderWindowInteractor);
    planeWidget->SetRepresentation(rep);

    planeWidget->AddObserver(vtkCommand::InteractionEvent, myCallback);

    renderer->GetActiveCamera()->Azimuth(-60);
    renderer->GetActiveCamera()->Elevation(30);
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Zoom(0.75);

    // Render and interact.
    vtkNew<InteractorStyle> style;
    style->SetplaneWidget(planeWidget);
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->Initialize();
    renderWindow->Render();
    planeWidget->On();

    // Begin mouse interaction.
    renderWindowInteractor->Start();
    // 自定义交互方式+传参

    return EXIT_SUCCESS;
}