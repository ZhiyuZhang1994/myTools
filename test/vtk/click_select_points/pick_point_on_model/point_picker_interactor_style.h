/**
 * @brief 拾取模型上节点的交互类型
 * 拾取后在拾取到的节点上创建一个红色圆球用于显示
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2023-9-13
 */

#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkExtractGeometry.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleRubberBandPick.h>
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
#include <vtkUnstructuredGrid.h>
#include <vtkVersion.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkSphereSource.h>
#include <vtkLabeledDataMapper.h>
#include <vtkActor2D.h>
#include <vtkIndent.h>
#include <vtkCellCenters.h>
#include <vtkTextProperty.h>
#include <vtkCylinderSource.h>
#include <vtkCellPicker.h>

// Define interaction style
class PointPickerInteractorStyle : public vtkInteractorStyleRubberBandPick
{
public:
  static PointPickerInteractorStyle* New();
  vtkTypeMacro(PointPickerInteractorStyle, vtkInteractorStyleRubberBandPick);

  PointPickerInteractorStyle();

  virtual void OnLeftButtonDown() override;


  void set_points(vtkSmartPointer<vtkPolyData> points);

private:
  vtkSmartPointer<vtkPolyData> points;
  vtkSmartPointer<vtkActor> SelectedActor;
  vtkSmartPointer<vtkDataSetMapper> SelectedMapper;
};
