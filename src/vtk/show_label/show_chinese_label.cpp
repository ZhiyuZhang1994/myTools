/**
 * @brief 显示多行标注信息：包括中文
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
#include <vtkHandleWidget.h>
#include <vtkProperty2D.h>
#include <algorithm>
#include <ctime>

int main()
{
    vtkNew<vtkNamedColors> colors;
    // 定义数据源:16-point 6-cell
    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetResolution(4);
    cylinder->Update();
    vtkPolyData *polyData = cylinder->GetOutput();

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
    vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(renderWindow);


    ////////////////////////////////////////////////////////////////////////////
    // // vtkTextActor
    std::stringstream ss; \
    ss << "     BeFEM Studio 2023R1\n";

    time_t now = time(nullptr);
    tm* curr_tm = localtime(&now);
    char time[80] = {0};
    strftime(time, 80, "       %Y-%m-%d  %H:%M:%S\n", curr_tm);
    ss << time;

    ss << "————————————\n";
    ss << "  节点结果\n";
    ss << "载荷步  载荷步1";

    std::string title = ss.str();
    std::cout << title << std::endl;
    auto timeActor_ = vtkSmartPointer<vtkTextActor>::New();
    timeActor_->SetInput(title.c_str());
    timeActor_->GetTextProperty()->SetJustificationToLeft();
    double timeColor[3] = {1.0, 1.0, 1.0};
    timeActor_->GetTextProperty()->SetFontFamily(VTK_FONT_FILE);
    timeActor_->GetTextProperty()->SetFontFile("C:\\Windows\\Fonts\\simkai.ttf");
    timeActor_->GetTextProperty()->SetColor(timeColor);
    timeActor_->GetTextProperty()->SetFontSize(15);
    timeActor_->GetTextProperty()->SetVerticalJustificationToCentered();
    timeActor_->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
    timeActor_->GetPositionCoordinate()->SetValue(0.1, 0.8);
    renderer->AddActor(timeActor_);


    interactor->Start();

    return 0;
}