#include <vtkAutoInit.h>
#include <vtkAxesActor.h>
#include <vtkCaptionActor2D.h>
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

VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkRenderingFreeType);

double GetLength(std::vector<double> data) {
    double result = sqrt(pow(data[0], 2) + pow(data[1], 2) + pow(data[2], 2));
    return result;
};

class GetCoordInteractor : public vtkInteractorStyleTrackballCamera {
public:
    static GetCoordInteractor* New();
    vtkTypeMacro(GetCoordInteractor, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonDown() override {
        if (this->Interactor->GetControlKey()) {
            this->StartRotate();
        } else {
            this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
                                                this->Interactor->GetEventPosition()[1],
                                                0, // always zero.
                                                this->Interactor->GetRenderWindow()
                                                    ->GetRenderers()
                                                    ->GetFirstRenderer());

            this->Interactor->GetPicker()->GetPickPosition(picked);
            point->SetRadius(0.05);
            point->Update();
            point_mapper->SetInputConnection(point->GetOutputPort());
            point_actor->SetMapper(point_mapper);
            point_actor->SetPosition(picked);
            auto renderer = this->Interactor->GetRenderWindow()
                                ->GetRenderers()
                                ->GetFirstRenderer();
            renderer->AddActor(point_actor);
            this->axes1->SetUserTransform(this->transform);
            renderer->AddActor(this->axes1);
            if (this->count == 0) {
                this->axes1->SetXAxisLabelText("X'");
                this->axes1->SetYAxisLabelText("Y'");
                this->axes1->SetZAxisLabelText("Z'");
                //对初始坐标系进行平移，使其原点平移到拾取的点的位置
                this->transform->Translate(picked[0], picked[1], picked[2]);
                count += 1;
                this->new_origin.push_back(picked[0]);
                this->new_origin.push_back(picked[1]);
                this->new_origin.push_back(picked[2]);
            } else if (this->count == 1) {
                //点击第二个点确定x轴
                //确定新的x轴的方向向量，即新的原点new_origin指向拾取的点
                std::vector<double> OX_new = {picked[0] - this->new_origin[0], picked[1] - new_origin[1], picked[2] - new_origin[2]};

                //计算OX转向OX_new的角度及转轴运用到transform里面
                //normal为原x轴和新x轴所成平面的法向量，也是这一步所做旋转的转轴
                std::vector<double> normal = {OX[1] * OX_new[2] - OX[2] * OX_new[1],
                                              OX[2] * OX_new[0] - OX[0] * OX_new[2],
                                              OX[0] * OX_new[1] - OX[1] * OX_new[0]};
                //向量XX'
                std::vector<double> XX_1 = {OX_new[0] - OX[0],
                                            OX_new[1] - OX[1],
                                            OX_new[2] - OX[2]};

                double a = GetLength(OX_new), c = GetLength(XX_1);
                //在三角形OXX'中运用余弦公式计算旋转的角度
                double cos = (pow(a, 2) + 1 - pow(c, 2)) / (2 * a);
                //这里计算出来是弧度制
                double radian = std::acos(cos);
                //转换成角度制
                double angle = radian / acos(-1) * 180;
                //传入的数据：旋转的角度，转轴的方向向量
                this->transform->RotateWXYZ(angle, normal[0], normal[1], normal[2]);

                this->count += 1;
            } else if (this->count == 2) {
                //选中的点在上一个坐标系的坐标
                //将选中的点转换成当前坐标系下的坐标再计算角度
                double vec_xoy[3];
                this->transform->GetInverse()->TransformPoint(picked, vec_xoy);
                std::cout << vec_xoy[0] << " " << vec_xoy[1] << " " << vec_xoy[2] << std::endl;

                //normal_1为已确定的x轴与向量vec_xoy所称平面的法向量，即z轴方向
                std::vector<double> normal_1 = {OX[1] * vec_xoy[2] - OX[2] * vec_xoy[1],
                                                OX[2] * vec_xoy[0] - OX[0] * vec_xoy[2],
                                                OX[0] * vec_xoy[1] - OX[1] * vec_xoy[0]};
                std::vector<double> ZZ_1 = {OZ[0] - normal_1[0],
                                            OZ[1] - normal_1[1],
                                            OZ[2] - normal_1[2]};
                //下面计算角度过程与count==1中相同
                double a = 1, b = GetLength(normal_1), c = GetLength(ZZ_1);
                double cos = (pow(b, 2) + pow(a, 2) - pow(c, 2)) / (2 * a * b);
                double radian = std::acos(cos);
                double angle = radian / acos(-1) * 180;
                //选中的点在当前坐标系z>0时逆时针旋转，z<0时顺时针旋转
                if (vec_xoy[2] >= 0) {
                    this->transform->RotateX(angle);
                } else {
                    this->transform->RotateX(-angle);
                }

                count += 1;

                double out2[3];
                this->transform->GetInverse()->TransformPoint(picked, out2);
                std::cout << out2[0] << " " << out2[1] << " " << out2[2] << std::endl;

            } else if (count == 3) {
                std::cout << "Pick no more than 3 points!" << std::endl;
            }
            // Forward events
            vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
        }
    };

    double picked[3];
    int count = 0;
    std::vector<double> new_origin;
    double OX[3] = {1, 0, 0};
    double OY[3] = {0, 1, 0};
    double OZ[3] = {0, 0, 1};
    vtkSmartPointer<vtkAxesActor> axes1 = vtkSmartPointer<vtkAxesActor>::New();
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    vtkSmartPointer<vtkSphereSource> point = vtkSmartPointer<vtkSphereSource>::New();
    vtkSmartPointer<vtkActor> point_actor = vtkSmartPointer<vtkActor>::New();
    vtkSmartPointer<vtkPolyDataMapper> point_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
};
vtkStandardNewMacro(GetCoordInteractor);

int main() {
    vtkNew<vtkNamedColors> colors;

    vtkSmartPointer<vtkAxesActor> axes0 = vtkSmartPointer<vtkAxesActor>::New();

    vtkNew<vtkPointPicker> worldPointPicker;

    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("CoordinateTransform");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetPicker(worldPointPicker);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<GetCoordInteractor> style;
    renderWindowInteractor->SetInteractorStyle(style);

    //局部坐标转世界坐标
    double local_coor[3] = {1, 2, 3};
    double world_coor[3];
    style->transform->TransformPoint(local_coor, world_coor);

    // Add the actor to the scene
    renderer->AddActor(axes0);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    // Render and interact
    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}
