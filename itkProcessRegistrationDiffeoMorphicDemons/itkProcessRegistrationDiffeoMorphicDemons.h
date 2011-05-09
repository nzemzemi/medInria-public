// /////////////////////////////////////////////////////////////////
// Generated by dtkPluginGenerator
// /////////////////////////////////////////////////////////////////

#ifndef ITKPROCESSREGISTRATIONDIFFEOMORPHICDEMONS_H
#define ITKPROCESSREGISTRATIONDIFFEOMORPHICDEMONS_H

#include <itkProcessRegistration/itkProcessRegistration.h>
#include "itkProcessRegistrationDiffeoMorphicDemonsPluginExport.h"

class itkProcessRegistrationDiffeoMorphicDemonsPrivate;

/**
 * @brief Registration process using diffeomorphic demons from itk.
 *
 * This plugin uses the registration programming interface (RPI).
 * It also implements a custom toolbox plugging itself onto the generic registration toolbox available in medinria/src/medGui.
 *
*/
class ITKPROCESSREGISTRATIONDIFFEOMORPHICDEMONSPLUGIN_EXPORT itkProcessRegistrationDiffeoMorphicDemons : public itkProcessRegistration
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     *
     * @param void
     */
    itkProcessRegistrationDiffeoMorphicDemons(void);

    /**
     * @brief
     *
     * @param void
    */
    virtual ~itkProcessRegistrationDiffeoMorphicDemons(void);

    /**
     * @brief Description of the plugin.
     *
     * @param void
     * @return QString
    */
    virtual QString description(void) const;

    /**
     * @brief tries to register the process with the factory.
     *
     * @param void
     * @return bool true if it succeeded, false otherwise.
    */
    static bool registered(void);

public:
    /**
     * @brief Runs the process.
     *
     * @param ImageType the fixed image image type.
     * @return int successful or not.
    */
    virtual int update(ImageType);

    /**
     * @brief
     *
     * @see rpiDiffeoMorphicDemons
     *
     * @param iterations
    */
    void setNumberOfIterations(std::vector <unsigned int> iterations);

    /**
     * @brief
     * @see rpiDiffeoMorphicDemons
     * @param updateRule
    */
    void setUpdateRule( unsigned char updateRule);

    /**
     * @brief
     * @see rpiDiffeoMorphicDemons
     * @param gradientType
    */
    void setGradientType (unsigned char gradientType);

    /**
     * @brief
     * @see rpiDiffeoMorphicDemons
     * @param maximumUpdateStepLength
    */
    void setMaximumUpdateLength (float maximumUpdateStepLength);

    /**
     * @brief
     * @see rpiDiffeoMorphicDemons
     * @param updateFieldStandardDeviation
    */
    void setUpdateFieldStandardDeviation(float updateFieldStandardDeviation);

    /**
     * @brief
     * @see rpiDiffeoMorphicDemons
     * @param displacementFieldStandardDeviation
    */
    void setDisplacementFieldStandardDeviation(float displacementFieldStandardDeviation);

    /**
     * @brief
     * @see rpiDiffeoMorphicDemons
     * @param useHistogramMatching
    */
    void setUseHistogramMatching(bool useHistogramMatching);

protected :
    /**
     * @brief Writes the transformation, in this case the displacement field,
     * in a file.
     *
     * @param file The path to the file is assumed to be existing. However the file may not exist beforehand.
     * @return bool successful or not.
    */
    virtual bool writeTransform(const QString& file);

private:
    itkProcessRegistrationDiffeoMorphicDemonsPrivate *d;

};

/**
 * @brief Function to instantiate the process from the factory.
 * @see registered()
 *
 * @param void
*/
dtkAbstractProcess *createitkProcessRegistrationDiffeoMorphicDemons(void);

#endif
