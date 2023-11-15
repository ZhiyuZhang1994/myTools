/**
 * @brief 旋转scene的交互类型
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2023-11-15
 */

#ifndef BASIC_INTERACTOR_STYLE_H
#define BASIC_INTERACTOR_STYLE_H

#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkUnstructuredGrid.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

class BasicInteractorStyle : public vtkInteractorStyleRubberBandPick {
public:
    static BasicInteractorStyle *New();

vtkTypeMacro(BasicInteractorStyle, vtkInteractorStyleRubberBandPick);

    BasicInteractorStyle();

    ///操控模式：旋转或平移
    enum ControlMode {
        ROTATE,
        PAN,
    };

    void OnLeftButtonDown() override;

    void OnLeftButtonUp() override;

    void OnRightButtonDown() override;

    void OnRightButtonUp() override;

    void OnMiddleButtonDown() override;

    void OnMiddleButtonUp() override;

    void OnMouseMove() override;

    void OnMouseWheelForward() override;

    void OnMouseWheelBackward() override;

    void OnChar() override;

    void Rotate() override;

    virtual void OnKeyPress() override;

    virtual void controlPress();

    virtual void OnKeyRelease() override;

    virtual void controlRelease();

    virtual void setDataSource(vtkSmartPointer<vtkDataSet> dataSource);

    /**
     * @brief Set the Default Rotation Center
     */
    void setDefaultRotationCenter();

    /**
     * @brief Set the User Defined Rotate Center
     */
    void setUserDefinedRotateCenter();

    /**
     * @brief 判断空间上的点和点击点是否在屏幕显示上相邻
     * @param pointPos 空间中的一点(世界三维坐标)
     * @param pos 屏幕上一点(像素坐标)
     * @return true 相近
     * @return false 相远
     */
    virtual bool isAdjacent(double* pointPos, const int* pos);

protected:
    virtual void buttonDown(ControlMode);

protected:
    vtkSmartPointer<vtkDataSet> dataSource_ = nullptr;
    double clickTolerate_ = 100;
    int controlPressed_ = 0;

private:
    vtkSmartPointer<vtkSphereSource> point_ = nullptr;
    vtkSmartPointer<vtkPolyDataMapper> mapper_ = nullptr;
    vtkSmartPointer<vtkActor> actor_ = nullptr;
    double pickedPos_[3];
    bool picked = false;
    int leftButtonPressedPos_[2];
};

#endif // BASIC_INTERACTOR_STYLE_H

