/**
 * @brief 显示一个标注信息
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2022-09-22
 */

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkLookupTable.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>
#include <vtkTextActor.h>
#include <vtkTextWidget.h>
#include <vtkTextRepresentation.h>
#include <vtkTextProperty.h>
#include <vtkCaptionRepresentation.h>
#include <vtkCaptionWidget.h>
#include <vtkCaptionActor2D.h>
#include <vtkBalloonRepresentation.h> 
#include <vtkBalloonWidget.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkAxesActor.h>
#include <vtkNamedColors.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkWidgetCallbackMapper.h>

int main()
{
    vtkNew<vtkNamedColors> colors;
    // 定义数据源:16-point 6-cell
    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetResolution(4);
    cylinder->Update();
    vtkPolyData* polyData = cylinder->GetOutput();

    // 创建mapper：显示带label属性的actor，框选时才能知道选中了哪些label
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polyData);
    mapper->SetScalarModeToUseCellData();

    //  创建actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

    // Visualize
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("text_widget_show_test");
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
	renderWindow->Render();
	vtkSmartPointer<vtkRenderWindowInteractor > interactor = vtkSmartPointer< vtkRenderWindowInteractor >::New();
	interactor->SetRenderWindow(renderWindow);

    //////////////////////////////////////////////////////////////////////////
    // vtkTextWidget
    vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
    textActor->SetInput("zzy!!!!!!!!!!!!!!!!!");  // string.c_str()
    textActor->GetTextProperty()->SetColor(0.0, 1.0, 0.0);

    vtkSmartPointer<vtkTextRepresentation> textRepresentation = vtkSmartPointer<vtkTextRepresentation>::New();
    vtkSmartPointer<vtkTextWidget> textWidget = vtkSmartPointer<vtkTextWidget>::New();

    textRepresentation->GetPositionCoordinate()->SetValue(0.3, 0.0); // 起始点的x、y相对屏幕位置
    textRepresentation->GetPosition2Coordinate()->SetValue(0.4, 0.1); // 终止点的x、y相对屏幕位置
    textWidget ->SetRepresentation( textRepresentation );

    textWidget->SetInteractor(interactor);
    textWidget->SetTextActor(textActor);
    textWidget->SelectableOff();
	textWidget->On();

    ////////////////////////////////////////////////////////////////////////////
    // // vtkCaptionWidget
    // vtkSmartPointer<vtkCaptionRepresentation> captionRepresentation = 
    // 	vtkSmartPointer<vtkCaptionRepresentation>::New();
    // captionRepresentation->GetCaptionActor2D()->SetCaption("Caption Widget");
    // captionRepresentation->GetCaptionActor2D()->GetTextActor()->GetTextProperty()->SetFontSize(20);

    // double pos[3] = {.5,0,0};
    // captionRepresentation->SetAnchorPosition(pos);

    // vtkSmartPointer<vtkCaptionWidget> captionWidget = 
    // 	vtkSmartPointer<vtkCaptionWidget>::New();
    // captionWidget->SetInteractor(interactor);
    // captionWidget->SetRepresentation(captionRepresentation);
    // captionWidget->On();

    renderWindow->Render();
    interactor->Initialize();
    interactor->Start();

    return 0;
}
