#include "medCropToolBox.h"

#include <itkRegionOfInterestImageFilter.h>

#include <medAbstractDataFactory.h>
#include <medAbstractLayeredView.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <medPluginManager.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxBody.h>
#include <medToolBoxFactory.h>
#include <medUtilities.h>
#include <medViewContainer.h>
#include <medVtkViewBackend.h>

#include <vtkBorderRepresentation.h>
#include <vtkBorderWidget.h>
#include <vtkCamera.h>
#include <vtkImageView2D.h>
#include <vtkImageView2DCommand.h>
#include <vtkImageView3D.h>
#include <vtkMatrix4x4.h>
#include <vtkPlane.h>
#include <vtkPolyData.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>

class medCropCallback;

class medCropToolBoxPrivate
{
public:
    medAbstractLayeredView* view;
    vtkImageView2D* view2D;
    vtkImageView3D* view3D;
    QPushButton* applyButton;
    QPushButton* saveButton;
    vtkSmartPointer<vtkBorderWidget> borderWidget;
    int currentOrientation;
    QList<dtkSmartPointer<medAbstractData> > outputData;
    vtkSmartPointer<medCropCallback> cropCallback;
    vtkMatrix4x4* orientationMatrix;
    vtkSmartPointer<vtkMatrix4x4> inverseOrientationMatrix;

    
    void updateBorderWidgetIfVisible();
    void updateCurrentOrientation();
    void updateBoxWidgetFromBorderWidget();
    void adjustBoxCornersToAnnulPlaceFactor(double* maxBoxCorner, double* minBoxCorner);
    void updateBorderWidgetFromBoxWidget();
    void setBorderFromWorldPoints(double* worldPoint1, double* worldPoint2);
    void getMinAndMaxBoxCorners(double* minCorner, double* maxCorner);
    void worldToNormalizedViewport(double* worldPoint, double* viewportPoint);
    void normalizedViewportToWorld(double* viewportPoint, double* worldPoint);
    void applyOrientationMatrix(double* worldPointIn, double* WorldPointOut);
    void applyInverseOrientationMatrix(double* worldPointIn, double* worldPointOut);
    void generateOutput();
    template <typename DATA_TYPE, unsigned int DIMENSIONS>
    medAbstractData* extractCroppedImage(medAbstractData* abstractData, const QString& destType, int* minIndices, int* maxIndices);
    void setOutputMetaData(medAbstractData* output, medAbstractData* input, int columns, int rows);
    void replaceViewWithOutputData(medAbstractWorkspace& workspace);
    void importOutput();
};

class medCropCallback : public vtkCommand
{

public:

    medCropToolBox* toolBox;

    static medCropCallback *New()
    {
        return new medCropCallback();
    }

    void Execute(vtkObject *caller, unsigned long ev, void *callData )
    {
        if (ev == vtkCommand::InteractionEvent)
        {
            toolBox->handleInteractionEvent();
        }
        else
        {
            if (ev == vtkImageView2DCommand::CameraMoveEvent)
            {
                toolBox->handleCameraMoveEvent();
            }
        }
    }

};

bool medCropToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medCropToolBox>();
}

medCropToolBox::medCropToolBox(QWidget* parent)
    : medAbstractSelectableToolBox(parent), applyButtonName("Apply"), saveButtonName("Save"), d(new medCropToolBoxPrivate())
{
    d->borderWidget = vtkSmartPointer<vtkBorderWidget>::New();
    d->borderWidget->CreateDefaultRepresentation();
    d->borderWidget->SelectableOff();
    static_cast<vtkBorderRepresentation*>(d->borderWidget->GetRepresentation())->GetBorderProperty()->SetColor(0,1,0);

    d->applyButton = new QPushButton(applyButtonName, this);
    d->applyButton->setObjectName(applyButtonName);

    d->saveButton = new QPushButton(saveButtonName, this);
    d->saveButton->setObjectName(saveButtonName);

    QWidget* cropToolBoxBody = new QWidget(this);
    QVBoxLayout* cropToolBoxLayout = new QVBoxLayout(cropToolBoxBody);
    cropToolBoxBody->setLayout(cropToolBoxLayout);
    addWidget(cropToolBoxBody);
    cropToolBoxLayout->addWidget(d->applyButton);
    connect(d->applyButton, SIGNAL(clicked()), this, SLOT(applyCrop()));
    cropToolBoxLayout->addWidget(d->saveButton);
    connect(d->saveButton, SIGNAL(clicked()), this, SLOT(saveCrop()));

    d->cropCallback = vtkSmartPointer<medCropCallback>::New();
    d->cropCallback->toolBox = this;

    d->view = 0;
    d->view2D = 0;
    d->view3D = 0;
}

medCropToolBox::~medCropToolBox()
{
    delete d;
}

dtkPlugin* medCropToolBox::plugin()
{
    return medPluginManager::instance()->plugin("Reformat");
}

medAbstractData* medCropToolBox::processOutput()
{
    if (d->view)
    {
        d->generateOutput();
    }
    return d->outputData.value(0);
}

void medCropToolBox::handleInteractionEvent()
{
    d->updateBoxWidgetFromBorderWidget();
}

void medCropToolBox::handleCameraMoveEvent()
{
    d->updateBorderWidgetFromBoxWidget();
}

void medCropToolBox::updateView()
{
    medViewContainer* currentContainer = this->getWorkspace()->stackedViewContainers()->getFirstSelectedContainer();
    medAbstractView *viewNotLayered = this->getWorkspace()->stackedViewContainers()->getFirstSelectedContainerView();
    medAbstractLayeredView* view = qobject_cast<medAbstractLayeredView*>(viewNotLayered);

    if (view)
    {
        if (view != d->view)
        {
            d->view = view;
            d->view2D = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
            d->view3D = static_cast<medVtkViewBackend*>(d->view->backend())->view3D;

            connect(d->view, SIGNAL(orientationChanged()), this, SLOT(reactToOrientationChange()), Qt::UniqueConnection);

            d->borderWidget->AddObserver(vtkCommand::InteractionEvent, d->cropCallback);
            d->borderWidget->SetInteractor(d->view2D->GetInteractor());
            d->borderWidget->On();

            d->orientationMatrix = d->view2D->GetOrientationMatrix();
            d->inverseOrientationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
            d->inverseOrientationMatrix->DeepCopy(d->orientationMatrix);
            d->inverseOrientationMatrix->Invert();

            d->view2D->GetInteractorStyle()->AddObserver(vtkImageView2DCommand::CameraMoveEvent, d->cropCallback);
        }
        d->updateBorderWidgetIfVisible();

        // do not allow to split the container
        currentContainer->setUserSplittable(false);
    }
    else
    {
        d->view = 0;
        d->view2D = 0;
        d->view3D = 0;
    }
}

void medCropToolBox::applyCrop()
{
    if (d->view)
    {
        d->generateOutput();
        d->replaceViewWithOutputData(*getWorkspace());
    }
}

void medCropToolBox::saveCrop()
{
    if (d->view)
    {
        d->importOutput();
    }
}

void medCropToolBox::reactToOrientationChange()
{
    if (d->view)
    {
        d->updateBorderWidgetIfVisible();
    }
}

void medCropToolBox::showEvent(QShowEvent *event)
{
    medAbstractSelectableToolBox::showEvent(event);
    setEnable(true);
}

void medCropToolBox::clear()
{
    setEnable(false);

    // allow to split container in other toolboxes
    medViewContainer* currentContainer = this->getWorkspace()->stackedViewContainers()->getFirstSelectedContainer();
    if (currentContainer)
    {
        currentContainer->setUserSplittable(true);
    }
}

void medCropToolBox::setEnable(bool enable)
{
    if (d->borderWidget)
    {
        d->borderWidget->GetBorderRepresentation()->SetVisibility(enable);
        d->borderWidget->SetProcessEvents(enable);
        if (d->borderWidget->GetCurrentRenderer() && d->borderWidget->GetCurrentRenderer()->GetRenderWindow())
        {
            d->borderWidget->GetCurrentRenderer()->GetRenderWindow()->Render();
        }
    }
}

void medCropToolBoxPrivate::updateBorderWidgetIfVisible()
{
    updateCurrentOrientation();
    if (borderWidget->GetCurrentRenderer() && borderWidget->GetCurrentRenderer()->GetRenderWindow())
    {
        updateBorderWidgetFromBoxWidget();
        borderWidget->GetCurrentRenderer()->GetRenderWindow()->Render();
    }
}

void medCropToolBoxPrivate::updateCurrentOrientation()
{
    currentOrientation = static_cast<medVtkViewBackend*>(view->backend())->view2D->GetSliceOrientation();
}

void medCropToolBoxPrivate::updateBoxWidgetFromBorderWidget()
{
    double minBoxCorner[3], maxBoxCorner[3], borderPoint1[3], borderPoint2[3], point1[3], point2[3];
    double *tempPoint;

    getMinAndMaxBoxCorners(minBoxCorner, maxBoxCorner);
    tempPoint = borderWidget->GetBorderRepresentation()->GetPosition();
    std::copy(tempPoint, tempPoint + 2, borderPoint1);
    tempPoint = borderWidget->GetBorderRepresentation()->GetPosition2();
    std::copy(tempPoint, tempPoint + 2, borderPoint2);

    borderPoint2[0] += borderPoint1[0];
    borderPoint2[1] += borderPoint1[1];

    normalizedViewportToWorld(borderPoint1, point1);
    normalizedViewportToWorld(borderPoint2, point2);

    applyOrientationMatrix(minBoxCorner, minBoxCorner);
    applyOrientationMatrix(maxBoxCorner, maxBoxCorner);

    vtkPlane::ProjectPoint(point1, minBoxCorner, view2D->GetRenderer()->GetActiveCamera()->GetViewPlaneNormal(), minBoxCorner);
    vtkPlane::ProjectPoint(point2, maxBoxCorner, view2D->GetRenderer()->GetActiveCamera()->GetViewPlaneNormal(), maxBoxCorner);

    applyInverseOrientationMatrix(minBoxCorner, minBoxCorner);
    applyInverseOrientationMatrix(maxBoxCorner, maxBoxCorner);

    adjustBoxCornersToAnnulPlaceFactor(maxBoxCorner, minBoxCorner);
    view3D->GetBoxWidget()->PlaceWidget(minBoxCorner[0], maxBoxCorner[0], minBoxCorner[1], maxBoxCorner[1], minBoxCorner[2], maxBoxCorner[2]);
}

void medCropToolBoxPrivate::adjustBoxCornersToAnnulPlaceFactor(double* maxBoxCorner, double* minBoxCorner)
{
    double center[3];
    double placeFactor = view3D->GetBoxWidget()->GetPlaceFactor();

    for (int i = 0; i < 3; i++)
    {
        center[i] = (minBoxCorner[i] + maxBoxCorner[i]) / 2.0;
        minBoxCorner[i] = center[i] + (minBoxCorner[i] - center[i]) / placeFactor;
        maxBoxCorner[i] = center[i] + (maxBoxCorner[i] - center[i]) / placeFactor;
    }
}

void medCropToolBoxPrivate::updateBorderWidgetFromBoxWidget()
{
    double point1[3], point2[3];

    getMinAndMaxBoxCorners(point1, point2);
    setBorderFromWorldPoints(point1, point2);
    borderWidget->GetBorderRepresentation()->Modified();
}

void medCropToolBoxPrivate::setBorderFromWorldPoints(double* worldPoint1, double* worldPoint2)
{
    double orientatedWorldPoint1[3], orientatedWorldPoint2[3], viewportPoint1[3], viewportPoint2[3];

    applyOrientationMatrix(worldPoint1, orientatedWorldPoint1);
    applyOrientationMatrix(worldPoint2, orientatedWorldPoint2);

    worldToNormalizedViewport(orientatedWorldPoint1, viewportPoint1);
    worldToNormalizedViewport(orientatedWorldPoint2, viewportPoint2);

    for (int i = 0; i < 2; i++)
    {
        if (viewportPoint2[i] < viewportPoint1[i])
        {
            std::swap(viewportPoint1[i], viewportPoint2[i]);
        }
    }

    borderWidget->GetBorderRepresentation()->SetPosition(viewportPoint1);
    borderWidget->GetBorderRepresentation()->SetPosition2(viewportPoint2[0] - viewportPoint1[0], viewportPoint2[1] - viewportPoint1[1]);
}

void medCropToolBoxPrivate::getMinAndMaxBoxCorners(double* minCorner, double* maxCorner)
{
    vtkPolyData* polyData = vtkPolyData::New();
    view3D->GetBoxWidget()->GetPolyData(polyData);
    polyData->GetPoint(0, minCorner);
    polyData->GetPoint(6, maxCorner);
    polyData->Delete();

    for (int i = 0; i < 2; i++)
    {
        if (minCorner[i] > maxCorner[i])
        {
            std::swap(minCorner[i], maxCorner[i]);
        }
    }
}

void medCropToolBoxPrivate::worldToNormalizedViewport(double* worldPoint, double* viewportPoint)
{
    vtkRenderer* renderer = borderWidget->GetCurrentRenderer();

    if (renderer)
    {
        renderer->SetWorldPoint(worldPoint);
        renderer->WorldToDisplay();
        renderer->GetDisplayPoint(viewportPoint);
        renderer->DisplayToNormalizedDisplay(viewportPoint[0], viewportPoint[1]);
        renderer->NormalizedDisplayToViewport(viewportPoint[0], viewportPoint[1]);
        renderer->ViewportToNormalizedViewport(viewportPoint[0], viewportPoint[1]);
    }
}

void medCropToolBoxPrivate::normalizedViewportToWorld(double* viewportPoint, double* worldPoint)
{
    vtkRenderer* renderer = borderWidget->GetCurrentRenderer();
    double homogeneousVector[4];

    if (renderer)
    {
        std::copy(viewportPoint, viewportPoint + 3, homogeneousVector);
        renderer->NormalizedViewportToViewport(homogeneousVector[0], homogeneousVector[1]);
        renderer->ViewportToNormalizedDisplay(homogeneousVector[0], homogeneousVector[1]);
        renderer->NormalizedDisplayToDisplay(homogeneousVector[0], homogeneousVector[1]);
        renderer->SetDisplayPoint(homogeneousVector);
        renderer->DisplayToWorld();
        renderer->GetWorldPoint(homogeneousVector);
        std::copy(homogeneousVector, homogeneousVector + 3, worldPoint);
    }
}

void medCropToolBoxPrivate::applyOrientationMatrix(double* worldPointIn, double* worldPointOut)
{
    double homogeneousVector[4];

    std::copy(worldPointIn, worldPointIn + 3, homogeneousVector);
    homogeneousVector[3] = 1;
    orientationMatrix->MultiplyPoint(homogeneousVector, homogeneousVector);
    std::copy(homogeneousVector, homogeneousVector + 3, worldPointOut);
}

void medCropToolBoxPrivate::applyInverseOrientationMatrix(double* worldPointIn, double* worldPointOut)
{
    double homogeneousVector[4];

    std::copy(worldPointIn, worldPointIn + 3, homogeneousVector);
    homogeneousVector[3] = 1;
    inverseOrientationMatrix->MultiplyPoint(homogeneousVector, homogeneousVector);
    std::copy(homogeneousVector, homogeneousVector + 3, worldPointOut);
}

void medCropToolBoxPrivate::generateOutput()
{
    double minBoxCorner[3], maxBoxCorner[3];
    int minIndices[3], maxIndices[3];

    getMinAndMaxBoxCorners(minBoxCorner, maxBoxCorner);
    applyOrientationMatrix(minBoxCorner, minBoxCorner);
    applyOrientationMatrix(maxBoxCorner, maxBoxCorner);
    view3D->GetImageCoordinatesFromWorldCoordinates(minBoxCorner, minIndices);
    view3D->GetImageCoordinatesFromWorldCoordinates(maxBoxCorner, maxIndices);

    outputData.clear();

    for (unsigned int layer = 0; layer < view->layersCount(); layer++)
    {
        medAbstractData* imageData = view->layerData(layer);
        medAbstractData* output = NULL;

        QStringList imageTypes;
        imageTypes << "itkDataImageChar3" << "itkDataImageUChar3" << "itkDataImageShort3" << "itkDataImageUShort3" << "itkDataImageInt3";
        imageTypes << "itkDataImageUInt3" << "itkDataImageLong3" << "itkDataImageULong3" << "itkDataImageFloat3" << "itkDataImageDouble3";

        switch (imageTypes.indexOf(imageData->identifier()))
        {
        case 0:
            output = extractCroppedImage<char, 3>(imageData, "itkDataImageChar3", minIndices, maxIndices);
            break;
        case 1:
            output = extractCroppedImage<unsigned char, 3>(imageData, "itkDataImageUChar3", minIndices, maxIndices);
            break;
        case 2:
            output = extractCroppedImage<short, 3>(imageData, "itkDataImageShort3", minIndices, maxIndices);
            break;
        case 3:
            output = extractCroppedImage<unsigned short, 3>(imageData, "itkDataImageUShort3", minIndices, maxIndices);
            break;
        case 4:
            output = extractCroppedImage<int, 3>(imageData, "itkDataImageInt3", minIndices, maxIndices);
            break;
        case 5:
            output = extractCroppedImage<unsigned int, 3>(imageData, "itkDataImageUInt3", minIndices, maxIndices);
            break;
        case 6:
            output = extractCroppedImage<long, 3>(imageData, "itkDataImageLong3", minIndices, maxIndices);
            break;
        case 7:
            output = extractCroppedImage<unsigned long, 3>(imageData, "itkDataImageULong3", minIndices, maxIndices);
            break;
        case 8:
            output = extractCroppedImage<float, 3>(imageData, "itkDataImageFloat3", minIndices, maxIndices);
            break;
        case 9:
            output = extractCroppedImage<double, 3>(imageData, "itkDataImageDouble3", minIndices, maxIndices);
            break;
        default:
            medMessageController::instance()->showError("Drop a 3D volume in the view",3000);
            qDebug()<<"medCropToolBoxPrivate::generateOutput id "<<imageData->identifier();
        }

        outputData.append(output);
    }
}

template <typename DATA_TYPE, unsigned int DIMENSIONS>
medAbstractData* medCropToolBoxPrivate::extractCroppedImage(medAbstractData* abstractData, const QString& destType, int* minIndices, int* maxIndices)
{
    typedef itk::Image<DATA_TYPE, DIMENSIONS> ImageType;
    typename ImageType::Pointer image = dynamic_cast<ImageType*>((itk::Object*)abstractData->data());
    typedef itk::RegionOfInterestImageFilter<ImageType, ImageType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();
    typename ImageType::IndexType desiredStart;
    typename ImageType::SizeType desiredSize;
    typename ImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();

    for (unsigned int i = 0; i < DIMENSIONS; i++)
    {
        if (minIndices[i] > maxIndices[i])
        {
            std::swap(minIndices[i], maxIndices[i]);
        }
        if (maxIndices[i] < 0 || minIndices[i] >= (int)imageSize[i])
        {
            return 0;
        }
        desiredStart[i] = std::max(minIndices[i], 0);
        desiredSize[i] = std::min(maxIndices[i] + 1, (int)imageSize[i]) - desiredStart[i];
    }

    typename ImageType::RegionType desiredRegion(desiredStart, desiredSize);
    filter->SetRegionOfInterest(desiredRegion);
    filter->SetInput(image);
    filter->Update();

    typename ImageType::Pointer filteredImage = filter->GetOutput();
    medAbstractData* output = medAbstractDataFactory::instance()->create(destType);
    output->setData(filteredImage);

    setOutputMetaData(output, abstractData, desiredSize[0], desiredSize[1]);

    return output;
}

void medCropToolBoxPrivate::setOutputMetaData(medAbstractData* output, medAbstractData* input, int columns, int rows)
{
    medUtilities::setDerivedMetaData(output, input, "cropped");
    output->setMetaData(medMetaDataKeys::Columns.key(), QString::number(columns));
    output->setMetaData(medMetaDataKeys::Rows.key(), QString::number(rows));
}

void medCropToolBoxPrivate::replaceViewWithOutputData(medAbstractWorkspace& workspace)
{
    if (outputData.length() >= 0)
    {
        medTabbedViewContainers* tabbedViewContainers = workspace.stackedViewContainers();
        medViewContainer* viewContainer = tabbedViewContainers->containersInTab(tabbedViewContainers->currentIndex()).at(0);

        viewContainer->removeView();
        viewContainer->setMultiLayered(true);
        viewContainer->addData(outputData[0]);
        medAbstractLayeredView* newView = qobject_cast<medAbstractLayeredView*>(viewContainer->view());
        for (int layer = 1; layer < outputData.length(); layer++)
        {
            newView->addLayer(outputData[layer]);
        }
    }
}

void medCropToolBoxPrivate::importOutput()
{
    foreach (medAbstractData* output, outputData)
    {
        medDataManager::instance()->importData(output, false);
    }
}
