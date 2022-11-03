/**
 * @brief 组装actor:将三个圆锥体组装成一个用于表示位移约束
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2022-11-3
 */

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

#include "vtkFiltersSourcesModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"
#include "vtkConeSource.h"
#include "vtkAppendPolyData.h"
#include "vtkTransformFilter.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"

class vtkThreeCone : public vtkPolyDataAlgorithm
{
public:
  static vtkThreeCone* New();

  vtkTypeMacro(vtkThreeCone, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkSetClampMacro(TipLength, double, 0.0, 1.0);
  vtkGetMacro(TipLength, double);
  vtkSetClampMacro(TipRadius, double, 0.0, 10.0);
  vtkGetMacro(TipRadius, double);

  vtkSetClampMacro(TipResolution, int, 1, 128);
  vtkGetMacro(TipResolution, int);

protected:
  vtkThreeCone();
  ~vtkThreeCone() override = default;

//   int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  int TipResolution;
  double TipLength;
  double TipRadius;

private:
  vtkThreeCone(const vtkThreeCone&) = delete;
  void operator=(const vtkThreeCone&) = delete;
};

vtkStandardNewMacro(vtkThreeCone);

//------------------------------------------------------------------------------
vtkThreeCone::vtkThreeCone()
{
  this->TipResolution = 10; // 锥体有10个面
  this->TipRadius = 0.08; // 锥体底部圆的半径
  this->TipLength = 0.8; // 锥体的高度

  this->SetNumberOfInputPorts(0);
}

//------------------------------------------------------------------------------
int vtkThreeCone::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* outputVector)
{
    // get the info object
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    // get the output
    vtkPolyData* output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    //   int piece, numPieces;
    vtkConeSource* coneX = vtkConeSource::New();
    vtkConeSource* coneY = vtkConeSource::New();
    vtkConeSource* coneZ = vtkConeSource::New();
    vtkTransform* transX = vtkTransform::New();
    vtkTransform* transY = vtkTransform::New();
    vtkTransform* transZ = vtkTransform::New();
    vtkTransformFilter* tfX = vtkTransformFilter::New();
    vtkTransformFilter* tfY = vtkTransformFilter::New();
    vtkTransformFilter* tfZ = vtkTransformFilter::New();
    vtkAppendPolyData* append = vtkAppendPolyData::New();

    //   piece = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER());
    //   numPieces = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES());
    outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS());

    coneX->SetResolution(this->TipResolution);
    coneX->SetHeight(this->TipLength);
    coneX->SetRadius(this->TipRadius);
    coneX->SetDirection(-1,0,0);

    coneY->SetResolution(this->TipResolution);
    coneY->SetHeight(this->TipLength);
    coneY->SetRadius(this->TipRadius);

    coneZ->SetResolution(this->TipResolution);
    coneZ->SetHeight(this->TipLength);
    coneZ->SetRadius(this->TipRadius);

    transX->Translate(this->TipLength/2, 0, 0.0);
    tfX->SetTransform(transX);
    tfX->SetInputConnection(coneX->GetOutputPort());

    transY->RotateZ(-90);
    transY->Translate(-this->TipLength/2, 0, 0.0);
    tfY->SetTransform(transY);
    tfY->SetInputConnection(coneY->GetOutputPort());

    transZ->RotateY(90);
    transZ->Translate(-this->TipLength/2, 0, 0);
    tfZ->SetTransform(transZ);
    tfZ->SetInputConnection(coneZ->GetOutputPort());

    append->AddInputConnection(tfX->GetOutputPort());
    append->AddInputConnection(tfY->GetOutputPort());
    append->AddInputConnection(tfZ->GetOutputPort());


    // used only when this->ArrowOrigin is Center (we aim to orient and scale from the centre).
    vtkTransform* trans3 = vtkTransform::New();
    vtkTransformFilter* tf3 = vtkTransformFilter::New();
    trans3->Translate(0, 0.0, 0.0);
    tf3->SetTransform(trans3);

    tf3->SetInputConnection(append->GetOutputPort());
    tf3->Update();
    output->ShallowCopy(tf3->GetOutput());

    transX->Delete();
    tfX->Delete();
    append->Delete();
    tfZ->Delete();
    transZ->Delete();
    tf3->Delete();
    trans3->Delete();

    return 1;
}

//------------------------------------------------------------------------------
void vtkThreeCone::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "TipResolution: " << this->TipResolution << "\n";
  os << indent << "TipRadius: " << this->TipRadius << "\n";
  os << indent << "TipLength: " << this->TipLength << "\n";
}

int main() {
    vtkSmartPointer<vtkThreeCone> twoCone = vtkSmartPointer<vtkThreeCone>::New();
    twoCone->Update();
    // arrow->GetOutput()
    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(twoCone->GetOutputPort());
    // create an actor
    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetEdgeVisibility(true); // 边框着色
    // sphereActor->GetProperty()->SetOpacity(0.1);
    sphereActor->GetProperty()->SetRepresentationToWireframe(); // 只显示边

    vtkNew<vtkAxesActor> axes;


    // a renderer and render window
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetWindowName("twoCone");
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(300, 300);
    // an interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    vtkNew<vtkNamedColors> colors;
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    renderer->AddActor(sphereActor);
    renderer->AddActor(axes);
    renderer->ResetCamera();
    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}
